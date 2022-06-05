#include "AudioDevice.h"
#include "Sound.h"
#include "Sound3D.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
AudioDevice::AudioDevice()
	: m_pX2Audio(nullptr)
	, m_pMasteringVoice(nullptr)
	, m_pSubmixVoice(nullptr)
	, m_soundList()
	, m_peakLevels()
	, m_RMSLevels()
	, m_prevVolume(FLT_MIN)

    , m_timer()
    , m_fadeVolume(0.0f)
    , m_startVolume(0.0f)
    , m_targetVolume(0.0f)
    , m_targetTime(0.0f)
    , m_fade(false)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool AudioDevice::Initialize()
{
    //IXAudio2作成
    UINT32 flags = 0;
    HRESULT hr = XAudio2Create(&m_pX2Audio, flags);
    if (m_pX2Audio == nullptr)
    {
        Debug::Log("IXAudio2作成失敗."); return false;
    }

#if _DEBUG
    XAUDIO2_DEBUG_CONFIGURATION debug{ 0 };
    debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
    debug.BreakMask = XAUDIO2_LOG_ERRORS;
    m_pX2Audio->SetDebugConfiguration(&debug, 0);
#endif

    //IXAudio2MasteringVoice作成
    hr = m_pX2Audio->CreateMasteringVoice(&m_pMasteringVoice);
    if (m_pMasteringVoice == nullptr)
    {
        Debug::Log("IXAudio2MasteringVoice作成失敗."); return false;
    }

    if (!CreateVolumeMeter())
        return false;

	return true;
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void AudioDevice::Finalize()
{
    //最終フェードアウト ※TODO: Soudクラスとロジックは一緒なので共通化
    SetFadeMaster(0.0f, 0.4f);
    while (true)
    {
        UpdateFade();
        if (!m_fade) break;
    }

    for (const auto& sound : m_soundList)
        sound->Release();

    if (m_pMasteringVoice)
    {
        m_pMasteringVoice->DestroyVoice();
        m_pMasteringVoice = nullptr;
    }
    if (m_pX2Audio)
    {
        m_pX2Audio->StopEngine();
        m_pX2Audio->Release();
        m_pX2Audio = nullptr;
    }
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void AudioDevice::Update(const matrix4x4& listener)
{
    if (m_pX2Audio == nullptr) return;
    if (m_pMasteringVoice == nullptr) return;

    //TOOD: サウンドリスナー更新

    //サウンドリスト更新
    for (auto itr = m_soundList.begin(); itr != m_soundList.end();)
    {
        if (!(*itr)->IsPlaying())
        {
            itr = m_soundList.erase(itr);
            continue;
        }
        else
        {
            (*itr)->Update();
        }
        ++itr;
    }


    UpdateFade();
    UpdateVolumeMeter();
}

//-----------------------------------------------------------------------------
// マスター音量を設定
//-----------------------------------------------------------------------------
void AudioDevice::SetMasterVolume(float volume)
{
    if (m_pX2Audio == nullptr) return;
    if (m_pMasteringVoice == nullptr) return;
    if (volume == m_prevVolume) { Debug::Log("以前の音量と同じなのでSetVolumeを飛ばします."); return; }

    volume = std::clamp(volume, -1.0f, 1.0f);
    m_pMasteringVoice->SetVolume(volume);
    m_prevVolume = volume;
}

//-----------------------------------------------------------------------------
// マスター音量を返す
//-----------------------------------------------------------------------------
float AudioDevice::GetMasterVolume()
{
    if (m_pX2Audio == nullptr) return FLT_MIN;
    if (m_pMasteringVoice == nullptr) return FLT_MIN;

    float volume; m_pMasteringVoice->GetVolume(&volume);
    return volume;
}

//-----------------------------------------------------------------------------
// マスター音量をフェードさせる
//-----------------------------------------------------------------------------
void AudioDevice::SetFadeMaster(float volume, float time)
{
    if (m_pX2Audio == nullptr) return;
    if (m_pMasteringVoice == nullptr) return;
    if (GetMasterVolume() == 0) return;//TOOD: あまりに小さい音量であれば必要ない

    //許容値
    time    = std::clamp(time, 0.0f, 5.0f);
    volume  = std::clamp(volume, -1.0f, 1.0f);

    m_fadeVolume    = (volume - GetMasterVolume()) / time;//時間ごとの変化量計算
    m_startVolume   = GetMasterVolume();
    m_targetTime    = time;
    m_targetVolume  = volume;
    m_fade          = true;

    //タイマ計測開始
    m_timer.Record();
}

//-----------------------------------------------------------------------------
// サウンドリストすべてをフェードさせる
//-----------------------------------------------------------------------------
void AudioDevice::SetFadeSoundList(float volume, float time)
{
    if (m_pX2Audio == nullptr) return;
    if (m_pMasteringVoice == nullptr) return;

    for (const auto& sound : m_soundList)
        sound->SetFade(volume, time);
}

//-----------------------------------------------------------------------------
// ボリュームメーター作成
//-----------------------------------------------------------------------------
bool AudioDevice::CreateVolumeMeter()
{
    if (m_pX2Audio == nullptr) return false;
    if (m_pMasteringVoice == nullptr) return false;

    //VolumeMeter(APO)作成
    IUnknown* pVolumeMeterAPO = nullptr;
    if (FAILED(XAudio2CreateVolumeMeter(&pVolumeMeterAPO)))
    {
        Debug::Log("XAudio2CreateVolumeMeter失敗."); return false;
    }

    XAUDIO2_VOICE_DETAILS details{};
    m_pMasteringVoice->GetVoiceDetails(&details);

    //エフェクトチェイン作成
    XAUDIO2_EFFECT_DESCRIPTOR descriptor{};
    descriptor.InitialState     = true;
    descriptor.OutputChannels   = details.InputChannels;
    descriptor.pEffect          = pVolumeMeterAPO;

    XAUDIO2_EFFECT_CHAIN chain{};
    chain.EffectCount           = 1;
    chain.pEffectDescriptors    = &descriptor;
    if (FAILED(m_pMasteringVoice->SetEffectChain(&chain)))
    {
        Debug::Log("SetEffectChain失敗."); return false;
    }

    pVolumeMeterAPO->Release();

    return true;
}

//-----------------------------------------------------------------------------
// ボリュームメーター更新
//-----------------------------------------------------------------------------
void AudioDevice::UpdateVolumeMeter()
{
    if (m_pX2Audio == nullptr) return;
    if (m_pMasteringVoice == nullptr) return;

    m_peakLevels.fill(0);
    m_RMSLevels.fill(0);

    XAUDIO2_VOICE_DETAILS details{};
    m_pMasteringVoice->GetVoiceDetails(&details);

    XAUDIO2FX_VOLUMEMETER_LEVELS Levels{};
    Levels.pPeakLevels  = &m_peakLevels[0];
    Levels.pRMSLevels   = &m_RMSLevels[0];
    Levels.ChannelCount = details.InputChannels;

    //パラメータ受信
    if (FAILED(m_pMasteringVoice->GetEffectParameters(0, &Levels, sizeof(Levels))))
    {
        Debug::Log("GetEffectParameters失敗.");
    }
}

//-----------------------------------------------------------------------------
// フェード更新
//-----------------------------------------------------------------------------
void AudioDevice::UpdateFade()
{
    if (m_pX2Audio == nullptr) return;
    if (m_pMasteringVoice == nullptr) return;
    if (!m_fade) return;

    //タイマから経過時間を取得し設定する音量を計算
    const float& progress_time = m_timer.GetElapsedSeconds<float>();

    SetMasterVolume(m_startVolume + (m_fadeVolume * progress_time));

    //フェード終了
    if (progress_time >= m_targetTime)
    {
        SetMasterVolume(m_targetVolume);
        m_fade = false;
    }
}
