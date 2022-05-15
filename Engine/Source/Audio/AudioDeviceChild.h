//-----------------------------------------------------------------------------
// File: AudioDeviceChild.h
//
// エンジンのオーディオ機能へのアクセス権限を付与
//-----------------------------------------------------------------------------
#pragma once
#include "AudioDevice.h"

//オーディオスデバイスの子クラスに継承
class AudioDeviceChild
{
public:

    //@brief オーディオデバイスの設定
    //@param device 設定するデバイスソース
    //@note アプリケーション実行時に一回呼び出す
    static void SetDevice(AudioDevice* device);

protected:

	static AudioDevice* m_audioDevice;

};
