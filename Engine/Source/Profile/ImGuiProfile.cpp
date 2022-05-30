#include "ImGuiProfile.h"

std::list<ImGuiProfile::LogData> ImGuiProfile::m_logDatas;
bool ImGuiProfile::m_addLog = false;

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ImGuiProfile::ImGuiProfile()
	: m_inifile(false)
	, m_showDemo(false)
	, m_pWindow(nullptr)
	, m_pFpsTimer(nullptr)
	, m_pGraphicsDevice(nullptr)
	, m_pAudioDevice(nullptr)
	, m_pCameraManager(nullptr)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void ImGuiProfile::Initialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsClassic();

	ImGui_ImplWin32_Init(m_hwnd);
	ImGui_ImplDX11_Init(GraphicsDeviceChild::GetDevice()->m_cpDevice.Get(), GraphicsDeviceChild::GetDevice()->m_cpContext.Get());

	ImFontConfig config;
	config.MergeMode = true;

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void ImGuiProfile::Finalize()
{
	if (m_inifile)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

//-----------------------------------------------------------------------------
// モニター描画
//-----------------------------------------------------------------------------
void ImGuiProfile::DrawProfileMonitor()
{
	//開始
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags wflags;
	if (true) wflags = ImGuiWindowFlags_NoCollapse;
	else wflags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

	if (m_showDemo)
		ImGui::ShowDemoWindow();

	AudioMonitor(wflags);
	CameraMonitor(wflags);
	SceneMonitor(wflags);
	LogMonitor(wflags);

	//終了
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//-----------------------------------------------------------------------------
// ログを追加
//-----------------------------------------------------------------------------
void ImGuiProfile::AddLog(std::string_view log, const Vector3& color, const std::source_location& location)
{
	time_t times_day; tm localtime;

	time(&times_day);
	localtime_s(&localtime, &times_day);

	std::string hour = (localtime.tm_hour < 10) ? "0" + ToString(localtime.tm_hour) : ToString(localtime.tm_hour);
	std::string min = (localtime.tm_min < 10) ? "0" + ToString(localtime.tm_min) : ToString(localtime.tm_min);
	std::string sec = (localtime.tm_sec < 10) ? "0" + ToString(localtime.tm_sec) : ToString(localtime.tm_sec);

	LogData data{};
	data.log += "[" + hour + ":" + min + ":" + sec + "]: ";
	data.log += log.substr();

	data.color = color;

	m_logDatas.push_back(data);

	m_addLog = true;
}

//-----------------------------------------------------------------------------
// ゲームシーン監視
//-----------------------------------------------------------------------------
void ImGuiProfile::SceneMonitor(ImGuiWindowFlags wflags)
{
	if (!ImGui::Begin("Scene Monitor", nullptr, wflags)) { ImGui::End(); return; }

	ImGui::Text(std::string("Fps: " + std::to_string(m_pFpsTimer->GetFPS())).c_str());
	ImGui::Text(std::string("DeltaTime: " + std::to_string(m_pFpsTimer->GetDeltaTime<float>())).c_str());
	ImGui::Text(std::string("TotalTime: " + std::to_string(m_pFpsTimer->GetTotalTime<float>())).c_str());
	ImGui::Text(std::string("TimeScale: " + std::to_string(m_pFpsTimer->GetTimeScale<float>())).c_str());
	ImGui::Text(std::string("\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e").c_str());

	static float timeScale = m_pFpsTimer->GetTimeScale<float>();
	if (ImGui::SliderFloat("TimeScale", &timeScale, 0, 5, "%.2f"))
		m_pFpsTimer->SetTimeScale(timeScale);

	const auto& cbCamera = m_pGraphicsDevice->m_spRendererStatus->m_cb5Camera.Get();
	ImGui::Text(std::string("CameraMatrix Trans: " + ToStringV(cbCamera.m_cameraMatrix.GetTranslation())).c_str());
	ImGui::Text(std::string("ViewMatrix Trans  : " + ToStringV(cbCamera.m_viewMatrix.GetTranslation())).c_str());

	ImGui::Text(std::string("NumParticles: " + ToString(ParticleSystem::m_numParticle)).c_str());
	
	ImGui::Text(std::string("MousePos: " + ToString(Input::GetMousePos().x) + ", " + ToString(Input::GetMousePos().y)).c_str());

	ImGui::End();
}

//-----------------------------------------------------------------------------
// カメラ
//-----------------------------------------------------------------------------
void ImGuiProfile::CameraMonitor(ImGuiWindowFlags wflags)
{
	if (!ImGui::Begin("Camera Monitor", nullptr, wflags)) { ImGui::End(); return; }

	//カメラの情報
	ImGui::Text(std::string("Use Camera: " + m_pCameraManager->GetUseCamera()->m_name).c_str());
	static std::weak_ptr<Camera> wpCamera;
	for (const auto& camera : m_pCameraManager->GetCameraList())
	{
		ImGui::PushID(camera.get());

		bool selected = (camera == wpCamera.lock());
		if (ImGui::Selectable(camera->m_name.c_str(), selected))
			wpCamera = camera;

		ImGui::PopID();
	}

	ImGui::Separator();
	ImGui::Text("ChangeMode Select.");

	if (ImGui::Button("Immediate ")) m_pCameraManager->SetChangeMode(ChangeMode::Immediate);
	if (ImGui::Button("Liner     ")) m_pCameraManager->SetChangeMode(ChangeMode::Liner);
	if (ImGui::Button("QuadIn    ")) m_pCameraManager->SetChangeMode(ChangeMode::QuadIn);
	if (ImGui::Button("QuadOut   ")) m_pCameraManager->SetChangeMode(ChangeMode::QuadOut);
	if (ImGui::Button("QuadInOut ")) m_pCameraManager->SetChangeMode(ChangeMode::QuadInOut);
	if (ImGui::Button("CubicIn   ")) m_pCameraManager->SetChangeMode(ChangeMode::CubicIn);
	if (ImGui::Button("CubicOut  ")) m_pCameraManager->SetChangeMode(ChangeMode::CubicOut);
	if (ImGui::Button("CubicInOut")) m_pCameraManager->SetChangeMode(ChangeMode::CubicInOut);

	ImGui::End();
}

//-----------------------------------------------------------------------------
// ゲームオーディオ監視
//-----------------------------------------------------------------------------
void ImGuiProfile::AudioMonitor(ImGuiWindowFlags wflags)
{
	if (!ImGui::Begin("Audio Monitor", nullptr, wflags)) { ImGui::End(); return; }

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::Text("XAudio2 version. (2.9)");
	ImGui::PopStyleColor();

	if (ImGui::Button("OpenSoundFileDialog"))
	{
		if (std::string path = ""; Utility::OpenFileDialog(path, m_pWindow->GetHwnd(), "再生ファイルを選択."))
		{
			std::shared_ptr<Sound> new_sound = std::make_shared<Sound>(path, true, true);
			m_pAudioDevice->SetFadeSoundList(0.0f, 0.4f);//タイムスタンプの記憶の関係で 読み込みなどの時間がかかる処理だと破綻する(草)
			if (new_sound) { new_sound->Play(); m_pAudioDevice->AddSound(new_sound); }
		}
	}

	ImGui::Separator();

	static float volume = m_pAudioDevice->GetMasterVolume();
	if (ImGui::SliderFloat("Main", &volume, 0.0f, 1.0f, "%.2f"))
		m_pAudioDevice->SetMasterVolume(volume);

	//サウンドリスト列挙
	static std::weak_ptr<Sound> wpSound;
	for (auto& sound : m_pAudioDevice->GetSoundList())
	{
		ImGui::PushID(sound.get());

		bool selected = (sound == wpSound.lock());
		if (ImGui::Selectable(sound->GetName().c_str(), selected))
			wpSound = sound;

		ImGui::PopID();
	}

	ImGui::Separator();

	//サウンドリストで選んだサウンドを編集
	if (wpSound.lock())
	{
		ImGui::Text(std::string("Select: " + wpSound.lock()->GetName()).c_str());

		auto val = wpSound.lock()->GetVolume();
		if (ImGui::SliderFloat("Volume", &val, 0.0f, 2.0f, "%.2f"))
			wpSound.lock()->SetVolume(val);

		static float frequencym = 0.0f;
		if (ImGui::SliderFloat("Frequencym", &frequencym, 0.0f, XAUDIO2_MAX_FILTER_FREQUENCY, "%.4f")) {}
		static float oneOverQ = 1.4142f;
		if (ImGui::SliderFloat("OneOverQ", &oneOverQ, 0.1f, XAUDIO2_MAX_FILTER_ONEOVERQ, "%.4f")) {}
		static float pan = 0.0f;
		if (ImGui::SliderFloat("Pan", &pan, -1.0f, 1.0f, "%.2f")) {}

		if (ImGui::Button("LowPassFilter         ")) wpSound.lock()->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, frequencym, oneOverQ);
		if (ImGui::Button("BandPassFilter        ")) wpSound.lock()->SetFilter(XAUDIO2_FILTER_TYPE::BandPassFilter, frequencym, oneOverQ);
		if (ImGui::Button("HighPassFilter        ")) wpSound.lock()->SetFilter(XAUDIO2_FILTER_TYPE::HighPassFilter, frequencym, oneOverQ);
		if (ImGui::Button("NotchFilter           ")) wpSound.lock()->SetFilter(XAUDIO2_FILTER_TYPE::NotchFilter, frequencym, oneOverQ);
		if (ImGui::Button("LowPassOnePoleFilter  ")) wpSound.lock()->SetFilter(XAUDIO2_FILTER_TYPE::LowPassOnePoleFilter, frequencym, oneOverQ);
		if (ImGui::Button("HighPassOnePoleFilter ")) wpSound.lock()->SetFilter(XAUDIO2_FILTER_TYPE::HighPassOnePoleFilter, frequencym, oneOverQ);
		if (ImGui::Button("Filter OFF            ")) wpSound.lock()->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 1, 1);
		if (ImGui::Button("Pan                   ")) wpSound.lock()->SetPan(pan);

	}
	else ImGui::Text("Select: none");

	ImGui::Separator();

	PlotVolumeMeter(30.0f);

	ImGui::End();
}

//-----------------------------------------------------------------------------
// ログモニター
//-----------------------------------------------------------------------------
void ImGuiProfile::LogMonitor(ImGuiWindowFlags wflags)
{
	wflags |= ImGuiWindowFlags_MenuBar;
	if (!ImGui::Begin("Log Monitor", nullptr, wflags)) { ImGui::End(); return; }

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			ImGui::Text("todo: fix");
			ImGui::EndMenu();
		}
		if (ImGui::Button("Clear"))
		{
			ClearLog();
		}
		if (ImGui::Button("Copy"))
		{
			ImGui::Text("todo: fix");
			Debug::Log("Log copied.");
		}
		ImGui::EndMenuBar();
	}

	//ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255));
	//ImGui::TextUnformatted(m_logBuffer.begin());
	//ImGui::PopStyleColor();

	for (const auto& log : m_logDatas)
	{
		ImGui::TextColored(ImVec4(log.color.x, log.color.y, log.color.z, 1.0f), log.log.c_str());
	}

	if (m_addLog)
	{
		ImGui::SetScrollHereY(1.0f);
		m_addLog = false;
	}

	ImGui::End();
}

//-----------------------------------------------------------------------------
// VolumeMeterのプロット表示
//-----------------------------------------------------------------------------
void ImGuiProfile::PlotVolumeMeter(const float refreshRate)
{
	static float valuesA[90]{};
	static float valuesB[90]{};
	static float valuesC[90]{};
	static float valuesD[90]{};

	static int values_offset = 0;
	static double refresh_time = ImGui::GetTime();

	//指定レートでデモ用データを作成
	while (refresh_time < ImGui::GetTime())
	{
		valuesA[values_offset] = m_pAudioDevice->m_RMSLevels[0];
		valuesB[values_offset] = m_pAudioDevice->m_RMSLevels[1];
		valuesC[values_offset] = m_pAudioDevice->m_peakLevels[0];
		valuesD[values_offset] = m_pAudioDevice->m_peakLevels[1];

		values_offset = (values_offset + 1) % IM_ARRAYSIZE(valuesA);
		refresh_time += 1.0f / refreshRate;
	}

	static constexpr float down = 0.0f, up = 1.0f;

	char overlay[32];
	sprintf_s(overlay, "avg %f", m_pAudioDevice->m_peakLevels[0]);
	ImGui::PlotLines("RMSLevels L", valuesA, IM_ARRAYSIZE(valuesA), values_offset, overlay, down, up, ImVec2(200, 60.0f));

	sprintf_s(overlay, "avg %f", m_pAudioDevice->m_peakLevels[1]);
	ImGui::PlotLines("RMSLevels R", valuesB, IM_ARRAYSIZE(valuesB), values_offset, overlay, down, up, ImVec2(200, 60.0f));

	sprintf_s(overlay, "avg %f", m_pAudioDevice->m_RMSLevels[0]);
	ImGui::PlotLines("PeakLevels L", valuesC, IM_ARRAYSIZE(valuesC), values_offset, overlay, down, up, ImVec2(200, 60.0f));

	sprintf_s(overlay, "avg %f", m_pAudioDevice->m_RMSLevels[1]);
	ImGui::PlotLines("PeakLevels R", valuesD, IM_ARRAYSIZE(valuesD), values_offset, overlay, down, up, ImVec2(200, 60.0f));
}
