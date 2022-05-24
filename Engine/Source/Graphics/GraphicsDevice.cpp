#include "GraphicsDevice.h"
#include "RendererStatus.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GraphicsDevice::GraphicsDevice(GRAPHICS_DEVICE_CREATE_PARAM createParam)
	: m_createParam(createParam)
	, m_spRendererStatus(nullptr)
	, m_spShaderManager(nullptr)
	, m_cpDevice(nullptr)
	, m_cpContext(nullptr)
	, m_cpGISwapChain(nullptr)
	, m_cpFactory(nullptr)
	, m_cpAdapter(nullptr)
	, m_sampleDesc()
	, m_adapterName(L"")
	, m_spBackbuffer(nullptr)
	, m_spDefaultZbuffer(nullptr)
	, m_spTempFixedVertexBuffer()
	, m_spTempVertexBuffer()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool GraphicsDevice::Initialize()
{
	//DXGIファクトリー作成
	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(m_cpFactory.GetAddressOf()))))
	{
		Debug::LogError("DXGIファクトリー作成失敗."); return false;
	}

	if (!CreateDevice()) { Debug::LogError("デバイス/デバイスコンテキスト作成失敗."); return false; }
	if (!CreateSwapChain()) { Debug::LogError("スワップチェイン作成失敗."); return false; }
	if (!CreateBackBuffer()) { Debug::LogError("バックバッファー作成失敗."); return false; }
	if (!CreateViewport()) { Debug::LogError("ビューポート変換行列の登録失敗."); return false; }

	//初期レンダーターゲット設定
	m_cpContext->OMSetRenderTargets(1, m_spBackbuffer->RTVAddress(), m_spDefaultZbuffer->DSV());

	//所持マネージャ初期化
	m_spRendererStatus = std::make_shared<RendererStatus>();
	if (!m_spRendererStatus->Initialize()) { Debug::LogError("RendererStatusの初期化失敗."); return false; }

	m_spShaderManager = std::make_shared<ShaderManager>();
	if (!m_spShaderManager->Initialize()) { Debug::LogError("ShaderManagerの初期化失敗."); return false; }

	//使いまわしバッファ
	UINT bufferSize = 80;
	for (int i = 0; i < 10; i++)
	{
		m_spTempFixedVertexBuffer[i] = std::make_shared<Buffer>();
		m_spTempFixedVertexBuffer[i]->Create(D3D11_BIND_VERTEX_BUFFER, bufferSize, D3D11_USAGE_DYNAMIC, nullptr);
		bufferSize *= 2;
	}
	m_spTempVertexBuffer = std::make_shared<Buffer>();

	return true;
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
bool GraphicsDevice::Finalize()
{
	m_spBackbuffer = m_spDefaultZbuffer = nullptr;

	//一般的にはデバイスのRelease前に配置するらしい
#if _DEBUG && 0
	ID3D11Debug* d3dDebug;
	HRESULT hr = m_cpDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	if (SUCCEEDED(hr))
	{
		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
	if (d3dDebug != nullptr) d3dDebug->Release();
#endif

	return true;
}

//-----------------------------------------------------------------------------
// デバイスとデバイスコンテキスト作成
//-----------------------------------------------------------------------------
bool GraphicsDevice::CreateDevice()
{
	D3D_FEATURE_LEVEL featureLevel;
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	UINT flags = 0;

	//詳細なデバッグ情報を取得する
	if (m_createParam.DebugMode)
		flags |= D3D11_CREATE_DEVICE_DEBUG;

	//アダプタ確認/取得
	CheckAdapter();

	//デバイスとデバイスコンテキスト作成
	if (FAILED(D3D11CreateDevice(m_cpAdapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags, featureLevels,
		_countof(featureLevels), D3D11_SDK_VERSION, &m_cpDevice, &featureLevel, &m_cpContext)))
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// 描画開始
//-----------------------------------------------------------------------------
void GraphicsDevice::Begin(const Vector3 clearColor)
{
	float clear[] = { clearColor.x, clearColor.y, clearColor.z };
	m_cpContext->ClearRenderTargetView(m_spBackbuffer->RTV(), clear);
	m_cpContext->ClearDepthStencilView(m_spDefaultZbuffer->DSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	m_cpContext->RSSetViewports(1, &m_viewport);

	m_cpContext->OMSetRenderTargets(1, m_spBackbuffer->RTVAddress(), m_spDefaultZbuffer->DSV());
}

//-----------------------------------------------------------------------------
// 描画終了
//-----------------------------------------------------------------------------
void GraphicsDevice::End(UINT syncInterval, UINT flags)
{
	//TODO: 全画面だと垂直同期が切れる
	HRESULT hr = m_cpGISwapChain->Present(syncInterval, flags);

	if (FAILED(hr))
		assert(0 && "エラー：画面更新の失敗.");
	if (hr == DXGI_ERROR_DEVICE_REMOVED)
		assert(0 && "エラー：ビデオカードがシステムから物理的に取り外されたか アップデートが行われました");
}

//-----------------------------------------------------------------------------
// 画面のリサイズ
//-----------------------------------------------------------------------------
void GraphicsDevice::Resize(WPARAM wparam, UINT width, UINT height)
{
	if (width == m_createParam.Width && height == m_createParam.Height) return;//サイズが変更されていない
	if (m_cpGISwapChain == nullptr) return;

	m_createParam.Width = width;
	m_createParam.Height = height;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	m_cpGISwapChain->GetDesc(&swapChainDesc);
	if (!swapChainDesc.Windowed)
		return;

	m_spBackbuffer.reset(); m_spBackbuffer = nullptr;
	m_spDefaultZbuffer.reset(); m_spDefaultZbuffer = nullptr;

	if (FAILED(m_cpGISwapChain->ResizeBuffers(swapChainDesc.BufferCount, width, height, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags)))
	{
		Debug::LogError("ResizeBuffers失敗."); return;
	}

	if (!CreateBackBuffer())
	{
		Debug::LogError("バックバッファー作成失敗."); return;
	}

	CreateViewport();
}

//-----------------------------------------------------------------------------
// フルスクリーン切り替え
//-----------------------------------------------------------------------------
void GraphicsDevice::ToggleScreen(bool fullscreen)
{
	DXGI_SWAP_CHAIN_DESC desc; m_cpGISwapChain->GetDesc(&desc);

	ComPtr<IDXGIOutput> pOutput;
	if (m_cpAdapter->EnumOutputs(0, pOutput.GetAddressOf()) == DXGI_ERROR_NOT_FOUND)
	{
		Debug::LogError("アダプターの出力先が見つかりません."); return;
	}
	if (FAILED(m_cpGISwapChain->SetFullscreenState(fullscreen, NULL)))
	{
		Debug::LogError("SetFullscreenState失敗."); return;
	}

	Resize(WPARAM(), 1920, 1080);
}

//-----------------------------------------------------------------------------
// 頂点描画用ヘルパー関数
//-----------------------------------------------------------------------------
void GraphicsDevice::DrawVertices(D3D_PRIMITIVE_TOPOLOGY topology, int vCount, const void* pVStream, UINT stride)
{
	const UINT totalSize = vCount * stride;

	//最適な固定長バッファを検索
	std::shared_ptr<Buffer> buffer = nullptr;
	for (auto&& n : m_spTempFixedVertexBuffer)
	{
		if (totalSize < n->GetSize()) {
			buffer = n; break;
		}
	}

	//可変長のバッファを使用
	if (buffer == nullptr)
	{
		Debug::Log("固定長バッファの範囲外 バッファを新規作成します. totalSize: " + ToString(totalSize));
		buffer = m_spTempVertexBuffer;

		// 頂点バッファのサイズが小さいときは、リサイズのため再作成する
		if (m_spTempVertexBuffer->GetSize() < totalSize)
			m_spTempVertexBuffer->Create(D3D11_BIND_VERTEX_BUFFER, totalSize, D3D11_USAGE_DYNAMIC, nullptr);
	}

	// 単純なDISCARDでの書き込み TODO: 修正
	buffer->WriteData(pVStream, totalSize);

	// バインド
	{
		m_cpContext->IASetPrimitiveTopology(topology);

		UINT offset = 0;
		m_cpContext->IASetVertexBuffers(0, 1, buffer->GetAddress(), &stride, &offset);
	}

	m_cpContext->Draw(vCount, 0);
}

//-----------------------------------------------------------------------------
// スワップチェイン作成
//-----------------------------------------------------------------------------
bool GraphicsDevice::CreateSwapChain()
{
	CheckMSAA();

	// スワップチェーンの設定データ
	DXGI_SWAP_CHAIN_DESC DXGISwapChainDesc;
	DXGISwapChainDesc.BufferDesc.Width						= m_createParam.Width;
	DXGISwapChainDesc.BufferDesc.Height						= m_createParam.Height;
	DXGISwapChainDesc.BufferDesc.RefreshRate.Numerator		= 0;
	DXGISwapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
	DXGISwapChainDesc.BufferDesc.Format						= m_createParam.UseHDR ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGISwapChainDesc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	DXGISwapChainDesc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;

	DXGISwapChainDesc.SampleDesc	= m_sampleDesc;

	DXGISwapChainDesc.BufferUsage	= DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	DXGISwapChainDesc.BufferCount	= m_createParam.BufferCount;
	DXGISwapChainDesc.OutputWindow	= m_hwnd;
	DXGISwapChainDesc.Windowed		= m_createParam.Windowed;
	DXGISwapChainDesc.SwapEffect	= DXGISwapChainDesc.BufferCount >= 2 ? DXGI_SWAP_EFFECT_DISCARD : DXGI_SWAP_EFFECT_SEQUENTIAL;
	DXGISwapChainDesc.Flags			= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (FAILED(m_cpFactory->CreateSwapChain(m_cpDevice.Get(), &DXGISwapChainDesc, m_cpGISwapChain.GetAddressOf())))
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// バックバッファーの作成
//-----------------------------------------------------------------------------
bool GraphicsDevice::CreateBackBuffer()
{
	//スワップチェインからバックバッファ取得
	ComPtr<ID3D11Texture2D> pBackBuffer;
	if (FAILED(m_cpGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)pBackBuffer.GetAddressOf())))
		return false;

	//バックバッファ(カラー)
	m_spBackbuffer = std::make_shared<Texture>();
	if (!m_spBackbuffer->Create(pBackBuffer.Get()))
		return false;

	//バックバッファ(Z)
	m_spDefaultZbuffer = std::make_shared<Texture>();
	if (!m_spDefaultZbuffer->CreateDepthStencil(m_createParam.Width, m_createParam.Height))
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// ビューポート変換行列の登録
//-----------------------------------------------------------------------------
bool GraphicsDevice::CreateViewport()
{
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width	= static_cast<FLOAT>(m_createParam.Width);
	m_viewport.Height	= static_cast<FLOAT>(m_createParam.Height);
	m_viewport.MinDepth = D3D11_MIN_DEPTH;
	m_viewport.MaxDepth = D3D11_MAX_DEPTH;

	m_cpContext->RSSetViewports(1, &m_viewport);
	return true;
}

//-----------------------------------------------------------------------------
// 環境に最適なアダプタを取得
//-----------------------------------------------------------------------------
void GraphicsDevice::CheckAdapter()
{
	//アダプタ列挙
	std::vector <ComPtr<IDXGIAdapter1>> adapters;
	for (int i = 0; m_cpFactory->EnumAdapters1(i, m_cpAdapter.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND; ++i)
		adapters.push_back(m_cpAdapter.Get());

	for (SIZE_T mem_size = 0; const auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc; adpt->GetDesc(&adesc);

		//ビデオメモリのサイズ比較
		if (SIZE_T mem_size_tmp = adesc.DedicatedVideoMemory; mem_size_tmp > mem_size)
		{
			m_adapterName = adesc.Description;
			mem_size = mem_size_tmp;
			adpt.CopyTo(m_cpAdapter.GetAddressOf());

			Debug::Log(m_adapterName + L": " + std::to_wstring(ByteToGB((double)mem_size)));
		}
	}
	adapters.clear();
	Debug::Log(L"Adapter decision: " + m_adapterName);
}

//-----------------------------------------------------------------------------
// 環境でMSAAを使用できるか確認しDXGI_SAMPLE_DESCを取得
//-----------------------------------------------------------------------------
void GraphicsDevice::CheckMSAA()
{
	for (int i = 1; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i <<= 1)
	{
		if (UINT Quality; SUCCEEDED(m_cpDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, i, &Quality)))
		{
			if (0 < Quality)
			{
				m_sampleDesc.Count = i;
				m_sampleDesc.Quality = Quality - 1;
			}
		}
	}

	m_sampleDesc.Count = m_createParam.UseMSAA ? m_sampleDesc.Count : 1;

	//MSAA非対応の場合
	if (m_sampleDesc.Count <= 1)
	{
		if (m_createParam.UseMSAA)
			Debug::Log("MSAA非対応 MSAAをOFFにします.");

		//MSAAをOFFに
		m_sampleDesc.Quality = 0;
		m_createParam.UseMSAA = false;
	}

	Debug::Log("m_sampleDesc.Count: " + std::to_string(m_sampleDesc.Count));
	Debug::Log("m_sampleDesc.Quality: " + std::to_string(m_sampleDesc.Quality));
}
