//-----------------------------------------------------------------------------
// File: Sound.h
//
// 音声データ単位 対応フォーマット(3GP,ASF,AVI,MP3,MPEG-4,SAMI,wav)
//-----------------------------------------------------------------------------
#pragma once
#include "AudioDeviceChild.h"

//サウンドデータ
class SoundData : public AudioDeviceChild
{
public:

	bool Create(const std::wstring& filepath, bool loop, bool useFilter);
	void Release();

	XAUDIO2_BUFFER		m_buffer;
	WAVEFORMATEX*		m_pWaveFormat;//WAVデータのフォーマット
	std::vector<BYTE>	m_mediaData;

	std::wstring		m_filepath;
	bool				m_loop;
	bool				m_useFilter;

};

//サウンドクラス
class Sound : public AudioDeviceChild
{
public:

	Sound(const std::wstring& filepath, bool loop, bool useFilter);
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

	float GetVolume();
	bool IsPlaying();

private:

	SoundData				m_soundData;	//バッファ,フォーマット,ユーザーデータなど
	IXAudio2SourceVoice*	m_pSourceVoice;	//ソースボイス

	CommonTimer				m_timer;		//フェード用タイマ
	float					m_fadeVolume;	//秒数ごとの変化量
	float					m_startVolume;	//フェード開始時の音量
	float					m_targetVolume;	//目標音量
	float					m_targetTime;	//目標時間
	bool					m_fade;			//フェードを行っている
	bool					m_done;			//解放済み
	float					m_prevVolume;	//音量が変化する前の音量

private:

	void UpdateFade();
	bool Load(const std::wstring& filepath, bool loop, bool useFilter);//任意のタイミングで呼びたいかもしれない
	bool SubmitBuffer(bool loop, UINT32 playBegin);

};

//@brief ソースボイスにフィルターを設定 (LowPassFilter, 1, 1)でフィルタをOFF
//@param type フィルタタイプ
//@param frequencym カットオフ周波数(ラジアン周波数)
//@param oneOverQ フィルタに使用するQ値の逆数 ※通常は1.0fか1.4142fを使用する
//@return 成功...true 失敗...false