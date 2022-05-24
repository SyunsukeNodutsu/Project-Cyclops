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

	static void AddLog(std::string_view log, const Vector3& color = Vector3(1, 1, 1) , const std::source_location& location = std::source_location::current());
	static void ClearLog() { m_logDatas.clear(); }

	void SetWindow(Window* pDevice) { m_pWindow = pDevice; }
	void SetFpsTimer(FpsTimer* pDevice) { m_pFpsTimer = pDevice; }
	void SetGraphicsDevice(GraphicsDevice* pDevice) { m_pGraphicsDevice = pDevice; }
	void SetAudioDevice(AudioDevice* pDevice) { m_pAudioDevice = pDevice; }

private:

	//色のあるログ
	struct LogData
	{
		std::string log = "";
		Vector3 color = Vector3::Zero;
	};
	static std::list<LogData> m_logDatas;
	static bool m_addLog;//ログを追加

	bool m_inifile;//iniを作成するかどうか
	bool m_showDemo;

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
