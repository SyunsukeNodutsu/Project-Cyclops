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
	void Start(const std::string &stampName, const std::source_location& location = std::source_location::current())
	{
		m_stampName = stampName;
		m_start = std::chrono::system_clock::now();
		Debug::Log("計測開始(" + m_stampName + ")", location);
	}

	//@brief 計測終了 ログにミリ秒表示
	void End(const std::source_location& location = std::source_location::current())
	{
		const auto& end = std::chrono::system_clock::now();//計測終了時間
		const auto& time = end - m_start;//処理に要した時間

		const auto& msec = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
		Debug::Log("計測完了(" + m_stampName + ") 時間(ミリ秒) : " + ToString(msec), location);
	}

private:

	std::chrono::system_clock::time_point m_start;
	std::string m_stampName;

};
