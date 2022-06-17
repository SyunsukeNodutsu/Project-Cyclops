﻿#pragma once

//--------------------------------------------------
// Row API
//--------------------------------------------------

//Direct3D11
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d3dcommon.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

//X2Audio X3DAudio
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>
#pragma comment(lib,"xaudio2.lib")

//XInput
#include <Xinput.h>
#pragma comment(lib, "Xinput.lib")

//Media Foundation
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

//#include <evr.h>
//#pragma comment(lib, "strmiids.lib")

//DirectXMath
#include <DirectXMath.h>

//--------------------------------------------------
// Engine
//--------------------------------------------------

//サブシステム
#include "Subsystem.h"

//算術
#include "Math/Easing.h"

//ユーティリティ
#include "Utility/Utility.h"
#include "Utility/Timer.h"

//プロファイリング
#include "Profile/ImGuiProfile.h"
#include "Profile/Profile.h"

//ウィンドウ
#include "Window/Window.h"
#include "Window/ShellSystem.h"

//グラフィックス
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsDeviceChild.h"

#include "Graphics/Buffer.h"
#include "Graphics/Texture.h"
#include "Graphics/RendererStatus.h"

#include "Graphics/Camera/Camera.h"
#include "Graphics/Particle/ParticleSystem.h"
#include "Graphics/Shader/ShaderManager.h"

//ビデオ
#include "Video/VideoDevice.h"

//オーディオ
#include "Audio/AudioDevice.h"
#include "Audio/AudioDeviceChild.h"

#include "Audio/Sound.h"
#include "Audio/Sound3D.h"

//モデル

//物理
#include "Physics/Collision.h"

//入力
#include "Input/Input.h"
#include "Input/GamePad.h"

//編集用モジュール

//高レベル層
#include "HighLevel/Camera/EditorCamera.h"
#include "HighLevel/Camera/FPSCamera.h"
#include "HighLevel/Camera/TPSCamera.h"

#include "HighLevel/Component/Component.h"
#include "HighLevel/Component/Transform.h"

#include "HighLevel/Behaviour/Behaviour.h"

#include "HighLevel/Manager/CameraManager.h"

#include "HighLevel/GUI/Button2D.h"

#include "HighLevel/CyclopsRoot.h"
