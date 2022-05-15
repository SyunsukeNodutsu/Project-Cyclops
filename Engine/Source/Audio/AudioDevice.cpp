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

#if _DEBUG && 0
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
    FadeOutSoundList(0.2f);

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
void AudioDevice::Update(const Matrix& listener)
{
    //TOOD: サウンドリスナー更新

    //サウンドリスト更新
    for (const auto& sound : m_soundList)
        sound->UpdateFade();

    UpdateVolumeMeter();
}

//-----------------------------------------------------------------------------
// マスター音量を設定
//-----------------------------------------------------------------------------
void AudioDevice::SetMasterVolume(float volume)
{
    volume = std::clamp(volume, -1.0f, 1.0f);
    m_pMasteringVoice->SetVolume(volume);
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

    XAUDIO2_VOICE_DETAILS details;
    m_pMasteringVoice->GetVoiceDetails(&details);

    //エフェクトチェイン作成
    XAUDIO2_EFFECT_DESCRIPTOR descriptor;
    descriptor.InitialState     = true;
    descriptor.OutputChannels   = details.InputChannels;
    descriptor.pEffect          = pVolumeMeterAPO;

    XAUDIO2_EFFECT_CHAIN chain;
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
    m_peakLevels.fill(0);
    m_RMSLevels.fill(0);

    XAUDIO2_VOICE_DETAILS details;
    m_pMasteringVoice->GetVoiceDetails(&details);

    XAUDIO2FX_VOLUMEMETER_LEVELS Levels;
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
// サウンドリストをフェードアウトさせる
//-----------------------------------------------------------------------------
void AudioDevice::FadeOutSoundList(float fadeSec)
{
    for (const auto& sound : m_soundList)
        sound->SetFade(0.0f, fadeSec);

    std::chrono::high_resolution_clock::time_point m_timeStamp;
    m_timeStamp = std::chrono::high_resolution_clock::now();

    while (true)
    {
        const auto& now = std::chrono::high_resolution_clock::now();
        const auto& time_span = std::chrono::duration_cast<std::chrono::duration<float>>(now - m_timeStamp);
        const float& total = time_span.count() * 1000.0f;

        for (const auto& sound : m_soundList)
            sound->UpdateFade();

        if (total >= fadeSec * 1000.0f)
            break;
    }
}
