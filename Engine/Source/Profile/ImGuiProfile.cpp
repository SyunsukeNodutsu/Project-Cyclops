#include "Pch.h"
#include "ImGuiProfile.h"

ImGuiTextBuffer ImGuiProfile::m_logBuffer;
bool ImGuiProfile::m_addLog = false;

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ImGuiProfile::ImGuiProfile()
	: m_inifile(false)
	, m_pWindow(nullptr)
	, m_pFpsTimer(nullptr)
	, m_pGraphicsDevice(nullptr)
	, m_pAudioDevice(nullptr)
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

	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
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

	SceneMonitor(wflags);
	AudioMonitor(wflags);
	LogMonitor(wflags);

	//終了
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//-----------------------------------------------------------------------------
// ログを追加
//-----------------------------------------------------------------------------
void ImGuiProfile::AddLog(std::string_view log, const std::source_location& location)
{
	time_t jikoku;
	tm ltime;

	time(&jikoku);
	localtime_s(&ltime, &jikoku);

	std::string buff;
	buff += "[" + ToString(ltime.tm_hour) + ":" + ToString(ltime.tm_min) + ":" + ToString(ltime.tm_sec) + "]: ";
	buff += log.substr();
	buff += "\n";

	m_logBuffer.append(buff.c_str());
	m_addLog = true;
}

//-----------------------------------------------------------------------------
// ゲームシーン監視
//-----------------------------------------------------------------------------
void ImGuiProfile::SceneMonitor(ImGuiWindowFlags wflags)
{
	if (!ImGui::Begin("Test Monitor", nullptr, wflags)) { ImGui::End(); return; }

	ImGui::Text(std::string("Fps: " + std::to_string(m_pFpsTimer->GetFPS())).c_str());
	ImGui::Text(std::string("DeltaTime: " + std::to_string(m_pFpsTimer->GetDeltaTime())).c_str());
	ImGui::Text(std::string("TotalTime: " + std::to_string(m_pFpsTimer->GetTotalTime())).c_str());
	ImGui::Text(std::string("TimeScale: " + std::to_string(m_pFpsTimer->GetTimeScale())).c_str());

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

	ImGui::Separator();

	static float volume = m_pAudioDevice->GetMasterVolume();
	if (ImGui::SliderFloat("Main", &volume, 0.0f, 1.0f, "%.2f"))
		m_pAudioDevice->SetMasterVolume(volume);

	static std::weak_ptr<Sound> wpSound;
	for (auto& sound : m_pAudioDevice->GetSoundList())
	{
		ImGui::PushID(sound.get());

		bool selected = (sound == wpSound.lock());
		if (ImGui::Selectable(sound->GetName().c_str(), selected))
			wpSound = sound;

		ImGui::PopID();
	}

	if (wpSound.lock())
	{
		ImGui::Text(std::string("Select: " + wpSound.lock()->GetName()).c_str());

		auto val = wpSound.lock()->GetVolume();
		if (ImGui::SliderFloat("Volume", &val, 0.0f, 2.0f, "%.2f"))
			wpSound.lock()->SetVolume(val);

		static float frequencym = 0.0f;
		if (ImGui::SliderFloat("frequencym", &frequencym, 0.0f, XAUDIO2_MAX_FILTER_FREQUENCY, "%.2f")) {}
		static float oneOverQ = 0.0f;
		if (ImGui::SliderFloat("oneOverQ", &oneOverQ, 0.1f, XAUDIO2_MAX_FILTER_ONEOVERQ, "%.2f")) {}

		if (ImGui::Button("LowPassFilter         ")) wpSound.lock()->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, frequencym, oneOverQ);
		if (ImGui::Button("BandPassFilter        ")) wpSound.lock()->SetFilter(XAUDIO2_FILTER_TYPE::BandPassFilter, frequencym, oneOverQ);
		if (ImGui::Button("HighPassFilter        ")) wpSound.lock()->SetFilter(XAUDIO2_FILTER_TYPE::HighPassFilter, frequencym, oneOverQ);
		if (ImGui::Button("NotchFilter           ")) wpSound.lock()->SetFilter(XAUDIO2_FILTER_TYPE::NotchFilter, frequencym, oneOverQ);
		if (ImGui::Button("LowPassOnePoleFilter  ")) wpSound.lock()->SetFilter(XAUDIO2_FILTER_TYPE::LowPassOnePoleFilter, frequencym, oneOverQ);
		if (ImGui::Button("HighPassOnePoleFilter ")) wpSound.lock()->SetFilter(XAUDIO2_FILTER_TYPE::HighPassOnePoleFilter, frequencym, oneOverQ);
		if (ImGui::Button("Filter OFF            ")) wpSound.lock()->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 1, 1);

	}
	else ImGui::Text("Select: none");

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
			m_logBuffer.clear();
		}
		if (ImGui::Button("Copy"))
		{
			ImGui::SetClipboardText(m_logBuffer.c_str());
			AddLog("INFO: Log copy done.");
		}
		ImGui::EndMenuBar();
	}

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255));
	ImGui::TextUnformatted(m_logBuffer.begin());
	ImGui::PopStyleColor();

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
		valuesA[values_offset] = m_pAudioDevice->m_peakLevels[0];
		valuesB[values_offset] = m_pAudioDevice->m_peakLevels[1];
		valuesC[values_offset] = m_pAudioDevice->m_RMSLevels[0];
		valuesD[values_offset] = m_pAudioDevice->m_RMSLevels[1];

		values_offset = (values_offset + 1) % IM_ARRAYSIZE(valuesA);
		refresh_time += 1.0f / refreshRate;
	}

	static constexpr float down = 0.0f, up = 1.0f;

	char overlay[32];
	sprintf_s(overlay, "avg %f", m_pAudioDevice->m_peakLevels[0]);
	ImGui::PlotLines("PeakLevels L", valuesA, IM_ARRAYSIZE(valuesA), values_offset, overlay, down, up, ImVec2(200, 60.0f));

	sprintf_s(overlay, "avg %f", m_pAudioDevice->m_peakLevels[1]);
	ImGui::PlotLines("PeakLevels R", valuesB, IM_ARRAYSIZE(valuesB), values_offset, overlay, down, up, ImVec2(200, 60.0f));

	sprintf_s(overlay, "avg %f", m_pAudioDevice->m_RMSLevels[0]);
	ImGui::PlotLines("RMSLevels L", valuesC, IM_ARRAYSIZE(valuesC), values_offset, overlay, down, up, ImVec2(200, 60.0f));

	sprintf_s(overlay, "avg %f", m_pAudioDevice->m_RMSLevels[1]);
	ImGui::PlotLines("RMSLevels R", valuesD, IM_ARRAYSIZE(valuesD), values_offset, overlay, down, up, ImVec2(200, 60.0f));
}
