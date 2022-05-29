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

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
CyclopsRoot::CyclopsRoot(ApplicationBase* app)
	: m_pApplicationBase(app)
	, m_windowWidth(1600)
	, m_windowHeight(900)
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
	//m_pCameraManager->Initialize();

	m_pImGuiProfile->SetWindow(m_pWindow);
	m_pImGuiProfile->SetFpsTimer(m_pFpsTimer);
	m_pImGuiProfile->SetGraphicsDevice(m_pGraphicsDevice);
	m_pImGuiProfile->SetAudioDevice(m_pAudioDevice);
	m_pImGuiProfile->SetCameraManager(m_pCameraManager);
	//<-------------------------サブシステムを適切な順番で生成ここまで

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

	//3D
	{
		m_pApplicationBase->OnDraw3D();

		//GPUパーティクル
		m_pGraphicsDevice->m_spShaderManager->m_GPUParticleShader.Begin();
		m_pGraphicsDevice->m_spParticleSystem->Update();
		m_pGraphicsDevice->m_spParticleSystem->Draw();
		m_pGraphicsDevice->m_spShaderManager->m_GPUParticleShader.End();
	}
	//2D
	{
		m_pGraphicsDevice->m_spShaderManager->m_spriteShader.Begin();

		m_pApplicationBase->OnDraw2D();

		m_pGraphicsDevice->m_spShaderManager->m_spriteShader.End();
	}

	m_pImGuiProfile->DrawProfileMonitor();

	m_pGraphicsDevice->End();
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void CyclopsRoot::LateUpdate()
{
	m_pAudioDevice->Update(Matrix());

	m_pApplicationBase->OnLateUpdate();
}
