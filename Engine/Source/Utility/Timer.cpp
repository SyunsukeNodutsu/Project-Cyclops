#include "Timer.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
FpsTimer::FpsTimer()
	: m_frequency()
	, m_lastTime()
	, m_maxDelta(0)
	, m_deltaTicks(0)
	, m_totalTicks(0)
	, m_totalFrameCount(0)
	, m_frameThisCount(0)
	, m_fps(0)
	, m_secondCounter(0)
	, m_scaling(1.0f)
{
	//周波数保存
	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_lastTime);

	m_maxDelta = m_frequency.QuadPart / 10;
}

//-----------------------------------------------------------------------------
// 前フレームからのデルタタイムを計算
//-----------------------------------------------------------------------------
void FpsTimer::Tick()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	//前回のTickからの経過時間
	uint64_t deltaTime = currentTime.QuadPart - m_lastTime.QuadPart;
	m_lastTime = currentTime;
	m_secondCounter += deltaTime;

	//何らかの理由で前回から時間が立ちすぎているときはm_maxDeltaをdelta_timeにする
	//そのまま使うと時間が大きすぎてゲームの処理が破綻するのを防ぐ
	if (deltaTime > m_maxDelta) {
		deltaTime = m_maxDelta;
		Debug::Log("処理落ちしました システム起動からの経過時間: " + ToString(TicksToSeconds(m_totalTicks)));
	}

	//時間として扱えるように計算
	deltaTime = deltaTime * TicksPerSecond / m_frequency.QuadPart;

	//それぞれ更新
	m_deltaTicks = deltaTime;
	m_totalTicks += deltaTime;
	++m_totalFrameCount;
	++m_frameThisCount;

	//1秒経過したら呼び出されたTick()の数をfpsとする ※FPSは1秒更新
	if (static_cast<uint64_t>(m_frequency.QuadPart) <= m_secondCounter)
	{
		m_fps = m_frameThisCount;
		m_frameThisCount = 0;

		//0にすると端数の時間が失われてタイマが徐々に狂う
		//剰余で端数の時間を保存しておく
		m_secondCounter %= m_frequency.QuadPart;
	}
}

//-----------------------------------------------------------------------------
// 経過時間リセット
//-----------------------------------------------------------------------------
void FpsTimer::ResetDeltaTime()
{
	QueryPerformanceCounter(&m_lastTime);
	m_frameThisCount = 0;
	m_fps = 0;
	m_secondCounter = 0;
}
