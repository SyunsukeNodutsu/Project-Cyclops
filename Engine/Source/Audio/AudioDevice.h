//-----------------------------------------------------------------------------
// File: AudioDevice.h
//
// エンジンのオーディオ機能(X2Audio/X3DAudioの機能をラップ)
//-----------------------------------------------------------------------------
#pragma once

class Sound;

//オーディオデバイス
class AudioDevice
{
public:

	AudioDevice();
	~AudioDevice() = default;

    bool Initialize();
    void Finalize();

    void Update(const Matrix& listener);

    void AddSound(const std::shared_ptr<Sound> sound) { m_soundList.push_back(sound); }
    const std::list<std::shared_ptr<Sound>>& GetSoundList() { return m_soundList; }

    void SetMasterVolume(float volume);
    float GetMasterVolume();

    void SetFadeMaster(float volume, float time);
    void SetFadeSoundList(float volume, float time);

public:

    IXAudio2*                   m_pX2Audio;
    IXAudio2MasteringVoice*     m_pMasteringVoice;
    IXAudio2SubmixVoice*        m_pSubmixVoice;

private:

    std::list<std::shared_ptr<Sound>>   m_soundList;    //サウンドリスト
    std::array<float, 8>                m_peakLevels;   //ピークメータ ※瞬間最大値
    std::array<float, 8>                m_RMSLevels;    //RMSメータ    ※平均値
    float                               m_prevVolume;   //以前の音量

    CommonTimer				            m_timer;		//フェード用タイマ
    float					            m_fadeVolume;	//秒数ごとの変化量
    float					            m_startVolume;	//フェード開始時の音量
    float					            m_targetVolume;	//目標音量
    float					            m_targetTime;	//目標時間
    bool					            m_fade;			//フェードを行っている

private:

    bool CreateVolumeMeter();
    void UpdateVolumeMeter();
    void UpdateFade();

};
