#include "Pch.h"
#include "ImGuiProfile.h"

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


	//描画
	if (ImGui::Begin("Test Monitor", nullptr, wflags))
	{
		ImGui::Text(std::string("Fps: " + std::to_string(m_pFpsTimer->GetFPS())).c_str());
		ImGui::Text(std::string("DeltaTime: " + std::to_string(m_pFpsTimer->GetDeltaTime())).c_str());
		ImGui::Text(std::string("TotalTime: " + std::to_string(m_pFpsTimer->GetTotalTime())).c_str());
		ImGui::Text(std::string("TimeScale: " + std::to_string(m_pFpsTimer->GetTimeScale())).c_str());

		PlotVolumeMeter(30.0f);

		ImGui::End();
	}

	//終了
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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

	static const float down = -1.0f, up = 1.0f;

	char overlay[32];
	sprintf_s(overlay, "avg %f", m_pAudioDevice->m_peakLevels[0]);
	ImGui::PlotLines("PeakLevels L", valuesA, IM_ARRAYSIZE(valuesA), values_offset, overlay, down, up, ImVec2(0, 60.0f));

	sprintf_s(overlay, "avg %f", m_pAudioDevice->m_peakLevels[1]);
	ImGui::PlotLines("PeakLevels R", valuesB, IM_ARRAYSIZE(valuesB), values_offset, overlay, down, up, ImVec2(0, 60.0f));

	sprintf_s(overlay, "avg %f", m_pAudioDevice->m_RMSLevels[0]);
	ImGui::PlotLines("RMSLevels L", valuesC, IM_ARRAYSIZE(valuesC), values_offset, overlay, down, up, ImVec2(0, 60.0f));

	sprintf_s(overlay, "avg %f", m_pAudioDevice->m_RMSLevels[1]);
	ImGui::PlotLines("RMSLevels R", valuesD, IM_ARRAYSIZE(valuesD), values_offset, overlay, down, up, ImVec2(0, 60.0f));
}
