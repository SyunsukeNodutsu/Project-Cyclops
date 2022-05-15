#include "AudioDeviceChild.h"

AudioDevice* AudioDeviceChild::m_audioDevice = nullptr;

//-----------------------------------------------------------------------------
// オーディオデバイスの設定
//-----------------------------------------------------------------------------
void AudioDeviceChild::SetDevice(AudioDevice* device)
{
	if (device == nullptr) return;
	m_audioDevice = device;
}
