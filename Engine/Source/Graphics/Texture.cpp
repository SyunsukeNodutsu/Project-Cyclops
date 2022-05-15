#include "Texture.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Texture::Texture()
	: m_cpBuffer(nullptr)
	, m_desc()
	, m_cpRTV(nullptr)
	, m_cpSRV(nullptr)
	, m_cpDSV(nullptr)
{
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Texture::Create(ID3D11Texture2D* texture, bool useMSAA)
{
	if (texture == nullptr) return false;

	m_cpBuffer = texture;
	texture->GetDesc(&m_desc);

	return CreateRTV(useMSAA);
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Texture::Create(const D3D11_TEXTURE2D_DESC& desc, const D3D11_SUBRESOURCE_DATA* fillData, bool useMSAA)
{
	HRESULT hr = g_graphicsDevice->m_cpDevice.Get()->CreateTexture2D(&desc, fillData, m_cpBuffer.GetAddressOf());
	if (FAILED(hr)) {
		assert(0 && "エラー：テクスチャバッファ作成失敗.");
		return false;
	}

	KdCreateViewsFromTexture2D(m_cpBuffer.Get(), m_cpSRV.GetAddressOf(), m_cpRTV.GetAddressOf(), m_cpDSV.GetAddressOf(), useMSAA);

	// DESC情報を取得
	m_cpBuffer->GetDesc(&m_desc);

	return true;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Texture::CreateRenderTarget(int height, int width, bool useMSAA, DXGI_FORMAT format, UINT arrayCnt)
{
	auto& smpleDesc = g_graphicsDevice->m_sampleDesc;

	D3D11_TEXTURE2D_DESC desc = {};

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = format;

	// ↓読み書き兼用で作成
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.Width = static_cast<UINT>(width);
	desc.Height = static_cast<UINT>(height);
	desc.CPUAccessFlags = 0;
	desc.MipLevels = 1;
	desc.ArraySize = arrayCnt;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = useMSAA ? smpleDesc.Count : 1;
	desc.SampleDesc.Quality = useMSAA ? smpleDesc.Quality : 0;

	if (!Create(desc, nullptr, useMSAA))
		return false;

	// Viewの作成
	if (!CreateRTV(useMSAA))
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Texture::CreateDepthStencil(int height, int width, bool useMSAA, DXGI_FORMAT format)
{
	auto& smpleDesc = g_graphicsDevice->m_sampleDesc;

	D3D11_TEXTURE2D_DESC desc = {};

	// 種類
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;

	desc.Height = static_cast<UINT>(height);
	desc.Width = static_cast<UINT>(width);

	// アクセスの種類
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;

	desc.MipLevels = 1;
	desc.ArraySize = 1;

	// その他の設定
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = useMSAA ? smpleDesc.Count : 1;
	desc.SampleDesc.Quality = useMSAA ? smpleDesc.Quality : 0;

	if (!Create(desc, nullptr, useMSAA))
		return false;

	// Viewの作成
	if (!CreateDSV(useMSAA))
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Texture::CreateRTV(bool useMSAA)
{
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = m_desc.Format;
	rtvDesc.ViewDimension = useMSAA ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;

	// RTV作成
	HRESULT hr = g_graphicsDevice->m_cpDevice.Get()->CreateRenderTargetView(m_cpBuffer.Get(), &rtvDesc, &m_cpRTV);
	if (FAILED(hr)) {
		assert(0 && "エラー：RenderTargetView作成失敗.");
		return false;
	}

	if (m_desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		// 作成するビューの設定
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

		// テクスチャがZバッファの場合は、最適なフォーマットにする
		if (m_desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			switch (m_desc.Format)
			{
			case DXGI_FORMAT_R16_TYPELESS: srvDesc.Format = DXGI_FORMAT_R16_UNORM; break;// 16ビット
			case DXGI_FORMAT_R32_TYPELESS: srvDesc.Format = DXGI_FORMAT_R32_FLOAT; break;// 32ビット
			case DXGI_FORMAT_R24G8_TYPELESS: srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;// 24ビット(Zバッファ) + 8ビット(ステンシルバッファ) 
			default: assert(0 && "[ShaderResource] 対応していないフォーマットです"); break;
			}
		}
		// Zバッファでない場合は、そのまま同じフォーマットを使用
		else srvDesc.Format = m_desc.Format;

		// 単品のテクスチャ(通常テクスチャ)の場合
		if (m_desc.ArraySize == 1)
		{
			srvDesc.ViewDimension = useMSAA ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = m_desc.MipLevels;
			if (srvDesc.Texture2D.MipLevels <= 0) srvDesc.Texture2D.MipLevels = -1;
		}
		// テクスチャ配列の場合
		else
		{
			// さらにキューブマップの場合
			if (m_desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE) srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			// 通常テクスチャ配列
			else srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;

			srvDesc.Texture2DArray.MostDetailedMip = 0;
			srvDesc.Texture2DArray.MipLevels = m_desc.MipLevels;
			srvDesc.Texture2DArray.ArraySize = m_desc.ArraySize;	// 要素数
			srvDesc.Texture2DArray.FirstArraySlice = 0;
		}

		// シェーダリソースビュー作成
		HRESULT hr = g_graphicsDevice->m_cpDevice.Get()->CreateShaderResourceView(m_cpBuffer.Get(), &srvDesc, m_cpSRV.GetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "ShaderResourceViewの作成に失敗");
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Texture::CreateDSV(bool useMSAA)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};

	//互換性を確認し最適なフォーマットを指定
	switch (m_desc.Format)
	{
		//16bit
	case DXGI_FORMAT_R16_TYPELESS: desc.Format = DXGI_FORMAT_R16_UNORM; break;
		//32bit
	case DXGI_FORMAT_R32_TYPELESS: desc.Format = DXGI_FORMAT_D32_FLOAT; break;
		//24bit(Zバッファ) + 8bit(ステンシルバッファ)
	case DXGI_FORMAT_R24G8_TYPELESS: desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; break;
		//非対応フォーマット
	default: assert(0 && "対応していないフォーマットです"); break;
	}

	//単品の場合
	desc.ViewDimension = useMSAA ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;

	//作成
	HRESULT hr = g_graphicsDevice->m_cpDevice.Get()->CreateDepthStencilView(m_cpBuffer.Get(), &desc, m_cpDSV.GetAddressOf());
	if (FAILED(hr)) {
		assert(0 && "エラー：DepthStencilView作成失敗.");
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Texture::KdCreateViewsFromTexture2D(ID3D11Texture2D* resource, ID3D11ShaderResourceView** ppSRV, ID3D11RenderTargetView** ppRTV, ID3D11DepthStencilView** ppDSV, bool useMSAA)
{
	if (resource == nullptr) return false;

	// テクスチャ本体の情報取得
	D3D11_TEXTURE2D_DESC desc;
	resource->GetDesc(&desc);

	//RenderTargetViewを作成する ================================================
	if (ppRTV && desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		//ビュー設定
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = desc.Format;

		//単品のテクスチャ(通常テクスチャ)の場合
		if (desc.ArraySize == 1)
		{
			rtvDesc.ViewDimension = useMSAA ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;//単品テクスチャ
		}
		//テクスチャ配列の場合
		else
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;	//テクスチャ配列
			rtvDesc.Texture2DArray.ArraySize = desc.ArraySize;						//要素数
			rtvDesc.Texture2DArray.FirstArraySlice = 0;
			rtvDesc.Texture2DArray.MipSlice = 0;
		}

		//レンダーターゲットビュー作成
		HRESULT hr = g_graphicsDevice->m_cpDevice.Get()->CreateRenderTargetView(resource, &rtvDesc, ppRTV);
		if (FAILED(hr)) {
			assert(0 && "RenderTargetViewの作成に失敗");
			return false;
		}
	}

	//ShaderResourceViewの情報を作成する ========================================
	if (ppSRV && desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

		// テクスチャがZバッファの場合は、最適なフォーマットにする
		if (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			switch (desc.Format)
			{
				//16ビット
			case DXGI_FORMAT_R16_TYPELESS: srvDesc.Format = DXGI_FORMAT_R16_UNORM; break;
				//32ビット
			case DXGI_FORMAT_R32_TYPELESS: srvDesc.Format = DXGI_FORMAT_R32_FLOAT; break;
				//24ビット(Zバッファ) + 8ビット(ステンシルバッファ) 
			case DXGI_FORMAT_R24G8_TYPELESS: srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;
				//非対応
			default: assert(0 && "[ShaderResource] 対応していないフォーマットです"); break;
			}
		}
		//Zバッファでない場合は そのまま同じフォーマットを使用
		else
		{
			srvDesc.Format = desc.Format;
		}

		//単品のテクスチャ(通常テクスチャ)の場合
		if (desc.ArraySize == 1)
		{
			srvDesc.ViewDimension = useMSAA ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;
			if (srvDesc.Texture2D.MipLevels <= 0) srvDesc.Texture2D.MipLevels = -1;
		}
		//テクスチャ配列の場合
		else
		{
			srvDesc.ViewDimension = (desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
				? D3D11_SRV_DIMENSION_TEXTURECUBE//さらにキューブマップの場合
				: D3D11_SRV_DIMENSION_TEXTURE2DARRAY;//通常テクスチャ配列
			srvDesc.Texture2DArray.MostDetailedMip = 0;
			srvDesc.Texture2DArray.MipLevels = desc.MipLevels;
			srvDesc.Texture2DArray.ArraySize = desc.ArraySize;
			srvDesc.Texture2DArray.FirstArraySlice = 0;
		}

		//シェーダリソースビュー作成
		HRESULT hr = g_graphicsDevice->m_cpDevice.Get()->CreateShaderResourceView(resource, &srvDesc, ppSRV);
		if (FAILED(hr)) {
			assert(0 && "ShaderResourceViewの作成に失敗");
			return false;
		}
	}

	//DepthStencilViewを作成する ================================================
	if (ppDSV && desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

		//テクスチャー作成時に指定したフォーマットと互換性があり、深度ステンシルビューとして指定できるフォーマットを指定する
		switch (desc.Format)
		{
			//16ビット
		case DXGI_FORMAT_R16_TYPELESS: dsvDesc.Format = DXGI_FORMAT_D16_UNORM; break;
			//32ビット
		case DXGI_FORMAT_R32_TYPELESS: dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; break;
			//24ビット(Zバッファ) + 8ビット(ステンシルバッファ) 
		case DXGI_FORMAT_R24G8_TYPELESS: dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; break;
			//非対応
		default: assert(0 && "[DepthStencil] 対応していないフォーマットです"); break;
		}

		//単品のテクスチャ(通常テクスチャ)の場合
		if (desc.ArraySize == 1)
		{
			dsvDesc.ViewDimension = useMSAA ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
		}
		//テクスチャ配列の場合
		else
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.Texture2DArray.ArraySize = desc.ArraySize;
			dsvDesc.Texture2DArray.MipSlice = 0;
			dsvDesc.Texture2DArray.FirstArraySlice = 0;
		}

		//デプスステンシルビュー作成
		HRESULT hr = g_graphicsDevice->m_cpDevice.Get()->CreateDepthStencilView(resource, &dsvDesc, ppDSV);
		if (FAILED(hr)) {
			assert(0 && "DepthStencilViewの作成に失敗");
			return false;
		}
	}

	return true;
}
