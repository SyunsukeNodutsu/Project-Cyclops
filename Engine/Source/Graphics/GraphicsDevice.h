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
}GRAPHICS_DEVICE_CREATE_PARAM;

//シェーダーステージ
enum class SHADER_STAGE
{
	VS,
	PS,
	CS,
};

class ShaderManager;
class RendererStatus;
class ParticleSystem;
class Texture;
class Buffer;

//グラフィックスデバイスクラス
class GraphicsDevice : public ISubsystem
{
public:

	GraphicsDevice(GRAPHICS_DEVICE_CREATE_PARAM createParam);
	~GraphicsDevice() = default;

	bool Initialize();
	bool Finalize();

	void Begin(const float3 clearColor = float3(0.0f, 0.0f, 1.0f));
	void End(UINT syncInterval = 0, UINT flags = 0);

	void Resize(WPARAM wparam, UINT width, UINT height);
	void ToggleScreen(bool fullscreen);

	void DrawVertices(D3D_PRIMITIVE_TOPOLOGY topology, int vCount, const void* pVStream, UINT stride);

	const HRESULT CreateBufferSRV(ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut);
	const HRESULT CreateBufferUAV(ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut);

	std::shared_ptr<Texture> GetWhiteTex() const { return m_texWhite; }

public://TODO: アクセス制限 例)"ACCESS_ENGINE"みたいな

	GRAPHICS_DEVICE_CREATE_PARAM	m_createParam;
	std::shared_ptr<RendererStatus> m_spRendererStatus;
	std::shared_ptr<ShaderManager>	m_spShaderManager;
	std::shared_ptr<ParticleSystem> m_spParticleSystem;

	ComPtr<ID3D11Device>		m_cpDevice;
	ComPtr<ID3D11DeviceContext> m_cpContext;		//即時コンテキスト

	ComPtr<IDXGISwapChain>		m_cpGISwapChain;
	ComPtr<IDXGIFactory1>		m_cpFactory;
	ComPtr<IDXGIAdapter1>		m_cpAdapter;

	DXGI_SAMPLE_DESC			m_sampleDesc;

	D3D11_VIEWPORT				m_viewport;

	std::wstring				m_adapterName;		//GPUアダプタの名前

private:

	std::shared_ptr<Texture>	m_spBackbuffer;
	std::shared_ptr<Texture>	m_spDefaultZbuffer;

	std::shared_ptr<Texture>	m_texWhite;

	std::shared_ptr<Buffer>		m_spTempFixedVertexBuffer[10];
	std::shared_ptr<Buffer>		m_spTempVertexBuffer;

private:

	bool CreateDevice();
	bool CreateSwapChain();
	bool CreateBackBuffer();
	bool CreateViewport();

	void CheckAdapter();
	void CheckMSAA();

};
