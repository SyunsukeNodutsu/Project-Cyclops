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

//FPS計測用タイマー ※TODO: 時間のスケーリングはここで管理するべきではないかも
class FpsTimer
{
public:

	FpsTimer();
	~FpsTimer() = default;

	void Tick();
	void ResetDeltaTime();

	double GetTimeScale() const { return m_scaling; }
	double GetDeltaTime(bool modeRaw = false) const { return TicksToSeconds(m_deltaTicks) * (modeRaw ? 1 : m_scaling); }
	double GetTotalTime() const { return TicksToSeconds(m_totalTicks); }

	std::uint32_t GetFPS() const { return m_fps; }
	std::uint32_t GetTotalFrameCount() const { return m_totalFrameCount; }

	void SetTimeScale(double scale) { m_scaling = scale; }

	static constexpr std::uint64_t TicksPerSecond = 10'000'000;
	static double TicksToSeconds(const std::uint64_t ticks) { return static_cast<double>(ticks) / TicksPerSecond; }
	static std::uint64_t SecondsToTicks(const double seconds) { return static_cast<std::uint64_t>(seconds * TicksPerSecond); }

public:

	static std::uint64_t DeltaTime;

private:

	LARGE_INTEGER	m_frequency;		//高解像度タイマの周波数
	LARGE_INTEGER	m_lastTime;			//前回計測時間

	std::uint64_t	m_maxDelta;			//許容する最大のデルタタイム
	std::uint64_t	m_deltaTicks;		//前フレームからのデルタティック
	std::uint64_t	m_totalTicks;		//このタイマの総経過ティック

	std::uint32_t	m_totalFrameCount;	//総フレーム数
	std::uint32_t	m_frameThisCount;	//現在の1秒のフレーム数を数える

	std::uint32_t	m_fps;				//前の1秒のフレームレート
	std::uint64_t	m_secondCounter;	//1秒を計測

	double			m_scaling;			//時間の経過をスケーリング

};

