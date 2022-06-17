#include "CyclopsRoot.h"

GraphicsDevice*		ApplicationBase::m_pGraphicsDevice	= nullptr;
AudioDevice*		ApplicationBase::m_pAudioDevice		= nullptr;
CameraManager*		ApplicationBase::m_pCameraManager	= nullptr;

Window*				CyclopsRoot::m_pWindow			= nullptr;
FpsTimer*			CyclopsRoot::m_pFpsTimer		= nullptr;
GraphicsDevice*		CyclopsRoot::m_pGraphicsDevice	= nullptr;
AudioDevice*		CyclopsRoot::m_pAudioDevice		= nullptr;
ImGuiProfile*		CyclopsRoot::m_pImGuiProfile	= nullptr;
CameraManager*		CyclopsRoot::m_pCameraManager	= nullptr;

std::vector<UnlitShader::Vertex> CyclopsRoot::m_debugLines;
float2 ApplicationBase::m_clientSize = float2::Zero;

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
CyclopsRoot::CyclopsRoot(ApplicationBase* app)
	: m_pApplicationBase(app)
	, m_windowWidth(1600)
	, m_windowHeight(900)
	, m_spScreenRT(nullptr)
	, m_spScreenZ(nullptr)
	, m_spHeightBrightTex(nullptr)
	, m_blurTex()
{
}

//-----------------------------------------------------------------------------
// エンジン開始
//-----------------------------------------------------------------------------
void CyclopsRoot::Run()
{
	Initialize();

	while (true)
	{
		if (!m_pWindow->ProcessMessage()) break;

		Update();
		Draw();
		LateUpdate();
	}

	Finalize();
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void CyclopsRoot::Initialize()
{
	//ウィンドウ作成
	WINDOE_CREATE_PARAM window_param = {
		.TitleName = L"Project Cyclops",
		.ClassName = L"ClassName",
		.ClientWidth = m_windowWidth,
		.ClientHeight = m_windowHeight,
	};
	m_pWindow = new Window(window_param);
	m_pWindow->Initialize();
	//サブシステムインターフェースに単一のHWNDを設定
	ISubsystem::SetHWND(m_pWindow->GetHwnd());

	m_pFpsTimer = new FpsTimer();

	//サブシステムを適切な順番で生成 -------------------------------->
	GRAPHICS_DEVICE_CREATE_PARAM device_param = {
		.BufferCount = 2,
		.Width = m_windowWidth,
		.Height = m_windowHeight,
		.RefreshRate = 0,
		.Windowed = true,
		.UseHDR = false,
		.UseMSAA = false,
		.DebugMode = true,
	};
	m_pGraphicsDevice = new GraphicsDevice(device_param);
	m_pAudioDevice = new AudioDevice();
	m_pImGuiProfile = new ImGuiProfile();
	m_pCameraManager = new CameraManager();

	GraphicsDeviceChild::SetDevice(m_pGraphicsDevice);
	AudioDeviceChild::SetDevice(m_pAudioDevice);

	m_pGraphicsDevice->Initialize();
	m_pAudioDevice->Initialize();
	m_pImGuiProfile->Initialize();
	m_pCameraManager->Initialize();

	m_pImGuiProfile->SetWindow(m_pWindow);
	m_pImGuiProfile->SetFpsTimer(m_pFpsTimer);
	m_pImGuiProfile->SetGraphicsDevice(m_pGraphicsDevice);
	m_pImGuiProfile->SetAudioDevice(m_pAudioDevice);
	m_pImGuiProfile->SetCameraManager(m_pCameraManager);
	//<-------------------------サブシステムを適切な順番で生成ここまで

	//ポストプロセス用にテクスチャ作成
	{
		m_spScreenRT = std::make_shared<Texture>();
		m_spScreenRT->CreateRenderTarget(m_windowWidth, m_windowHeight, DXGI_FORMAT_R16G16B16A16_FLOAT);

		m_spScreenZ = std::make_shared<Texture>();
		m_spScreenZ->CreateDepthStencil(m_windowWidth, m_windowHeight);

		m_spHeightBrightTex = std::make_shared<Texture>();
		m_spHeightBrightTex->CreateRenderTarget(m_windowWidth, m_windowHeight, DXGI_FORMAT_R16G16B16A16_FLOAT);

		m_blurTex.Create(m_windowWidth, m_windowHeight, false);
	}

	//アプリケーション開始
	ApplicationBase::SetGraphicsDevice(m_pGraphicsDevice);
	ApplicationBase::SetAudioDevice(m_pAudioDevice);
	ApplicationBase::SetCameraManager(m_pCameraManager);
	m_pApplicationBase->OnStart();
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void CyclopsRoot::Finalize()
{
	m_pApplicationBase->OnEnd();

	//サブシステムの終了処理
	m_pGraphicsDevice->m_spParticleSystem->AllEnd();

	m_pImGuiProfile->Finalize();
	m_pAudioDevice->Finalize();
	m_pGraphicsDevice->Finalize();
	m_pWindow->Finalize();

	ShellSystem::DeleteBatFileAll();

	SafeDelete(m_pCameraManager);
	SafeDelete(m_pImGuiProfile);
	SafeDelete(m_pFpsTimer);
	SafeDelete(m_pAudioDevice);
	SafeDelete(m_pGraphicsDevice);
	SafeDelete(m_pWindow);
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void CyclopsRoot::Update()
{
	m_pFpsTimer->Tick();

	m_pGraphicsDevice->m_spRendererStatus->m_cb7Time.Work().m_deltaTime = FpsTimer::GetDeltaTime<float>();
	m_pGraphicsDevice->m_spRendererStatus->m_cb7Time.Work().m_totalTime = FpsTimer::GetTotalTime<float>();
	m_pGraphicsDevice->m_spRendererStatus->m_cb7Time.Work().m_timeScale = FpsTimer::GetTimeScale<float>();
	m_pGraphicsDevice->m_spRendererStatus->m_cb7Time.Write();

	m_pCameraManager->Update();

	m_pApplicationBase->OnUpdate();
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void CyclopsRoot::Draw()
{
	m_pCameraManager->SetToShader();

	m_pGraphicsDevice->Begin();

	{
		float clear_color[] = { 0, 0, 0, 1 };

		RestoreRenderTarget rrt = {};
		m_pGraphicsDevice->m_cpContext->OMSetRenderTargets(1, m_spScreenRT->RTVAddress(), m_spScreenZ->DSV());
		m_pGraphicsDevice->m_cpContext->ClearRenderTargetView(m_spScreenRT->RTV(), clear_color);
		m_pGraphicsDevice->m_cpContext->ClearDepthStencilView(m_spScreenZ->DSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

		//3D
		m_pApplicationBase->OnDraw3D();

		//GPUパーティクル
		m_pGraphicsDevice->m_spShaderManager->m_GPUParticleShader.Begin();
		m_pGraphicsDevice->m_spParticleSystem->Update();
		m_pGraphicsDevice->m_spParticleSystem->Draw(true);
		m_pGraphicsDevice->m_spShaderManager->m_GPUParticleShader.End();
	}

	//ブラー描画
	m_pGraphicsDevice->m_spShaderManager->m_postProcessShader.BlurDraw(m_spScreenRT.get(), float2(0, 0));

	//ブルーム
	{
		//しきい値以上のピクセルを抽出
		m_pGraphicsDevice->m_spShaderManager->m_postProcessShader.BrightFiltering(m_spHeightBrightTex.get(), m_spScreenRT.get());

		m_pGraphicsDevice->m_spShaderManager->m_postProcessShader.GenerateBlur(m_blurTex, m_spHeightBrightTex.get());

		m_pGraphicsDevice->m_spRendererStatus->SetBlend(BlendMode::Add);

		//各サイズの画像を加算合成
		for (int bCnt = 0; bCnt < 5; bCnt++)
			m_pGraphicsDevice->m_spShaderManager->m_postProcessShader.DrawColor(m_blurTex.m_rt[bCnt][0].get());

		m_pGraphicsDevice->m_spRendererStatus->SetBlend(BlendMode::Alpha);
	}

	//デバッグライン
	{
		if (m_debugLines.size() >= 1)
		{
			m_pGraphicsDevice->m_spShaderManager->m_unlitShader.Begin();

			m_pGraphicsDevice->m_spShaderManager->m_unlitShader.DrawVertices(m_debugLines, D3D_PRIMITIVE_TOPOLOGY_LINELIST);
			m_debugLines.clear();
		}
	}

	//2D
	{
		m_pGraphicsDevice->m_spShaderManager->m_spriteShader.Begin();

		m_pApplicationBase->OnDraw2D(m_pGraphicsDevice->m_spShaderManager->m_spriteShader);

		m_pGraphicsDevice->m_spShaderManager->m_spriteShader.End();
	}
	
	//ImGui
	m_pImGuiProfile->DrawProfileMonitor();

	m_pGraphicsDevice->End();
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void CyclopsRoot::LateUpdate()
{
	m_pAudioDevice->Update(matrix4x4::Identity);

	m_pApplicationBase->OnLateUpdate();
}
