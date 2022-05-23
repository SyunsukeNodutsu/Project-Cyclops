//-----------------------------------------------------------------------------
// File: ImGuiProfile.h
//
// ImGuiによる処理計測
//-----------------------------------------------------------------------------
#pragma once

class Window; class FpsTimer;
class GraphicsDevice; class AudioDevice;

//ImGuiによる処理計測クラス
class ImGuiProfile : public ISubsystem
{
public:

	ImGuiProfile();
	~ImGuiProfile() = default;

	void Initialize();
	void Finalize();
	void DrawProfileMonitor();

	static void AddLog(std::string_view log, const std::source_location& location = std::source_location::current());
	static void ClearLog() { m_logBuffer.clear(); }

	void SetWindow(Window* pDevice) { m_pWindow = pDevice; }
	void SetFpsTimer(FpsTimer* pDevice) { m_pFpsTimer = pDevice; }
	void SetGraphicsDevice(GraphicsDevice* pDevice) { m_pGraphicsDevice = pDevice; }
	void SetAudioDevice(AudioDevice* pDevice) { m_pAudioDevice = pDevice; }

private:

	static ImGuiTextBuffer m_logBuffer;//ログ用のImGui専用バッファー
	static bool m_addLog;//ログを追加
	bool m_inifile;//iniを作成するかどうか

	//監視サブシステム群
	Window*			m_pWindow;
	FpsTimer*		m_pFpsTimer;
	GraphicsDevice* m_pGraphicsDevice;
	AudioDevice*	m_pAudioDevice;

private:

	void SceneMonitor(ImGuiWindowFlags wflags);
	void AudioMonitor(ImGuiWindowFlags wflags);
	void LogMonitor(ImGuiWindowFlags wflags);
	void PlotVolumeMeter(const float refreshRate = 30.0f);

};
