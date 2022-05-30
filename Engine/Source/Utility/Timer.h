//-----------------------------------------------------------------------------
// File: Timer.h
//
// 各種タイマー
// TODO: FpsTimerはInputと同様ウィンドウのループに依存
//-----------------------------------------------------------------------------
#pragma once

//汎用時間計測タイマー
class CommonTimer
{
public:

	template<class T> static const T GetElapsedSeconds();
	template<class T> static const T GetElapsedMilliseconds();

public:

	//@brief コンストラクタ レコードの開始
	CommonTimer() { Record(); }

	//@brief 基準となるタイムスタンプの記録
	inline void Record() {
		m_timeStamp = std::chrono::high_resolution_clock::now();
	}

	//@brief 最後にRecord()が呼ばれてからの経過時間(秒)を返す
	//@return 経過時間(秒)
	template<> static const double GetElapsedSeconds() {
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - m_timeStamp);
		return time_span.count();
	}
	template<> static const float GetElapsedSeconds() {
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::duration<float>>(now - m_timeStamp);
		return time_span.count();
	}

	//@brief 最後にRecord()が呼ばれてからの経過時間(ミリ秒)を返す
	//@return 経過時間(ミリ秒)
	template<> static const double GetElapsedMilliseconds() { return GetElapsedSeconds<double>() * 1000.0; }
	template<> static const float GetElapsedMilliseconds() { return GetElapsedSeconds<float>() * 1000.0f; }

private:

	static std::chrono::high_resolution_clock::time_point m_timeStamp;

};

//FPS計測用タイマー
//TODO: 時間のスケーリングはここで管理するべきではないかも
class FpsTimer
{
public:

	//非型テンプレートパラメータとしてクラス型を許可
	template<class T> static constexpr T GetDeltaTime(bool modeRaw = false);
	template<class T> static constexpr T GetTotalTime();
	template<class T> static constexpr T GetTimeScale();

public:

	FpsTimer();
	~FpsTimer() = default;

	void Tick();
	void ResetDeltaTime();

	//@brief 1フレーム前からの経過時間を返す
	//@param modeRaw true...TimeScaleの影響を受けない
	template<> static constexpr double GetDeltaTime(bool modeRaw) { return TicksToSeconds(m_deltaTicks) * (modeRaw ? 1 : m_scaling); }
	template<> static constexpr float  GetDeltaTime(bool modeRaw) { return static_cast<float>(TicksToSeconds(m_deltaTicks) * (modeRaw ? 1 : m_scaling)); }

	//@brief システム起動からの経過時間を返す
	template<> static constexpr double GetTotalTime() { return TicksToSeconds(m_totalTicks); }
	template<> static constexpr float  GetTotalTime() { return static_cast<float>(TicksToSeconds(m_totalTicks)); }

	//@brief 時間のスケーリングを返す
	template<> static constexpr double GetTimeScale() { return m_scaling; }
	template<> static constexpr float  GetTimeScale() { return static_cast<float>(m_scaling); }

	//@brief FPSを返す
	static uint32_t GetFPS() { return m_fps; }

	//@brief システム起動からの総フレーム数を返す
	static uint32_t GetTotalFrameCount() { return m_totalFrameCount; }

	//@brief 時間のスケーリングを設定する
	static void SetTimeScale(double scale) { m_scaling = scale; }

private:

	LARGE_INTEGER	m_frequency;		//高解像度タイマの周波数
	LARGE_INTEGER	m_lastTime;			//前回計測時間

	uint64_t		m_maxDelta;			//許容する最大のデルタタイム
	uint32_t		m_frameThisCount;	//現在の1秒のフレーム数を数える
	uint64_t		m_secondCounter;	//1秒を計測

	static uint64_t	m_deltaTicks;		//前フレームからのデルタティック
	static uint64_t	m_totalTicks;		//このタイマの総経過ティック
	static uint32_t	m_totalFrameCount;	//総フレーム数
	static uint32_t	m_fps;				//前の1秒のフレームレート
	static double	m_scaling;			//時間の経過をスケーリング

private:

	static constexpr std::uint64_t TicksPerSecond = 10'000'000;
	static constexpr double TicksToSeconds(const std::uint64_t ticks) { return static_cast<double>(ticks) / TicksPerSecond; }
	static std::uint64_t SecondsToTicks(const double seconds) { return static_cast<std::uint64_t>(seconds * TicksPerSecond); }

};
