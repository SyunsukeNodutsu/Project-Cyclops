//-----------------------------------------------------------------------------
// File: Profile.h
//
// 各種計測
//-----------------------------------------------------------------------------
#pragma once

//各種計測クラス
class Profile
{
public:

	Profile() = default;
	~Profile() = default;

	//@brief 計測開始
	void Start()
	{
		m_start = std::chrono::system_clock::now();
	}

	//@brief 計測終了 ログにミリ秒表示
	void End()
	{
		m_end = std::chrono::system_clock::now();//計測終了時間
		auto time = m_end - m_start;//処理に要した時間

		auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
		Debug::Log("計測完了 時間(ミリ秒) : " + ToString(msec));
	}

private:

	std::chrono::system_clock::time_point m_start, m_end;

};
