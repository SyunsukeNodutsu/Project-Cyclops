#include "GameDemo.h"

Window*			GameDemo::m_pWindow			= nullptr;
FpsTimer*		GameDemo::m_pFpsTimer		= nullptr;

GraphicsDevice* GameDemo::m_pGraphicsDevice = nullptr;
AudioDevice*	GameDemo::m_pAudioDevice	= nullptr;

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GameDemo::GameDemo()
	: m_windowWidth(1280)
	, m_windowHeight(720)
	, m_spTexture(nullptr)
	, m_profile()
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
	m_profile.Start("サブシステム起動");
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
	m_pAudioDevice = new AudioDevice();

	GraphicsDeviceChild::SetDevice(m_pGraphicsDevice);
	AudioDeviceChild::SetDevice(m_pAudioDevice);

	m_pGraphicsDevice->Initialize();
	m_pAudioDevice->Initialize();
	m_profile.End();
	//<-------------------------サブシステムを適切な順番で生成ここまで

	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui::StyleColorsClassic();

		ImGui_ImplWin32_Init(m_pWindow->GetHwnd());
		ImGui_ImplDX11_Init(m_pGraphicsDevice->m_cpDevice.Get(), m_pGraphicsDevice->m_cpContext.Get());

		ImFontConfig config;
		config.MergeMode = true;

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontDefault();
		io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}

	m_pAudioDevice->SetMasterVolume(1.0f);
	
	if (std::wstring path = L""; Utility::OpenFileDialog(path, m_pWindow->GetHwnd(),"再生ファイルを選択."))
	{
		m_profile.Start("Sound読み込み");
		std::shared_ptr<Sound> new_sound = std::make_shared<Sound>(path, true, true);
		if (new_sound)
		{
			new_sound->Play();
			new_sound->SetVolume(0.0f);
			new_sound->SetFade(1.0f, 0.4f);
			m_pAudioDevice->AddSound(new_sound);
		}
		m_profile.End();
	}

	m_profile.Start("テクスチャ読み込み");
	m_spTexture = std::make_shared<Texture>();
	m_spTexture->Load(L"Assets/test.jpg");
	m_profile.End();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void GameDemo::Update()
{
	m_pFpsTimer->Tick();

	const auto& mvol = m_pAudioDevice->GetMasterVolume();
	if (Input::IsKeyDown(KeyCode::UpArrow)) { m_pAudioDevice->SetMasterVolume(mvol + 0.1f); Debug::Log("音量変更: " + ToString(m_pAudioDevice->GetMasterVolume())); }
	if (Input::IsKeyDown(KeyCode::DownArrow)) { m_pAudioDevice->SetMasterVolume(mvol - 0.1f); Debug::Log("音量変更: " + ToString(m_pAudioDevice->GetMasterVolume())); }

	if (Input::IsKeyDown(KeyCode::K))
		m_pAudioDevice->SetFadeSoundList(0.0f, 1.0f);

	if (Input::IsKeyDown(KeyCode::Space))
	{
		if (std::wstring path = L""; Utility::OpenFileDialog(path, m_pWindow->GetHwnd(),"再生ファイルを選択."))
		{
			m_profile.Start("Sound読み込み");
			std::shared_ptr<Sound> new_sound = std::make_shared<Sound>(path, true, true);
			m_pAudioDevice->SetFadeSoundList(0.0f, 0.4f);//タイムスタンプの記憶の関係で 読み込みなどの時間がかかる処理だと破綻する(草)
			if (new_sound)
			{
				new_sound->Play();
				new_sound->SetVolume(0.0f);
				new_sound->SetFade(1.0f, 0.4f);
				m_pAudioDevice->AddSound(new_sound);
			}
			m_profile.End();
		}
	}
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

	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//ここでImGui描画

		if (!ImGui::Begin("Audio Monitor", nullptr, ImGuiWindowFlags_NoCollapse)) {
			ImGui::End();
			return;
		}

		ImGui::Text("XAudio2 version. (2.9)");

		ImGui::End();


		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	m_pGraphicsDevice->End();
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GameDemo::LateUpdate()
{
	m_pAudioDevice->Update(Matrix());
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void GameDemo::Finalize()
{
	{
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = NULL;

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	m_pAudioDevice->Finalize();
	m_pGraphicsDevice->Finalize();
	m_pWindow->Finalize();

	SafeDelete(m_pFpsTimer);
	SafeDelete(m_pAudioDevice);
	SafeDelete(m_pGraphicsDevice);
	SafeDelete(m_pWindow);

	Debug::Log("終了.");
}
