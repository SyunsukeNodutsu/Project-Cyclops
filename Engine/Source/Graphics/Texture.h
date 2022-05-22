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
	Texture(const std::wstring& filepath) { Load(filepath); }
	~Texture() = default;

	bool Load(const std::wstring& filepath, bool renderTarget = false, bool depthStencil = false, bool mipmap = true);

	bool Create(ID3D11Texture2D* pTexture2D);
	bool Create(const D3D11_TEXTURE2D_DESC& desc, const D3D11_SUBRESOURCE_DATA* fillData = nullptr);
	bool Create(int w, int h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, UINT arrayCnt = 1, const D3D11_SUBRESOURCE_DATA* fillData = nullptr);

	bool CreateRenderTarget(int w, int h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, UINT arrayCnt = 1, const D3D11_SUBRESOURCE_DATA* fillData = nullptr, UINT miscFlags = 0);
	bool CreateDepthStencil(int w, int h, DXGI_FORMAT format = DXGI_FORMAT_R24G8_TYPELESS, UINT arrayCnt = 1, const D3D11_SUBRESOURCE_DATA* fillData = nullptr, UINT miscFlags = 0);

	void SetSRView(ID3D11ShaderResourceView* srv);

	UINT GetWidth() const { return m_desc.Width; }
	UINT GetHeight() const { return m_desc.Height; }
	float GetAspectRatio() const { return (float)m_desc.Width / m_desc.Height; }

	const D3D11_TEXTURE2D_DESC& GetInfo() const { return m_desc; }
	const std::wstring& GetFilepath() const { return m_filepath; }

	ID3D11Texture2D* GetResource() const;

	inline ID3D11RenderTargetView* RTV() const { return m_cpRTV.Get(); }
	inline ID3D11RenderTargetView* const* RTVAddress() const { return m_cpRTV.GetAddressOf(); }
	inline ID3D11ShaderResourceView* SRV() const { return m_cpSRV.Get(); }
	inline ID3D11ShaderResourceView* const* SRVAddress() const { return m_cpSRV.GetAddressOf(); }
	inline ID3D11DepthStencilView* DSV() const { return m_cpDSV.Get(); }

private:

	ComPtr<ID3D11ShaderResourceView>	m_cpSRV;		//SRV
	ComPtr<ID3D11RenderTargetView>		m_cpRTV;		//RTV
	ComPtr<ID3D11DepthStencilView>		m_cpDSV;		//DSV 深度バッファ書き込み用
	D3D11_TEXTURE2D_DESC				m_desc;			//テクスチャ情報
	std::wstring						m_filepath;		//Load時のファイルパス

private:

	Texture(const Texture& src) = delete;
	void operator=(const Texture& src) = delete;

	bool CreateViewsFromTexture2D(ID3D11Texture2D* resource, ID3D11ShaderResourceView** ppSRV, ID3D11RenderTargetView** ppRTV, ID3D11DepthStencilView** ppDSV);

};;
