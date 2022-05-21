﻿//-----------------------------------------------------------------------------
// File: GraphicsDevice.h
//
// エンジンのグラフィックス機能 DX12で実装したいなー
//-----------------------------------------------------------------------------
#pragma once

//グラフィックスデバイス作成情報
typedef struct GRAPHICS_DEVICE_CREATE_PARAM
{
	int BufferCount;
	int Width, Height;
	int RefreshRate;

	bool Windowed;
	bool UseHDR;
	bool UseMSAA;
	bool DebugMode;

	HWND Hwnd;
}GRAPHICS_DEVICE_CREATE_PARAM;

//シェーダーステージ
enum class SHADER_STAGE
{
	VS,
	PS,
	CS,
};

class Texture;
class Buffer;

//グラフィックスデバイスクラス
class GraphicsDevice
{
public:

	GraphicsDevice(GRAPHICS_DEVICE_CREATE_PARAM createParam);
	~GraphicsDevice() = default;

	bool Initialize();
	bool Finalize();

	void Begin(const Vector3 clearColor = Vector3(0.0f, 0.0f, 1.0f));
	void End(UINT syncInterval = 0, UINT flags = 0);

	void Resize(WPARAM wparam, UINT width, UINT height);
	void ToggleScreen(bool fullscreen);

public://TODO: アクセス制限 例)"ACCESS_ENGINE"みたいな

	GRAPHICS_DEVICE_CREATE_PARAM m_createParam;

	ComPtr<ID3D11Device>		m_cpDevice;
	ComPtr<ID3D11DeviceContext> m_cpContext;		//即時コンテキスト

	ComPtr<IDXGISwapChain>		m_cpGISwapChain;
	ComPtr<IDXGIFactory1>		m_cpFactory;
	ComPtr<IDXGIAdapter1>		m_cpAdapter;

	DXGI_SAMPLE_DESC			m_sampleDesc;

	D3D11_VIEWPORT				m_viewport;

	std::wstring				m_adapterName;		//GPUアダプタの名前

	std::shared_ptr<Texture>	m_spBackbuffer;
	std::shared_ptr<Texture>	m_spDefaultZbuffer;

private:

	bool CreateDevice();
	bool CreateSwapChain();
	bool CreateBackBuffer();
	bool CreateViewport();

	void CheckAdapter();
	void CheckMSAA();

};
