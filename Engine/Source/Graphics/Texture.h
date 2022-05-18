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
	Texture(const std::string& filename) { Load(filename); }

	~Texture() { Release(); }

	void Release();

	bool Load(const std::string& filename, bool renderTarget = false, bool depthStencil = false, bool generateMipmap = true);

	bool Create(ID3D11Texture2D* pTexture2D);
	bool Create(const D3D11_TEXTURE2D_DESC& desc, const D3D11_SUBRESOURCE_DATA* fillData = nullptr);
	bool Create(int w, int h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, UINT arrayCnt = 1, const D3D11_SUBRESOURCE_DATA* fillData = nullptr);

	bool CreateRenderTarget(int w, int h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, UINT arrayCnt = 1, const D3D11_SUBRESOURCE_DATA* fillData = nullptr, UINT miscFlags = 0);
	bool CreateDepthStencil(int w, int h, DXGI_FORMAT format = DXGI_FORMAT_R24G8_TYPELESS, UINT arrayCnt = 1, const D3D11_SUBRESOURCE_DATA* fillData = nullptr, UINT miscFlags = 0);

	void SetSRView(ID3D11ShaderResourceView* srv);

	float GetAspectRatio() const { return (float)m_desc.Width / m_desc.Height; }
	UINT GetWidth() const { return m_desc.Width; }
	UINT GetHeight() const { return m_desc.Height; }
	const D3D11_TEXTURE2D_DESC& GetInfo() const { return m_desc; }
	const std::string& GetFilepath() const { return m_filepath; }

	ID3D11Texture2D* GetResource() const;

	ID3D11ShaderResourceView* SRV() const { return m_srv; }
	ID3D11ShaderResourceView* const* SRVAddress() const { return &m_srv; }
	ID3D11RenderTargetView* RTV() const { return m_rtv; }
	ID3D11RenderTargetView* const* RTVAddress() const { return &m_rtv; }
	ID3D11DepthStencilView* DSV() const { return m_dsv; }

private:

	ID3D11ShaderResourceView* m_srv;
	ID3D11RenderTargetView* m_rtv;
	ID3D11DepthStencilView* m_dsv;
	D3D11_TEXTURE2D_DESC m_desc;

	std::string m_filepath;

private:

	Texture(const Texture& src) = delete;
	void operator=(const Texture& src) = delete;

	bool CreateViewsFromTexture2D(ID3D11Texture2D* resource, ID3D11ShaderResourceView** ppSRV, ID3D11RenderTargetView** ppRTV, ID3D11DepthStencilView** ppDSV);

};;
