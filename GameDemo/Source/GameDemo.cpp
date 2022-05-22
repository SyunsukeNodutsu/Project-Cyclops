#include "GameDemo.h"

Window*			GameDemo::m_pWindow			= nullptr;
FpsTimer*		GameDemo::m_pFpsTimer		= nullptr;

GraphicsDevice* GameDemo::m_pGraphicsDevice = nullptr;
AudioDevice*	GameDemo::m_pAudioDevice	= nullptr;
VideoDevice*	GameDemo::m_pVideoDevice	= nullptr;

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GameDemo::GameDemo()
	: m_windowWidth(1280)
	, m_windowHeight(720)
	, m_spSound(nullptr)
	, m_soundPath(L"Assets/Sunshine.mp3")
	, m_spTexture(nullptr)
{
}

//-----------------------------------------------------------------------------
// 実行大本
//-----------------------------------------------------------------------------
void GameDemo::Run()
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
void GameDemo::Initialize()
{
	//ウィンドウ作成
	WINDOE_CREATE_PARAM window_param = {
		.TitleName		= L"Project Cyclops",
		.ClassName		= L"ClassName",
		.ClientWidth	= m_windowWidth,
		.ClientHeight	= m_windowHeight,
	};
	m_pWindow = new Window(window_param);
	m_pWindow->Initialize();
	//サブシステムインターフェースに単一のHWNDを設定
	ISubsystem::SetHWND(m_pWindow->GetHwnd());

	m_pFpsTimer = new FpsTimer();
	
	//サブシステムを適切な順番で生成 -------------------------------->
	//いずれこの一連の初期化はエンジンが提供します
	Profile profile;
	profile.Start("サブシステム起動");
	GRAPHICS_DEVICE_CREATE_PARAM device_param = {
		.BufferCount	= 2,
		.Width			= m_windowWidth,
		.Height			= m_windowHeight,
		.RefreshRate	= 0,
		.Windowed		= true,
		.UseHDR			= false,
		.UseMSAA		= false,
		.DebugMode		= true,
	};
	m_pGraphicsDevice = new GraphicsDevice(device_param);

	m_pVideoDevice = new VideoDevice();
	m_pAudioDevice = new AudioDevice();

	GraphicsDeviceChild::SetDevice(m_pGraphicsDevice);
	AudioDeviceChild::SetDevice(m_pAudioDevice);

	m_pGraphicsDevice->Initialize();
	m_pVideoDevice->Initialize();
	m_pAudioDevice->Initialize();
	profile.End();
	//<-------------------------サブシステムを適切な順番で生成ここまで

	//m_pVideoDevice->Play();
	m_pAudioDevice->SetMasterVolume(0.2f);
	
	std::thread([&] {
		m_spSound = std::make_shared<Sound>(m_soundPath, true, true);
		if (m_spSound)
		{
			m_spSound->Play();
			m_pAudioDevice->AddSound(m_spSound);
		}}
	).detach();

	profile.Start("テクスチャ読み込み");
	m_spTexture = std::make_shared<Texture>();
	m_spTexture->Load(L"Assets/test.jpg");
	profile.End();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void GameDemo::Update()
{
	GamePad::Update();
	m_pFpsTimer->Tick();
	m_pAudioDevice->Update(Matrix());
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void GameDemo::Draw()
{
	static float x = 0.0f, y = 0.0f;
	float total_time = static_cast<float>(m_pFpsTimer->GetTotalTime());

	m_pGraphicsDevice->Begin();

	m_pGraphicsDevice->m_spShaderManager->m_spriteShader.Begin();

	x = sinf(total_time) * 200.0f;
	y = tanf(total_time) * 100.0f;
	m_pGraphicsDevice->m_spShaderManager->m_spriteShader.DrawTexture(m_spTexture.get(), Vector2(x, y));

	m_pGraphicsDevice->m_spShaderManager->m_spriteShader.End();

	m_pGraphicsDevice->End();
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GameDemo::LateUpdate()
{

}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void GameDemo::Finalize()
{
	m_pAudioDevice->Finalize();
	m_pVideoDevice->Finalize();
	m_pGraphicsDevice->Finalize();
	m_pWindow->Finalize();

	delete m_pFpsTimer;
	delete m_pAudioDevice;
	delete m_pVideoDevice;
	delete m_pGraphicsDevice;
	delete m_pWindow;

	Debug::Log("終了.");
}
