#include "Texture.h"

void SetTextureInfo(D3D11_TEXTURE2D_DESC& rDesc, int w, int h, DXGI_FORMAT format, UINT arrayCnt, UINT miscFlags)
{
	rDesc.Width = (UINT)w;
	rDesc.Height = (UINT)h;
	rDesc.Format = format;
	rDesc.ArraySize = arrayCnt;
	rDesc.MiscFlags = miscFlags;
}

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Texture::Texture()
	: m_srv(nullptr)
	, m_rtv(nullptr)
	, m_dsv(nullptr)
	, m_desc()
	, m_filepath("")
{
}

//-----------------------------------------------------------------------------
// 解放
//-----------------------------------------------------------------------------
void Texture::Release()
{
	SafeRelease(m_srv);
	SafeRelease(m_rtv);
	SafeRelease(m_dsv);
	m_filepath = "";
}

//-----------------------------------------------------------------------------
// リソースを取得
//-----------------------------------------------------------------------------
ID3D11Texture2D* Texture::GetResource() const
{
	if (m_srv == nullptr) return nullptr;

	ID3D11Resource* res;
	m_srv->GetResource(&res);

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
// 
//-----------------------------------------------------------------------------
bool Texture::Load(const std::string& filename, bool renderTarget, bool depthStencil, bool generateMipmap)
{
	return false;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Texture::Create(ID3D11Texture2D* pTexture2D)
{
	Release();
	if (pTexture2D == nullptr) return false;

	if (!CreateViewsFromTexture2D(pTexture2D, &m_srv, &m_rtv, &m_dsv))
	{
		Release(); Debug::Log("CreateViewsFromTexture2D失敗."); return false;
	}

	pTexture2D->GetDesc(&m_desc);
	m_filepath = "";

	return true;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Texture::Create(const D3D11_TEXTURE2D_DESC& desc, const D3D11_SUBRESOURCE_DATA* fillData)
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpDevice == nullptr) return;

	Release();

	ID3D11Texture2D* resource;
	if (FAILED(m_graphicsDevice->m_cpDevice->CreateTexture2D(&desc, fillData, &resource)))
	{
		Release(); Debug::Log("CreateTexture2D失敗."); return false;
	}

	if (CreateViewsFromTexture2D(resource, &m_srv, &m_rtv, &m_dsv) == false)
	{
		Release(); Debug::Log("CreateViewsFromTexture2D失敗."); return false;
	}

	resource->GetDesc(&m_desc);
	resource->Release();

	return true;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Texture::Create(int w, int h, DXGI_FORMAT format, UINT arrayCnt, const D3D11_SUBRESOURCE_DATA* fillData)
{
	Release();

	D3D11_TEXTURE2D_DESC desc;
	//SetTextureInfo(desc, w, h, format, arrayCnt, 0);
	desc.Usage				= D3D11_USAGE_DEFAULT;
	desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags		= 0;
	desc.MipLevels			= 1;
	desc.SampleDesc.Count	= 1;
	desc.SampleDesc.Quality = 0;

	return Create(desc, fillData);
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Texture::CreateRenderTarget(int w, int h, DXGI_FORMAT format, UINT arrayCnt, const D3D11_SUBRESOURCE_DATA* fillData, UINT miscFlags)
{
	Release();

	D3D11_TEXTURE2D_DESC desc;
	SetTextureInfo(desc, w, h, format, arrayCnt, 0);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	return Create(desc, fillData);
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Texture::CreateDepthStencil(int w, int h, DXGI_FORMAT format, UINT arrayCnt, const D3D11_SUBRESOURCE_DATA* fillData, UINT miscFlags)
{
	Release();

	D3D11_TEXTURE2D_DESC desc;
	SetTextureInfo(desc, w, h, format, arrayCnt, 0);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	return Create(desc, fillData);
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void Texture::SetSRView(ID3D11ShaderResourceView* srv)
{
	if (srv == nullptr) return;

	Release();
	m_srv = srv;
	m_srv->AddRef();//参照カウンタを増やしておく
	GetResource()->GetDesc(&m_desc);
}

//-----------------------------------------------------------------------------
// リソースから最適なビューを作成
//-----------------------------------------------------------------------------
bool Texture::CreateViewsFromTexture2D(ID3D11Texture2D* resource, ID3D11ShaderResourceView** ppSRV, ID3D11RenderTargetView** ppRTV, ID3D11DepthStencilView** ppDSV)
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpDevice == nullptr) return;
	if (resource == nullptr) return false;

	//テクスチャ本体の情報取得
	D3D11_TEXTURE2D_DESC desc;
	resource->GetDesc(&desc);

	//===========================================================
	//
	// RenderTargetViewを作成する
	//
	//===========================================================

	// レンダーターゲットフラグがついてる時のみ
	if (ppRTV && desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		// 作成するビューの設定
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = desc.Format;	// Format
		// 単品のテクスチャ(通常テクスチャ)の場合
		if (desc.ArraySize == 1) {
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;			// 単品テクスチャ
		}
		// テクスチャ配列の場合
		else {
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;		// テクスチャ配列
			rtvDesc.Texture2DArray.ArraySize = desc.ArraySize;			// 要素数
			rtvDesc.Texture2DArray.FirstArraySlice = 0;
			rtvDesc.Texture2DArray.MipSlice = 0;
		}

		// レンダーターゲットビュー作成
		HRESULT hr = m_graphicsDevice->m_cpDevice->CreateRenderTargetView(resource, &rtvDesc, ppRTV);
		if (FAILED(hr))
		{
			assert(0 && "RenderTargetViewの作成に失敗");
			return false;
		}
	}

	//===========================================================
	//
	// ShaderResourceViewの情報を作成する
	//
	//===========================================================
	// シェーダリソースビューフラグがついてる時のみ
	if (ppSRV && desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		// 作成するビューの設定
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

		// テクスチャがZバッファの場合は、最適なフォーマットにする
		if (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			switch (desc.Format) {
				// 16ビット
			case DXGI_FORMAT_R16_TYPELESS:
				srvDesc.Format = DXGI_FORMAT_R16_UNORM;
				break;
				// 32ビット
			case DXGI_FORMAT_R32_TYPELESS:
				srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				break;
				// 24ビット(Zバッファ) + 8ビット(ステンシルバッファ) 
			case DXGI_FORMAT_R24G8_TYPELESS:
				srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				break;
			default:
				assert(0 && "[ShaderResource] 対応していないフォーマットです");
				break;
			}
		}
		// Zバッファでない場合は、そのまま同じフォーマットを使用
		else
		{
			srvDesc.Format = desc.Format;
		}

		// 単品のテクスチャ(通常テクスチャ)の場合
		if (desc.ArraySize == 1)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;
			if (srvDesc.Texture2D.MipLevels <= 0)srvDesc.Texture2D.MipLevels = -1;
		}
		// テクスチャ配列の場合
		else {
			// さらにキューブマップの場合
			if (desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE) {
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			}
			// 通常テクスチャ配列
			else {
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			}
			srvDesc.Texture2DArray.MostDetailedMip = 0;
			srvDesc.Texture2DArray.MipLevels = desc.MipLevels;
			srvDesc.Texture2DArray.ArraySize = desc.ArraySize;	// 要素数
			srvDesc.Texture2DArray.FirstArraySlice = 0;
		}

		// シェーダリソースビュー作成
		HRESULT hr = m_graphicsDevice->m_cpDevice->CreateShaderResourceView(resource, &srvDesc, ppSRV);
		if (FAILED(hr))
		{
			assert(0 && "ShaderResourceViewの作成に失敗");
			return false;
		}
	}

	//===========================================================
	//
	// DepthStencilViewを作成する
	//
	//===========================================================
	// Zバッファフラグがついてる時のみ
	if (ppDSV && desc.BindFlags & D3D11_BIND_DEPTH_STENCIL) {
		// 作成するビューの設定
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

		// テクスチャー作成時に指定したフォーマットと互換性があり、深度ステンシルビューとして指定できるフォーマットを指定する
		switch (desc.Format) {
			// 16ビット
		case DXGI_FORMAT_R16_TYPELESS:
			dsvDesc.Format = DXGI_FORMAT_D16_UNORM;
			break;
			// 32ビット
		case DXGI_FORMAT_R32_TYPELESS:
			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
			break;
			// 24ビット(Zバッファ) + 8ビット(ステンシルバッファ) 
		case DXGI_FORMAT_R24G8_TYPELESS:
			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		default:
			assert(0 && "[DepthStencil] 対応していないフォーマットです");
			break;
		}

		// 単品のテクスチャ(通常テクスチャ)の場合
		if (desc.ArraySize == 1) {
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
		}
		// テクスチャ配列の場合
		else {
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.Texture2DArray.ArraySize = desc.ArraySize;
			dsvDesc.Texture2DArray.MipSlice = 0;
			dsvDesc.Texture2DArray.FirstArraySlice = 0;
		}

		//-------------------------------
		// デプスステンシルビュー作成
		//-------------------------------
		HRESULT hr = m_graphicsDevice->m_cpDevice->CreateDepthStencilView(resource, &dsvDesc, ppDSV);
		if (FAILED(hr)) {
			assert(0 && "DepthStencilViewの作成に失敗");
			return false;
		}
	}

	return true;
}
