//-----------------------------------------------------------------------------
// File: Timer.h
//
// 各種タイマー
// TODO: FpsTimerはInputと同様ウィンドウのループに依存
//-----------------------------------------------------------------------------
#pragma once

//汎用時間計測タイマー
struct CommonTimer
{
	//@brief コンストラクタ レコードの開始
	CommonTimer() { Record(); }

	//@brief 基準となるタイムスタンプの記録
	inline void Record() {
		m_timeStamp = std::chrono::high_resolution_clock::now();
	}

	//@brief 最後にRecord()が呼ばれてからの経過時間(秒)を返す
	//@return 経過時間(秒)
	inline double GetElapsedSeconds() {
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - m_timeStamp);
		return time_span.count();
	}

	//@brief 最後にRecord()が呼ばれてからの経過時間(ミリ秒)を返す
	//@return 経過時間(ミリ秒)
	inline double GetElapsedMilliseconds() {
		return GetElapsedSeconds() * 1000.0;
	}

private:

	std::chrono::high_resolution_clock::time_point m_timeStamp;

};

//FPS計測用タイマー
//TODO: 時間のスケーリングはここで管理するべきではないかも
class FpsTimer
{
public:

	FpsTimer();
	~FpsTimer() = default;

	void Tick();
	void ResetDeltaTime();

	static double GetTimeScale() { return m_scaling; }
	static double GetDeltaTime(bool modeRaw = false) { return TicksToSeconds(m_deltaTicks) * (modeRaw ? 1 : m_scaling); }
	static double GetTotalTime() { return TicksToSeconds(m_totalTicks); }

	static uint32_t GetFPS() { return m_fps; }
	static uint32_t GetTotalFrameCount() { return m_totalFrameCount; }

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
	static double TicksToSeconds(const std::uint64_t ticks) { return static_cast<double>(ticks) / TicksPerSecond; }
	static std::uint64_t SecondsToTicks(const double seconds) { return static_cast<std::uint64_t>(seconds * TicksPerSecond); }

};

