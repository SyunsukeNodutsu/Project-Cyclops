//-----------------------------------------------------------------------------
// File: Sound.h
//
// 音声データ単位
//-----------------------------------------------------------------------------
#pragma once
#include "AudioDeviceChild.h"

//サウンドデータ
class SoundData : public AudioDeviceChild
{
public:

	bool Create(const std::string& filepath, bool loop, bool useFilter);
	void Release();

	XAUDIO2_BUFFER		m_buffer;
	WAVEFORMATEX*		m_pWaveFormat;//WAVデータのフォーマット
	std::vector<BYTE>	m_mediaData;

	std::string			m_filepath;
	bool				m_loop;
	bool				m_useFilter;

};

//サウンドクラス
class Sound : public AudioDeviceChild
{
public:

	Sound(const std::string& filepath, bool loop, bool useFilter);
	~Sound() { Release(); }

	void Update();
	void Release();

	void Play(bool flush = false, DWORD delay = 0);
	void Stop(bool flush = false);

	void SetVolume(float volume);
	void SetFade(float volume, float time);
	void SetPitch(float pitch);
	void SetPan(float pan);
	void SetFilter(XAUDIO2_FILTER_TYPE type, float frequencym, float oneOverQ = 1.0f);
	void SetEqualizer(FXEQ_PARAMETERS eqParam);

	float GetVolume();
	bool IsPlaying();

	const std::string& GetName() { return Utility::GetFilenameFromFullpath(m_soundData.m_filepath); }
	const unsigned int GetSeconds() const { return m_soundSeconds; }
	const unsigned int GetElapsedSeconds() const { return m_soundElapsedSeconds; }

private:

	SoundData				m_soundData;	//バッファ,フォーマット,ユーザーデータなど
	IXAudio2SourceVoice*	m_pSourceVoice;	//ソースボイス

	CommonTimer				m_seekTimer;	//再生位置用タイマ
	float					m_prevVolume;	//音量が変化する前の音量
	bool					m_done;			//解放済み
	unsigned int			m_soundSeconds;	//サウンドの秒数
	unsigned int			m_soundElapsedSeconds;//サウンドの経過秒数

	//フェード用
	CommonTimer				m_timer;		//フェード用タイマ
	float					m_fadeVolume;	//秒数ごとの変化量
	float					m_startVolume;	//フェード開始時の音量
	float					m_targetVolume;	//目標音量
	float					m_targetTime;	//目標時間
	bool					m_fade;			//フェードを行っている

private:

	void UpdateFade();
	bool Load(const std::string& filepath, bool loop, bool useFilter);//任意のタイミングで呼びたいかもしれない
	bool SubmitBuffer(bool loop, UINT32 playBegin);

};

//@brief ソースボイスにフィルターを設定 (LowPassFilter, 1, 1)でフィルタをOFF
//@param type フィルタタイプ
//@param frequencym カットオフ周波数(ラジアン周波数)
//@param oneOverQ フィルタに使用するQ値の逆数 ※通常は1.0fか1.4142fを使用する
//@return 成功...true 失敗...false