#include "Texture.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Texture::Texture()
	: m_cpSRV(nullptr)
	, m_cpRTV(nullptr)
	, m_cpDSV(nullptr)
	, m_desc()
	, m_filepath(L"")
{
}

//-----------------------------------------------------------------------------
// リソースを返す
//-----------------------------------------------------------------------------
ID3D11Texture2D* Texture::GetResource() const
{
	if (m_cpSRV == nullptr) return nullptr;

	ID3D11Resource* res;
	m_cpSRV->GetResource(&res);

	ID3D11Texture2D* texture;
	if (FAILED(res->QueryInterface<ID3D11Texture2D>(&texture)))
	{
		res->Release();
		return nullptr;
	}
	res->Release();
	texture->Release();

	return texture;
}

//-----------------------------------------------------------------------------
// パスから読み込み
//-----------------------------------------------------------------------------
bool Texture::Load(const std::wstring& filepath, bool renderTarget, bool depthStencil, bool mipmap)
{
	if (m_graphicsDevice == nullptr) return false;
	if (m_graphicsDevice->m_cpDevice == nullptr) return false;
	if (filepath.empty()) return false;

	UINT bindFlags = 0;
	bindFlags |= D3D11_BIND_SHADER_RESOURCE;
	if (renderTarget)bindFlags |= D3D11_BIND_RENDER_TARGET;
	if (depthStencil)bindFlags |= D3D11_BIND_DEPTH_STENCIL;


	// ※DirectX Texライブラリを使用して画像を読み込む

	DirectX::TexMetadata meta;
	DirectX::ScratchImage image;

	bool bLoaded = false;

	// WIC画像読み込み
	//  WIC_FLAGS_ALL_FRAMES … gifアニメなどの複数フレームを読み込んでくれる
	if (SUCCEEDED(DirectX::LoadFromWICFile(filepath.c_str(), DirectX::WIC_FLAGS_ALL_FRAMES, &meta, image)))
	{
		bLoaded = true;
	}

	// DDS画像読み込み
	if (bLoaded == false) {
		if (SUCCEEDED(DirectX::LoadFromDDSFile(filepath.c_str(), DirectX::DDS_FLAGS_NONE, &meta, image)))
		{
			bLoaded = true;
		}
	}

	// TGA画像読み込み
	if (bLoaded == false) {
		if (SUCCEEDED(DirectX::LoadFromTGAFile(filepath.c_str(), &meta, image)))
		{
			bLoaded = true;
		}
	}

	// HDR画像読み込み
	if (bLoaded == false) {
		if (SUCCEEDED(DirectX::LoadFromHDRFile(filepath.c_str(), &meta, image)))
		{
			bLoaded = true;
		}
	}

	// 読み込み失敗
	if (bLoaded == false)
	{
		return false;
	}

	// ミップマップ生成
	if (meta.mipLevels == 1 && mipmap)
	{
		DirectX::ScratchImage mipChain;
		if (SUCCEEDED(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain)))
		{
			image.Release();
			image = std::move(mipChain);
		}
	}

	//------------------------------------
	// テクスチャリソース作成
	//------------------------------------
	ID3D11Texture2D* tex2D = nullptr;
	HRESULT hr = DirectX::CreateTextureEx(
		m_graphicsDevice->m_cpDevice.Get(),
		image.GetImages(), image.GetImageCount(), image.GetMetadata(),
		D3D11_USAGE_DEFAULT,//Usage
		bindFlags,//BindFlags
		0, 0, false,
		(ID3D11Resource**)&tex2D);

	if (FAILED(hr))
	{
		Debug::Log("CreateTextureEx失敗."); return false;
	}

	//各ビュー作成
	if (!CreateViewsFromTexture2D(tex2D, m_cpSRV.GetAddressOf(), m_cpRTV.GetAddressOf(), m_cpDSV.GetAddressOf()))
	{
		tex2D->Release();
		return false;
	}

	if (tex2D == nullptr)
	{
		Debug::Log("aaaa");
	}

	// 画像情報取得
	tex2D->GetDesc(&m_desc);
	tex2D->Release();

	m_filepath = filepath;

	Debug::Log(L"テクスチャ読み込み: " + m_filepath);

	return true;
}

//-----------------------------------------------------------------------------
// リソースから作成
//-----------------------------------------------------------------------------
bool Texture::Create(ID3D11Texture2D* pTexture2D)
{
	if (pTexture2D == nullptr) return false;

	if (!CreateViewsFromTexture2D(pTexture2D, m_cpSRV.GetAddressOf(), m_cpRTV.GetAddressOf(), m_cpDSV.GetAddressOf()))
	{
		Debug::Log("CreateViewsFromTexture2D失敗."); return false;
	}

	pTexture2D->GetDesc(&m_desc);
	m_filepath = L"";

	return true;
}

//-----------------------------------------------------------------------------
// DESCから作成
//-----------------------------------------------------------------------------
bool Texture::Create(const D3D11_TEXTURE2D_DESC& desc, const D3D11_SUBRESOURCE_DATA* fillData)
{
	if (m_graphicsDevice == nullptr) return false;
	if (m_graphicsDevice->m_cpDevice == nullptr) return false;

	ID3D11Texture2D* resource;
	if (FAILED(m_graphicsDevice->m_cpDevice->CreateTexture2D(&desc, fillData, &resource)))
	{
		Debug::Log("CreateTexture2D失敗."); return false;
	}

	if (CreateViewsFromTexture2D(resource, m_cpSRV.GetAddressOf(), m_cpRTV.GetAddressOf(), m_cpDSV.GetAddressOf()) == false)
	{
		Debug::Log("CreateViewsFromTexture2D失敗."); return false;
	}

	resource->GetDesc(&m_desc);
	resource->Release();

	return true;
}

//-----------------------------------------------------------------------------
// フォーマットから作成
//-----------------------------------------------------------------------------
bool Texture::Create(int w, int h, DXGI_FORMAT format, UINT arrayCnt, const D3D11_SUBRESOURCE_DATA* fillData)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width				= static_cast<UINT>(w);
	desc.Height				= static_cast<UINT>(h);
	desc.Format				= format;
	desc.ArraySize			= arrayCnt;
	desc.MiscFlags			= 0;

	desc.Usage				= D3D11_USAGE_DEFAULT;
	desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags		= 0;
	desc.MipLevels			= 1;
	desc.SampleDesc.Count	= 1;
	desc.SampleDesc.Quality = 0;

	return Create(desc, fillData);
}

//-----------------------------------------------------------------------------
// レンダーターゲットとして作成
//-----------------------------------------------------------------------------
bool Texture::CreateRenderTarget(int w, int h, DXGI_FORMAT format, UINT arrayCnt, const D3D11_SUBRESOURCE_DATA* fillData, UINT miscFlags)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width				= static_cast<UINT>(w);
	desc.Height				= static_cast<UINT>(h);
	desc.Format				= format;
	desc.ArraySize			= arrayCnt;
	desc.MiscFlags			= miscFlags;

	desc.Usage				= D3D11_USAGE_DEFAULT;
	desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags		= 0;
	desc.MipLevels			= 1;
	desc.SampleDesc.Count	= 1;
	desc.SampleDesc.Quality = 0;

	return Create(desc, fillData);
}

//-----------------------------------------------------------------------------
// 深度テクスチャとして作成
//-----------------------------------------------------------------------------
bool Texture::CreateDepthStencil(int w, int h, DXGI_FORMAT format, UINT arrayCnt, const D3D11_SUBRESOURCE_DATA* fillData, UINT miscFlags)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width				= static_cast<UINT>(w);
	desc.Height				= static_cast<UINT>(h);
	desc.Format				= format;
	desc.ArraySize			= arrayCnt;
	desc.MiscFlags			= miscFlags;

	desc.Usage				= D3D11_USAGE_DEFAULT;
	desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags		= 0;
	desc.MipLevels			= 1;
	desc.SampleDesc.Count	= 1;
	desc.SampleDesc.Quality = 0;

	return Create(desc, fillData);
}

//-----------------------------------------------------------------------------
// SRV設定
//-----------------------------------------------------------------------------
void Texture::SetSRView(ID3D11ShaderResourceView* srv)
{
	if (srv == nullptr) return;

	m_cpSRV = srv;
	m_cpSRV->AddRef();//参照カウンタを増やしておく
	GetResource()->GetDesc(&m_desc);
}

//-----------------------------------------------------------------------------
// リソースから最適なビューを作成
//-----------------------------------------------------------------------------
bool Texture::CreateViewsFromTexture2D(ID3D11Texture2D* resource, ID3D11ShaderResourceView** ppSRV, ID3D11RenderTargetView** ppRTV, ID3D11DepthStencilView** ppDSV)
{
	if (m_graphicsDevice == nullptr) return false;
	if (m_graphicsDevice->m_cpDevice == nullptr) return false;
	if (resource == nullptr) return false;

	D3D11_TEXTURE2D_DESC desc;
	resource->GetDesc(&desc);

	//RTVを作成
	if (ppRTV && desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		D3D11_RENDER_TARGET_VIEW_DESC RTV_desc{};
		RTV_desc.Format = desc.Format;

		//単品のテクスチャ(通常テクスチャ)の場合
		if (desc.ArraySize == 1)
		{
			RTV_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		}
		//テクスチャ配列の場合
		else
		{
			RTV_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			RTV_desc.Texture2DArray.ArraySize = desc.ArraySize;
			RTV_desc.Texture2DArray.FirstArraySlice = 0;
			RTV_desc.Texture2DArray.MipSlice = 0;
		}

		if (FAILED(m_graphicsDevice->m_cpDevice->CreateRenderTargetView(resource, &RTV_desc, ppRTV)))
		{
			Debug::Log("CreateRenderTargetView失敗."); return false;
		}
	}

	//SRVを作成
	if (ppSRV && desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRV_desc{};

		//テクスチャがZバッファの場合は 最適なフォーマットにする
		if (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			switch (desc.Format)
			{
			case DXGI_FORMAT_R16_TYPELESS: SRV_desc.Format = DXGI_FORMAT_R16_UNORM; break;//16ビット
			case DXGI_FORMAT_R32_TYPELESS: SRV_desc.Format = DXGI_FORMAT_R32_FLOAT; break;//32ビット
			case DXGI_FORMAT_R24G8_TYPELESS: SRV_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;//24ビット(Zバッファ) + 8ビット(ステンシルバッファ) 
			default: Debug::Log("非対応フォーマット."); break/*return false*/;
			}
		}
		//Zバッファでない場合は、そのまま同じフォーマットを使用
		else
		{
			SRV_desc.Format = desc.Format;
		}

		//単品のテクスチャ(通常テクスチャ)の場合
		if (desc.ArraySize == 1)
		{
			SRV_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRV_desc.Texture2D.MostDetailedMip = 0;
			SRV_desc.Texture2D.MipLevels = desc.MipLevels;
			if (SRV_desc.Texture2D.MipLevels <= 0) SRV_desc.Texture2D.MipLevels = -1;
		}
		//テクスチャ配列の場合
		else
		{
			SRV_desc.ViewDimension = (desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
				? D3D11_SRV_DIMENSION_TEXTURECUBE//キューブマップ
				: D3D11_SRV_DIMENSION_TEXTURE2DARRAY;

			SRV_desc.Texture2DArray.MostDetailedMip = 0;
			SRV_desc.Texture2DArray.MipLevels = desc.MipLevels;
			SRV_desc.Texture2DArray.ArraySize = desc.ArraySize;
			SRV_desc.Texture2DArray.FirstArraySlice = 0;
		}

		if (FAILED(m_graphicsDevice->m_cpDevice->CreateShaderResourceView(resource, &SRV_desc, ppSRV)))
		{
			Debug::Log("CreateShaderResourceView失敗."); return false;
		}
	}

	//DSVを作成
	if (ppDSV && desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC DSV_desc{};

		//テクスチャー作成時に指定したフォーマットと互換性があり、深度ステンシルビューとして指定できるフォーマットを指定する
		switch (desc.Format)
		{
		case DXGI_FORMAT_R16_TYPELESS: DSV_desc.Format = DXGI_FORMAT_D16_UNORM; break;//16ビット
		case DXGI_FORMAT_R32_TYPELESS: DSV_desc.Format = DXGI_FORMAT_D32_FLOAT; break;//32ビット
		case DXGI_FORMAT_R24G8_TYPELESS: DSV_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; break;//24ビット(Zバッファ) + 8ビット(ステンシルバッファ) 
		default: Debug::Log("非対応フォーマット."); break/*return false*/;
		}

		//単品テクスチャの場合
		if (desc.ArraySize == 1)
		{
			DSV_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			DSV_desc.Texture2D.MipSlice = 0;
		}
		//テクスチャ配列の場合
		else
		{
			DSV_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			DSV_desc.Texture2DArray.ArraySize = desc.ArraySize;
			DSV_desc.Texture2DArray.MipSlice = 0;
			DSV_desc.Texture2DArray.FirstArraySlice = 0;
		}

		if (FAILED(m_graphicsDevice->m_cpDevice->CreateDepthStencilView(resource, &DSV_desc, ppDSV)))
		{
			Debug::Log("CreateDepthStencilView失敗."); return false;
		}
	}

	return true;
}