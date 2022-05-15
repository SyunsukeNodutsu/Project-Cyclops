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
	WAVEFORMATEX*		m_pWaveFormat;  //WAVデータのフォーマット
	std::vector<BYTE>	m_mediaData;

};

//サウンドクラス
class Sound : public AudioDeviceChild
{
public:

	//@brief コンストラクタ
	Sound();

	//@brief デストラクタ
	~Sound() { Release(); }

	//@brief 解放
	void Release();

	//@brief 読み込み
	bool Load(const std::wstring& filepath, bool loop, bool useFilter);

	//@brief 再生
	//@param flush 最初から再生
	//@param delay 再生遅延 ミリ秒(1000...1秒)
	void Play(bool flush = false, DWORD delay = 0);

	//@brief 停止
	//@param flush シークをリセット ※次回の再生では最初からになる
	void Stop(bool flush = false);

	//@brief フェード設定
	//@param volume 目的の音量
	//@param time フェードが完了する時間(秒)
	void SetFade(float volume, float time);

	//@brief フェード更新
	void UpdateFade();

	//@brief ピッチ設定
	void SetPitch(float pitch);

	//@brief 音量設定
	void SetVolume(float volume);

	//@brief 音量を返す
	float GetVolume();

	//@brief 再生中かどうかを返す
	bool IsPlaying();

	//@brief 音を左右に振り分ける
	//@oaram pan -1:左 1:右
	bool SetPan(float pan);

	//@brief ソースボイスにフィルターを設定 (LowPassFilter, 1, 1)でフィルタをOFF
	//@param type フィルタタイプ
	//@param frequencym カットオフ周波数 ※実際はラジアン周波数を入れる
	//@param oneOverQ フィルタに使用するQ値の逆数 ※通常は1.0fか1.4142fを使用する
	//@return 成功...true 失敗...false
	bool SetFilter(XAUDIO2_FILTER_TYPE type, float frequencym, float oneOverQ = 1.0f);

private:

	SoundData				m_soundData;
	IXAudio2SourceVoice*	m_pSourceVoice;
	std::wstring			m_filepath;
	bool					m_loop;

	CommonTimer				m_timer;//フェード用タイマ
	float					m_fadeVolume;//秒数ごとの変化量
	float					m_startVolume;//フェード開始時の音量

	float					m_targetVolume;//目標音量
	float					m_targetTime;//目標時間

	bool					m_fade;
	bool					m_done;//解放済み

private:

	//@brief オーディオバッファを追加
	bool SubmitBuffer(bool loop, UINT32 playBegin);

};
