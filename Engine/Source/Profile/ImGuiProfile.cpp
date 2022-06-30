#include "ImGuiProfile.h"

std::list<ImGuiProfile::LogData> ImGuiProfile::m_logDatas;
bool ImGuiProfile::m_addLog = false;

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ImGuiProfile::ImGuiProfile()
	: m_inifile(false)
	, m_showDemo(true)
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
void ImGuiProfile::AddLog(std::string_view log, const float3& color, const std::source_location& location)
{
	time_t times_day; tm localtime;

	time(&times_day);
	localtime_s(&localtime, &times_day);

	const std::string& hour = (localtime.tm_hour < 10) ? "0" + ToString(localtime.tm_hour) : ToString(localtime.tm_hour);
	const std::string& min  = (localtime.tm_min  < 10) ? "0" + ToString(localtime.tm_min)  : ToString(localtime.tm_min);
	const std::string& sec  = (localtime.tm_sec  < 10) ? "0" + ToString(localtime.tm_sec)  : ToString(localtime.tm_sec);

	LogData data =
	{
		.log = "[" + hour + ":" + min + ":" + sec + "]: " + log.data(),
		.color = color,
	};

	m_logDatas.push_back(data);
	m_addLog = true;
}

//-----------------------------------------------------------------------------
// ゲームシーン
//-----------------------------------------------------------------------------
void ImGuiProfile::SceneMonitor(ImGuiWindowFlags wflags)
{
	if (!ImGui::Begin("Scene Monitor", nullptr, wflags)) { ImGui::End(); return; }

	ImGui::Text(std::string("Fps: " + std::to_string(m_pFpsTimer->GetFPS())).c_str());
	ImGui::Text(std::string("DeltaTime: " + std::to_string(m_pFpsTimer->GetDeltaTime<float>())).c_str());
	ImGui::Text(std::string("TotalTime: " + std::to_string(m_pFpsTimer->GetTotalTime<float>())).c_str());
	ImGui::Text(std::string("TimeScale: " + std::to_string(m_pFpsTimer->GetTimeScale<float>())).c_str());
	ImGui::Text(std::string("\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e").c_str());

	ImGui::Text(reinterpret_cast<const char*>(u8"これで日本語いける(;o;)"));

	static float timeScale = m_pFpsTimer->GetTimeScale<float>();
	if (ImGui::SliderFloat("TimeScale", &timeScale, 0, 5, "%.2f"))
		m_pFpsTimer->SetTimeScale(timeScale);

	const auto& cbCamera = m_pGraphicsDevice->m_spRendererStatus->m_cb5Camera.Get();
	ImGui::Text(std::string("CameraMatrix Trans: " + ToStringV(cbCamera.m_cameraMatrix.Translation())).c_str());
	ImGui::Text(std::string("ViewMatrix Trans  : " + ToStringV(cbCamera.m_viewMatrix.Translation())).c_str());

	ImGui::Text(std::string("NumParticles: " + ToString(ParticleSystem::m_numParticle)).c_str());
	
	ImGui::Text(std::string("MousePos: " + ToString(Input::GetMousePos().x) + ", " + ToString(Input::GetMousePos().y)).c_str());

	static bool show_headers = false;
	ImGui::Checkbox("show_headers", &show_headers);
	if (ImGui::BeginTable("table_padding", 3, 0))
	{
		if (show_headers)
		{
			ImGui::TableSetupColumn("One");
			ImGui::TableSetupColumn("Two");
			ImGui::TableSetupColumn("Three");
			ImGui::TableHeadersRow();
		}

		for (int row = 0; row < 5; row++)
		{
			ImGui::TableNextRow();
			for (int column = 0; column < 3; column++)
			{
				ImGui::TableSetColumnIndex(column);
				if (row == 0)
				{
					ImGui::Text("Avail %.2f", ImGui::GetContentRegionAvail().x);
				}
				else
				{
					char buf[32];
					sprintf_s(buf, "Hello %d,%d", column, row);
					ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
				}
				if (ImGui::TableGetColumnFlags() & ImGuiTableColumnFlags_IsHovered)
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(0, 100, 0, 255));
			}
		}
		ImGui::EndTable();
	}

	ImGui::End();
}

//-----------------------------------------------------------------------------
// カメラ
//-----------------------------------------------------------------------------
void ImGuiProfile::CameraMonitor(ImGuiWindowFlags wflags)
{
	if (!ImGui::Begin("Camera Monitor", nullptr, wflags)) { ImGui::End(); return; }

	//カメラの情報
	const auto& cam_name = m_pCameraManager->IsNowDolly() ? "DollyCamera" : m_pCameraManager->GetUseCamera()->m_name;
	ImGui::Text(std::string("Use Camera: " + cam_name).c_str());
	static std::weak_ptr<Camera> wpCamera;
	for (const auto& camera : m_pCameraManager->GetCameraList())
	{
		ImGui::PushID(camera.get());

		bool selected = (camera == wpCamera.lock());
		if (ImGui::Selectable(camera->m_name.c_str(), selected))
			wpCamera = camera;

		ImGui::PopID();
	}

	if (wpCamera.lock())
	{
		ImGui::Text(std::string("Select: " + wpCamera.lock()->m_name).c_str());
	}

	ImGui::Separator();
	ImGui::Text("ChangeMode Select.");

	static float time = 1.0f;
	if (ImGui::SliderFloat("ChangeTime", &time, 0, 5, "%.2f"))
		m_pCameraManager->SetChangeTime(time);

	const char* items[] = { "Immediate", "Liner", "QuadIn", "QuadOut", "QuadInOut", "CubicIn", "CubicOut", "CubicInOut" };
	static int item = 0;
	if (ImGui::Combo("Change Mode", &item, items, IM_ARRAYSIZE(items)))
	{
		switch (item)
		{
			using enum ChangeMode;
		case static_cast<int>(Immediate):	m_pCameraManager->SetChangeMode(Immediate);		break;
		case static_cast<int>(Liner):		m_pCameraManager->SetChangeMode(Liner);			break;
		case static_cast<int>(QuadIn):		m_pCameraManager->SetChangeMode(QuadIn);		break;
		case static_cast<int>(QuadOut):		m_pCameraManager->SetChangeMode(QuadOut);		break;
		case static_cast<int>(QuadInOut):	m_pCameraManager->SetChangeMode(QuadInOut);		break;
		case static_cast<int>(CubicIn):		m_pCameraManager->SetChangeMode(CubicIn);		break;
		case static_cast<int>(CubicOut):	m_pCameraManager->SetChangeMode(CubicOut);		break;
		case static_cast<int>(CubicInOut):	m_pCameraManager->SetChangeMode(CubicInOut);	break;
		}
		ImGui::SameLine();
	}

	ImGui::End();
}

//-----------------------------------------------------------------------------
// ゲームオーディオ
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
		UINT lenM = wpSound.lock()->GetSeconds() / 60;
		UINT lenS = wpSound.lock()->GetSeconds() % 60;
		UINT lenN_M = wpSound.lock()->GetElapsedSeconds() / 60;
		UINT lenN_S = wpSound.lock()->GetElapsedSeconds() % 60;

		ImGui::Text(std::string("Name : " + wpSound.lock()->GetName()).c_str());
		ImGui::Text(std::string("Time : " + ToString(lenN_M) + ":" + ToString(lenN_S) + " / " + ToString(lenM) + ":" + ToString(lenS)).c_str());

		auto val = wpSound.lock()->GetVolume();
		if (ImGui::SliderFloat("Volume", &val, 0.0f, 1.0f, "%.2f")) wpSound.lock()->SetVolume(val);

		if (ImGui::Button("Start")) wpSound.lock()->Play();
		if (ImGui::Button("Stop")) wpSound.lock()->Stop();//ウィンドウに応じて ボタンサイズ変更(文字列を短くしたり？)

		if (ImGui::TreeNodeEx("Equalizer", ImGuiTreeNodeFlags_None))
		{
			//ImGui::Checkbox("NoDamage", &m_isNoDamage);

			bool change = false;

			static float bandW0 = FXEQ_DEFAULT_BANDWIDTH, bandW1 = FXEQ_DEFAULT_BANDWIDTH, bandW2 = FXEQ_DEFAULT_BANDWIDTH, bandW3 = FXEQ_DEFAULT_BANDWIDTH;
			if (ImGui::SliderFloat("BandWidth 0", &bandW0, FXEQ_MIN_BANDWIDTH, FXEQ_MAX_BANDWIDTH, "%.2f")) { change = true; }
			if (ImGui::SliderFloat("BandWidth 1", &bandW1, FXEQ_MIN_BANDWIDTH, FXEQ_MAX_BANDWIDTH, "%.2f")) { change = true; }
			if (ImGui::SliderFloat("BandWidth 2", &bandW2, FXEQ_MIN_BANDWIDTH, FXEQ_MAX_BANDWIDTH, "%.2f")) { change = true; }
			if (ImGui::SliderFloat("BandWidth 3", &bandW3, FXEQ_MIN_BANDWIDTH, FXEQ_MAX_BANDWIDTH, "%.2f")) { change = true; }

			static float gain0 = FXEQ_DEFAULT_GAIN, gain1 = FXEQ_DEFAULT_GAIN, gain2 = FXEQ_DEFAULT_GAIN, gain3 = FXEQ_DEFAULT_GAIN;
			if (ImGui::SliderFloat("Gain 0: ", &gain0, FXEQ_MIN_GAIN, FXEQ_MAX_GAIN, "%.2f")) { change = true; }
			if (ImGui::SliderFloat("Gain 1: ", &gain1, FXEQ_MIN_GAIN, FXEQ_MAX_GAIN, "%.2f")) { change = true; }
			if (ImGui::SliderFloat("Gain 2: ", &gain2, FXEQ_MIN_GAIN, FXEQ_MAX_GAIN, "%.2f")) { change = true; }
			if (ImGui::SliderFloat("Gain 3: ", &gain3, FXEQ_MIN_GAIN, FXEQ_MAX_GAIN, "%.2f")) { change = true; }

			FXEQ_PARAMETERS param =
			{
				.FrequencyCenter0	= FXEQ_DEFAULT_FREQUENCY_CENTER_0,
				.Gain0				= gain0,
				.Bandwidth0			= bandW0,

				.FrequencyCenter1	= FXEQ_DEFAULT_FREQUENCY_CENTER_1,
				.Gain1				= gain1,
				.Bandwidth1			= bandW1,

				.FrequencyCenter2	= FXEQ_DEFAULT_FREQUENCY_CENTER_2,
				.Gain2				= gain2,
				.Bandwidth2			= bandW2,

				.FrequencyCenter3	= FXEQ_DEFAULT_FREQUENCY_CENTER_3,
				.Gain3				= gain3,
				.Bandwidth3			= bandW3,
			};

			//if (ImGui::Button("SetEqualizer")) wpSound.lock()->SetEqualizer(param);
			if (change) wpSound.lock()->SetEqualizer(param);

			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Filter and Pan", ImGuiTreeNodeFlags_None))
		{
			static float frequencym = 0.0f;//フィルタ/パンの値もSoundクラスに保存してもよさそう
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

			ImGui::TreePop();
		}

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
	ImGui::PlotLines("PeakLevels L", valuesC, IM_ARRAYSIZE(valuesC), values_offset, overlay, down, up + 1.0f, ImVec2(200, 60.0f));

	sprintf_s(overlay, "avg %f", m_pAudioDevice->m_RMSLevels[1]);
	ImGui::PlotLines("PeakLevels R", valuesD, IM_ARRAYSIZE(valuesD), values_offset, overlay, down, up + 1.0f, ImVec2(200, 60.0f));
}
