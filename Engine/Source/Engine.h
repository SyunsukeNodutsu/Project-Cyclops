#pragma once

//--------------------------------------------------
// Row API
//--------------------------------------------------

//Direct3D11
#include <dxgi1_6.h>
#include <d3d11_4.h>
#include <d3dcommon.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

//X2Audio X3DAudio
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>
#pragma comment(lib,"xaudio2.lib")

//Media Foundation
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

//XInput
#include <Xinput.h>
#pragma comment(lib, "Xinput.lib")

//DirectXMath
#include <DirectXMath.h>

//--------------------------------------------------
// Engine
//--------------------------------------------------

//算術
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"

//デバッグ
#include "Debug/Debug.h"

//ユーティリティ
#include "Utility/Timer.h"

//ウィンドウ
#include "Window/Window.h"

//グラフィックス
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsDeviceChild.h"

#include "Graphics/Texture.h"

//ビデオ
#include "Video/VideoDevice.h"

//オーディオ
#include "Audio/AudioDevice.h"
#include "Audio/AudioDeviceChild.h"

#include "Audio/Sound.h"
#include "Audio/Sound3D.h"

//モデル


//入力
#include "Input/Input.h"

//高レベル層
#include "HighLevel/Behaviour/Behaviour.h"
#include "HighLevel/Component/Component.h"
