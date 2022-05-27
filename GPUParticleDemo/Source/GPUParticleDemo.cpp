#include "GPUParticleDemo.h"

Window*			GPUParticleDemo::m_pWindow			= nullptr;
FpsTimer*		GPUParticleDemo::m_pFpsTimer		= nullptr;

GraphicsDevice* GPUParticleDemo::m_pGraphicsDevice	= nullptr;
AudioDevice*	GPUParticleDemo::m_pAudioDevice		= nullptr;
ImGuiProfile*	GPUParticleDemo::m_pImGuiProfile	= nullptr;

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GPUParticleDemo::GPUParticleDemo()
	: m_windowWidth(1600)
	, m_windowHeight(900)
	, m_profile()
	, m_camera()
	, m_emitData()
{
}

//-----------------------------------------------------------------------------
// 実行大本
//-----------------------------------------------------------------------------
void GPUParticleDemo::Run()
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
void GPUParticleDemo::Initialize()
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
	//いずれこの一連の初期化はエンジンが提供します
	m_profile.Start("サブシステム起動");
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

	GraphicsDeviceChild::SetDevice(m_pGraphicsDevice);

	m_pGraphicsDevice->Initialize();
	m_pAudioDevice->Initialize();
	m_pImGuiProfile->Initialize();

	m_pImGuiProfile->SetWindow(m_pWindow);
	m_pImGuiProfile->SetFpsTimer(m_pFpsTimer);
	m_pImGuiProfile->SetGraphicsDevice(m_pGraphicsDevice);
	m_pImGuiProfile->SetAudioDevice(m_pAudioDevice);

	m_profile.End();
	//<-------------------------サブシステムを適切な順番で生成ここまで

	//初期カメラ
	m_camera.m_name = "Camera Main";
	m_camera.m_priority = 10.0f;
	m_camera.SetCameraMatrix(Matrix::CreateTranslation(Vector3(0, 0, -3)));

	
	m_emitData.m_minPosition = Vector3::Zero;
	m_emitData.m_maxPosition = Vector3::Zero;

	const float vel = 2.0f;
	m_emitData.m_minVelocity = Vector3(-vel, -vel, -vel);
	m_emitData.m_maxVelocity = Vector3(vel, vel, vel);

	m_emitData.m_minLifeSpan = 1.0f;
	m_emitData.m_maxLifeSpan = 3.0f;
	m_emitData.m_color = Vector4(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void GPUParticleDemo::Update()
{
	m_pFpsTimer->Tick();

	//粒子テスト
	static float time_count = 0.0f;
	time_count += (float)FpsTimer::GetDeltaTime();
	if (time_count >= 0.2f)
	{
		m_pGraphicsDevice->m_spParticleSystem->Emit(m_emitData, 10000, false);
		time_count = 0.0f;
	}

	//定数バッファ関連
	m_pGraphicsDevice->m_spRendererStatus->m_cb4Behaviour.Work().m_worldMatrix = Matrix::CreateTranslation(0, 0, 0);
	m_pGraphicsDevice->m_spRendererStatus->m_cb4Behaviour.Write();

	m_pGraphicsDevice->m_spRendererStatus->m_cb7Time.Work().m_deltaTime = (float)FpsTimer::GetDeltaTime();
	m_pGraphicsDevice->m_spRendererStatus->m_cb7Time.Work().m_totalTime = (float)FpsTimer::GetTotalTime();
	m_pGraphicsDevice->m_spRendererStatus->m_cb7Time.Write();

	//カメラ操作
	static Vector3 pos = m_camera.GetCameraMatrix().GetTranslation();
	static constexpr float move_pow = 10.0f;
	static constexpr float rot_pow = 30.0f;

	//座標
	if (Input::IsKeyPressed(KeyCode::LeftArrow))	pos.x -= move_pow * (float)FpsTimer::GetDeltaTime(true);
	if (Input::IsKeyPressed(KeyCode::RightArrow))	pos.x += move_pow * (float)FpsTimer::GetDeltaTime(true);
	if (Input::IsKeyPressed(KeyCode::UpArrow))		pos.z += move_pow * (float)FpsTimer::GetDeltaTime(true);
	if (Input::IsKeyPressed(KeyCode::DownArrow))	pos.z -= move_pow * (float)FpsTimer::GetDeltaTime(true);

	//回転
	static Vector3 vrot = Vector3::Zero;
	if (Input::IsKeyPressed(KeyCode::W)) vrot.x -= rot_pow * (float)FpsTimer::GetDeltaTime(true);
	if (Input::IsKeyPressed(KeyCode::S)) vrot.x += rot_pow * (float)FpsTimer::GetDeltaTime(true);
	if (Input::IsKeyPressed(KeyCode::A)) vrot.y += rot_pow * (float)FpsTimer::GetDeltaTime(true);
	if (Input::IsKeyPressed(KeyCode::D)) vrot.y -= rot_pow * (float)FpsTimer::GetDeltaTime(true);

	//trans x rot
	Matrix trans	= Matrix::CreateTranslation(pos);
	Matrix rotation = Matrix::CreateFromYawPitchRoll(DegToRad(vrot.y), DegToRad(vrot.x), DegToRad(vrot.z));
	m_camera.SetCameraMatrix(trans * rotation);
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void GPUParticleDemo::Draw()
{
	static float x = 0.0f, y = 0.0f;

	m_camera.SetToShader();

	m_pGraphicsDevice->Begin();

	//3D
	{
		m_pGraphicsDevice->m_spShaderManager->m_GPUParticleShader.Begin();

		m_pGraphicsDevice->m_spParticleSystem->Update();
		m_pGraphicsDevice->m_spParticleSystem->Draw();
	}

	m_pImGuiProfile->DrawProfileMonitor();

	m_pGraphicsDevice->End();
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GPUParticleDemo::LateUpdate()
{
	m_pAudioDevice->Update(Matrix());
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void GPUParticleDemo::Finalize()
{
	m_pImGuiProfile->Finalize();
	m_pAudioDevice->Finalize();
	m_pGraphicsDevice->Finalize();
	m_pWindow->Finalize();

	SafeDelete(m_pFpsTimer);
	SafeDelete(m_pImGuiProfile);
	SafeDelete(m_pGraphicsDevice);
	SafeDelete(m_pAudioDevice);
	SafeDelete(m_pWindow);

	Debug::Log("終了.");
}