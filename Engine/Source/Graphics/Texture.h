//-----------------------------------------------------------------------------
// File: Texture.h
//
// テクスチャ ID3D11Texture2Dのラッパー
//-----------------------------------------------------------------------------
#pragma once
#include "GraphicsDeviceChild.h"

//テクスチャクラス
class Texture : public GraphicsDeviceChild
{
public:

	Texture();
	~Texture() = default;

	bool Create(ID3D11Texture2D* texture, bool useMSAA = false);
	bool Create(const D3D11_TEXTURE2D_DESC& desc, const D3D11_SUBRESOURCE_DATA* fillData = nullptr, bool useMSAA = false);

	bool CreateRenderTarget(int height, int width, bool useMSAA = false, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, UINT arrayCnt = 1);
	bool CreateDepthStencil(int height, int width, bool useMSAA = false, DXGI_FORMAT format = DXGI_FORMAT_R24G8_TYPELESS);

	inline ID3D11RenderTargetView*		RTV() const { return m_cpRTV.Get(); }
	inline ID3D11ShaderResourceView*	SRV() const { return m_cpSRV.Get(); }
	inline ID3D11DepthStencilView*		DSV() const { return m_cpDSV.Get(); }

	inline ID3D11RenderTargetView* const*	RTVAddress() const { return m_cpRTV.GetAddressOf(); }
	inline ID3D11ShaderResourceView* const* SRVAddress() const { return m_cpSRV.GetAddressOf(); }

	const D3D11_TEXTURE2D_DESC& GetDesc() const { return m_desc; }

private:

	ComPtr<ID3D11Texture2D>				m_cpBuffer;	//作成されたテクスチャバッファ
	D3D11_TEXTURE2D_DESC				m_desc;		//テクスチャ情報
	ComPtr<ID3D11RenderTargetView>		m_cpRTV;	//RTV
	ComPtr<ID3D11ShaderResourceView>	m_cpSRV;	//SRV
	ComPtr<ID3D11DepthStencilView>		m_cpDSV;	//DSV 深度バッファ書き込み用

private:

	bool CreateRTV(bool useMSAA = false);
	bool CreateDSV(bool useMSAA = false);
	bool KdCreateViewsFromTexture2D(ID3D11Texture2D* resource, ID3D11ShaderResourceView** ppSRV, ID3D11RenderTargetView** ppRTV, ID3D11DepthStencilView** ppDSV, bool useMSAA = false);

};
