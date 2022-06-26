#include "Sound.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Sound::Sound(const std::string& filepath, bool loop, bool useFilter)
    : m_soundData()
    , m_pSourceVoice(nullptr)
    , m_timer()
    , m_fadeVolume(0.0f)
    , m_startVolume(0.0f)
    , m_targetVolume(0.0f)
    , m_targetTime(0.0f)
    , m_fade(false)
    , m_done(false)
    , m_prevVolume(1.0f)
{
    if (!Load(filepath, loop, useFilter))
        Release();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void Sound::Update()
{
    UpdateFade();
}

//-----------------------------------------------------------------------------
// 解放
//-----------------------------------------------------------------------------
void Sound::Release()
{
    if (m_done) return;

    if (m_pSourceVoice)
    {
        m_pSourceVoice->DestroyVoice();
        m_pSourceVoice = nullptr;
    }

    m_soundData.Release();

    m_done = true;
}

//-----------------------------------------------------------------------------
// 再生
//-----------------------------------------------------------------------------
void Sound::Play(bool flush, DWORD delay)
{
    if (m_pSourceVoice == nullptr) return;

    Stop(flush);

    if (delay > 0)
    {
        std::thread([=] {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            m_pSourceVoice->Start();
            }).detach();
    }
    else m_pSourceVoice->Start();
}

//-----------------------------------------------------------------------------
// 停止
//-----------------------------------------------------------------------------
void Sound::Stop(bool flush)
{
    if (m_pSourceVoice == nullptr) return;

    m_pSourceVoice->Stop();
    if (flush)
    {
        m_pSourceVoice->FlushSourceBuffers();
        SubmitBuffer(m_soundData.m_loop, 0);
    }
}

//-----------------------------------------------------------------------------
// 音量を設定
//-----------------------------------------------------------------------------
void Sound::SetVolume(float volume)
{
    if (m_pSourceVoice == nullptr) return;
    if (volume == m_prevVolume) { /*Debug::LogWarning("以前の音量と同じなのでSetVolumeを飛ばします.");*/ return; }
    if (const float subb = fabsf(m_prevVolume - volume); subb <= 0.01f) { /*Debug::Log("変化量が極小なのでSetVolumeを飛ばします. 変化量: " + ToString(subb));*/ return; }
    
    volume = std::clamp(volume, -XAUDIO2_MAX_VOLUME_LEVEL, XAUDIO2_MAX_VOLUME_LEVEL);
    m_pSourceVoice->SetVolume(volume);
    m_prevVolume = volume;
}

//-----------------------------------------------------------------------------
// フェード
//-----------------------------------------------------------------------------
void Sound::SetFade(float volume, float time)
{
    if (m_pSourceVoice == nullptr) return;

    //許容値
    time    = std::clamp(time, 0.0f, 5.0f);
    volume  = std::clamp(volume, -1.0f, 1.0f);

    m_fadeVolume    = (volume - GetVolume()) / time;//時間ごとの変化量計算
    m_startVolume   = GetVolume();
    m_targetTime    = time;
    m_targetVolume  = volume;
    m_fade          = true;

    //タイマ計測開始
    m_timer.Record();
}

//-----------------------------------------------------------------------------
// ピッチ設定
//-----------------------------------------------------------------------------
void Sound::SetPitch(float pitch)
{
    if (m_pSourceVoice == nullptr) return;
    m_pSourceVoice->SetFrequencyRatio(pitch);
}

//-----------------------------------------------------------------------------
// 音を左右に振り分ける
//-----------------------------------------------------------------------------
void Sound::SetPan(float pan)
{
    if (m_audioDevice == nullptr) return;
    if (m_audioDevice->m_pX2Audio == nullptr) return;
    if (m_audioDevice->m_pMasteringVoice == nullptr) return;
    if (m_pSourceVoice == nullptr) return;

    //左右のパンの値を計算
    pan = std::clamp(pan, -1.0f, 1.0f);

    float left = 0.5f - pan / 2;
    float right = 0.5f + pan / 2;

    //チャンネル取得
    XAUDIO2_VOICE_DETAILS VoiceDetails{};
    m_pSourceVoice->GetVoiceDetails(&VoiceDetails);

    XAUDIO2_VOICE_DETAILS MasterVoiceDetails{};
    m_audioDevice->m_pMasteringVoice->GetVoiceDetails(&MasterVoiceDetails);

    UINT32 source_channels = VoiceDetails.InputChannels;
    UINT32 destination_channels = MasterVoiceDetails.InputChannels;

    //スピーカー構成に応じて行列の値を設定
    float outputMatrix[8];
    for (int i = 0; i < 8; i++) outputMatrix[i] = 0;
    
    DWORD dwChannelMask{};
    m_audioDevice->m_pMasteringVoice->GetChannelMask(&dwChannelMask);

    outputMatrix[0] = outputMatrix[1] = left;
    outputMatrix[2] = outputMatrix[3] = right;

    if (FAILED(m_pSourceVoice->SetOutputMatrix(m_audioDevice->m_pMasteringVoice, source_channels, destination_channels, outputMatrix)))
    {
        Debug::LogError("SetOutputMatrix失敗.");
    }
}

//-----------------------------------------------------------------------------
// ソースボイスにフィルターを設定
//-----------------------------------------------------------------------------
void Sound::SetFilter(XAUDIO2_FILTER_TYPE type, float frequencym, float oneOverQ)
{
    if (m_pSourceVoice == nullptr) return;

    frequencym = std::clamp(frequencym, 0.0f, XAUDIO2_MAX_FILTER_FREQUENCY);
    oneOverQ = std::clamp(oneOverQ, 0.1f, XAUDIO2_MAX_FILTER_ONEOVERQ);

    XAUDIO2_FILTER_PARAMETERS FilterParams;
    FilterParams.Type = type;
    FilterParams.Frequency = frequencym;
    FilterParams.OneOverQ = oneOverQ;

    if (FAILED(m_pSourceVoice->SetFilterParameters(&FilterParams)))
    {
        Debug::LogError("SetFilterParameters失敗.useFilterフラグを確認してください.");
    }
}

//-----------------------------------------------------------------------------
// イコライザー検証
//-----------------------------------------------------------------------------
void Sound::SetEqualizer(FXEQ_PARAMETERS eqParam)
{
    if (m_audioDevice == nullptr) return;
    if (m_audioDevice->m_pX2Audio == nullptr) return;
    if (m_audioDevice->m_pMasteringVoice == nullptr) return;
    if (m_pSourceVoice == nullptr) return;

    if (FAILED(m_pSourceVoice->SetEffectParameters(0, &eqParam, sizeof(eqParam))))
    {
        Debug::LogError("SetEffectParameters失敗.");
    }
}

//-----------------------------------------------------------------------------
// 音量を返す
//-----------------------------------------------------------------------------
float Sound::GetVolume()
{
    if (m_pSourceVoice == nullptr) return FLT_MIN;

    float volume; m_pSourceVoice->GetVolume(&volume);
    return volume;
}

//-----------------------------------------------------------------------------
// 再生中かどうかを返す
//-----------------------------------------------------------------------------
bool Sound::IsPlaying()
{
    if (m_pSourceVoice == nullptr) return false;

    XAUDIO2_VOICE_STATE pState;
    m_pSourceVoice->GetState(&pState);

    return (pState.BuffersQueued > 0);
}

//-----------------------------------------------------------------------------
// フェード更新
//-----------------------------------------------------------------------------
void Sound::UpdateFade()
{
    if (m_pSourceVoice == nullptr) return;
    if (!m_fade) return;

    //タイマから経過時間を取得し設定する音量を計算
    const float& progress_time = m_timer.GetElapsedSeconds<float>();

    SetVolume(m_startVolume + (m_fadeVolume * progress_time));

    //フェード終了
    if (progress_time >= m_targetTime)
    {
        SetVolume(m_targetVolume);
        m_fade = false;
    }
}

//-----------------------------------------------------------------------------
// 読み込み
//-----------------------------------------------------------------------------
bool Sound::Load(const std::string& filepath, bool loop, bool useFilter)
{
    if (m_audioDevice == nullptr) return false;
    if (m_audioDevice->m_pX2Audio == nullptr) return false;
    if (m_audioDevice->m_pMasteringVoice == nullptr) return false;

    //サウンドデータクラス作成/音源情報読み込み
    if (!m_soundData.Create(filepath, loop, useFilter))
    {
        Debug::LogError("SoundData作成失敗."); return false;
    }

    //送信先になるの宛先ボイスを定義
    XAUDIO2_SEND_DESCRIPTOR sendDescriptors[1];
    //LPFダイレクトパス
    sendDescriptors[0].Flags = XAUDIO2_SEND_USEFILTER;
    sendDescriptors[0].pOutputVoice = m_audioDevice->m_pMasteringVoice;

    const XAUDIO2_VOICE_SENDS sendList = { 1, sendDescriptors };

    //ソースボイス作成
    if (FAILED(m_audioDevice->m_pX2Audio->CreateSourceVoice(
        &m_pSourceVoice, m_soundData.m_pWaveFormat,
        useFilter ? XAUDIO2_VOICE_USEFILTER : 0, 2.0f, nullptr, &sendList, nullptr)))
    {
        Debug::LogError("CreateSourceVoice失敗."); return false;
    }

    //オーディオバッファを追加
    if (!SubmitBuffer(loop, 0))
        return false;

    //サウンドの秒数取得
    {
        XAUDIO2_VOICE_DETAILS voiceDetails = { 0 };
        m_pSourceVoice->GetVoiceDetails(&voiceDetails);

        const UINT NumBytesPerChannelSample = 2;//TODO: 調査
        const UINT bytesPerSample = voiceDetails.InputChannels * NumBytesPerChannelSample;

        m_soundSeconds = m_soundData.m_buffer.AudioBytes / bytesPerSample / voiceDetails.InputSampleRate;
    }

    //TODO: エフェクトチェインの作成は 関数化するべき
    {
        IUnknown* pXAPO;
        if (FAILED(CreateFX(__uuidof(FXEQ), &pXAPO)))
        {
            Debug::Log("CreateFX(FXEQ)失敗.");// return false;
        }

        XAUDIO2_VOICE_DETAILS details{};
        m_audioDevice->m_pMasteringVoice->GetVoiceDetails(&details);

        //エフェクトチェイン作成
        XAUDIO2_EFFECT_DESCRIPTOR descriptor{};
        descriptor.InitialState = true;
        descriptor.OutputChannels = details.InputChannels;
        descriptor.pEffect = pXAPO;

        XAUDIO2_EFFECT_CHAIN chain{};
        chain.EffectCount = 1;
        chain.pEffectDescriptors = &descriptor;
        if (FAILED(m_pSourceVoice->SetEffectChain(&chain)))
        {
            Debug::Log("SetEffectChain失敗.");// return false;
        }

        pXAPO->Release();
    }

    Debug::Log("SourceVoice作成: " + Utility::GetFilenameFromFullpath(m_soundData.m_filepath));
    return true;
}

//-----------------------------------------------------------------------------
// オーディオバッファを追加
//-----------------------------------------------------------------------------
bool Sound::SubmitBuffer(bool loop, UINT32 playBegin)
{
    if (m_pSourceVoice == nullptr) return false;

    m_soundData.m_buffer.pAudioData = m_soundData.m_mediaData.data();
    m_soundData.m_buffer.Flags      = XAUDIO2_END_OF_STREAM;
    m_soundData.m_buffer.AudioBytes = sizeof(BYTE) * static_cast<UINT32>(m_soundData.m_mediaData.size());
    m_soundData.m_buffer.LoopCount  = loop ? XAUDIO2_LOOP_INFINITE : 0;
    m_soundData.m_buffer.PlayBegin  = playBegin;

    if (FAILED(m_pSourceVoice->SubmitSourceBuffer(&m_soundData.m_buffer)))
    {
        Debug::LogError("SubmitSourceBuffer失敗."); return false;
    }
    return true;
}



//=============================================================================
// 
// SounData
// 
//=============================================================================

//-----------------------------------------------------------------------------
// 作成
//-----------------------------------------------------------------------------
bool SoundData::Create(const std::string& filepath, bool loop, bool useFilter)
{
    //ユーザーデータ取得
    m_filepath = filepath;
    m_loop = loop;
    m_useFilter = useFilter;

    //ソースリーダー作成
    IMFSourceReader* pMFSourceReader = nullptr;
    if (FAILED(MFCreateSourceReaderFromURL(sjis_to_wide(filepath).c_str(), nullptr, &pMFSourceReader)))
    {
        Debug::LogError("MFCreateSourceReaderFromURL失敗."); return false;
    }

    //メディアタイプ初期設定(PCMで読み込むため)
    IMFMediaType* pMFMediaType = nullptr;
    if (FAILED(MFCreateMediaType(&pMFMediaType)))
    {
        pMFSourceReader->Release();
        Debug::LogError("MFCreateMediaType失敗."); return false;
    }
    pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    if (FAILED(pMFSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMFMediaType)))
    {
        pMFSourceReader->Release();
        Debug::LogError("SetCurrentMediaType失敗."); return false;
    }

    pMFMediaType->Release();
    pMFMediaType = nullptr;

    //メディアタイプの取得
    if (FAILED(pMFSourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMFMediaType)))
    {
        pMFSourceReader->Release();
        Debug::LogError("GetCurrentMediaType失敗."); return false;
    }

    //TOOD: 一部を除き設定する必要がある？
    UINT32 sampleRate;
    pMFMediaType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &sampleRate);
    pMFMediaType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, sampleRate);
    //Debug::Log("sampleRate: " + ToString(sampleRate));

    UINT32 byteParSec;
    pMFMediaType->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &byteParSec);
    pMFMediaType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, byteParSec);
    //Debug::Log("byteParSec: " + ToString(byteParSec));

    UINT32 bitRate;
    pMFMediaType->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &bitRate);
    pMFMediaType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, bitRate);
    //Debug::Log("bitRate: " + ToString(bitRate));

    UINT32 channels;
    pMFMediaType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &channels);
    pMFMediaType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, channels);
    //Debug::Log("channels: " + ToString(channels));

    //メディアタイプ設定完了
    if(FAILED(pMFSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMFMediaType)))
    {
        pMFSourceReader->Release();
        Debug::LogError("SetCurrentMediaType失敗."); return false;
    }

    //設定したメディアタイプをもとにオーディオデータ形式の作成
    UINT32 formatSize = 0;
    if (FAILED(MFCreateWaveFormatExFromMFMediaType(pMFMediaType, &m_pWaveFormat, &formatSize)))
    {
        pMFSourceReader->Release();
        Debug::LogError("MFCreateWaveFormatExFromMFMediaType失敗."); return false;
    }
    pMFMediaType->Release();
    pMFMediaType = nullptr;

    //読み込み
    while (true)
    {
        IMFSample* pMFSample = nullptr;
        DWORD dwStreamFlags = 0;
        if (FAILED(pMFSourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &pMFSample)))
        {
            pMFSourceReader->Release();
            Debug::LogError("ReadSample失敗."); return false;
        }

        //ストリームの終端 ->読み込み完了
        if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
            break;

        IMFMediaBuffer* pMFMediaBuffer = nullptr;
        if(FAILED(pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer)))
        {
            pMFSourceReader->Release();
            Debug::LogError("ConvertToContiguousBuffer失敗."); return false;
        }

        {
            //書き込み
            BYTE* pBuffer = nullptr;
            DWORD cbCurrentLength = 0;
            if (FAILED(pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength)))
            {
                pMFMediaBuffer->Release();
                pMFSourceReader->Release();
                Debug::LogError("Lock失敗."); return false;
            }

            m_mediaData.resize(m_mediaData.size() + cbCurrentLength);
            memcpy(m_mediaData.data() + m_mediaData.size() - cbCurrentLength, pBuffer, cbCurrentLength);
        }

        pMFMediaBuffer->Unlock();
        pMFMediaBuffer->Release();
        pMFSample->Release();
    }
    pMFSourceReader->Release();

    return true;
}

//-----------------------------------------------------------------------------
// 解放
//-----------------------------------------------------------------------------
void SoundData::Release()
{
    if (m_pWaveFormat)
    {
        CoTaskMemFree(m_pWaveFormat);
        m_pWaveFormat = nullptr;
    }

    m_mediaData.clear();
}
