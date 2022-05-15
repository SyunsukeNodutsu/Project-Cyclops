//-----------------------------------------------------------------------------
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

class Texture;
class Buffer;

//グラフィックスデバイスクラス
class GraphicsDevice
{
public:

	//@brief コンストラクタ
	//@param createParam 作成情報
	GraphicsDevice(GRAPHICS_DEVICE_CREATE_PARAM createParam);

	//@brief デストラクタ
	~GraphicsDevice();

	//@brief 初期化
	bool Initialize();

	//@brief 終了
	bool Finalize();

	//@brief 描画開始
	void Begin();

	// @brief 描画終了と画面更新
	// @param syncInterval フレームの表示を垂直ブランクと同期する方法
	// @param flags スワップチェーン表示オプション
	void End(UINT syncInterval = 0, UINT flags = 0);

public://TODO: アクセス制限

	GRAPHICS_DEVICE_CREATE_PARAM m_createParam;

	ComPtr<ID3D11Device>		m_cpDevice;
	ComPtr<ID3D11DeviceContext> m_cpContext;		//即時コンテキスト

	ComPtr<IDXGISwapChain>		m_cpGISwapChain;
	ComPtr<IDXGIFactory1>		m_cpFactory;

	DXGI_SAMPLE_DESC			m_sampleDesc;

	D3D11_VIEWPORT				m_viewport;

	std::wstring				m_adapterName;		//GPUアダプタの名前

	std::shared_ptr<Texture>	m_spBackbuffer;
	std::shared_ptr<Texture>	m_spDefaultZbuffer;

private:

	bool CreateDevice();
	bool CreateSwapChain();
	bool CreateBackBuffer();

	IDXGIAdapter* CheckAdapter();
	void CheckMSAA();

};

/*enum SHADERSTAGE
{
	MS,
	AS,
	VS,
	HS,
	DS,
	GS,
	PS,
	CS,
	LIB,
	SHADERSTAGE_COUNT,
};*/
