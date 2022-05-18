﻿//-----------------------------------------------------------------------------
// File: main.cpp
//
// アプリケーション側の実装
// 
// メモ:
//  SmpleMathでいい気がしてきた...
//  面倒なのでモデルはライブラリ使用して実装しよう
//-----------------------------------------------------------------------------
#include "../../Engine/Source/Pch.h"
#pragma comment(lib, "Engine.lib")

//アプリケーション必須デバイス
static Window* window = nullptr;
static GraphicsDevice* graphicsDevice = nullptr;
static AudioDevice* audio_device = nullptr;
static VideoDevice* video_device = nullptr;

static GamePad* game_pad = nullptr;

static constexpr int windowWidth = 1280;
static constexpr int windowHeight = 720;

std::shared_ptr<Sound> sp_sound = nullptr;
std::wstring sound_path = L"Assets/Sunshine.mp3";

void Initialize();
void Update();
void Draw();
void LateUpdate();
void Finalize();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	//不使用な引数をコンパイラに伝えてWarningを抑制
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#if _DEBUG
	//メモリリークの検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//Windows10 CreatorsUpdateで入ったモニタ単位でDPIが違う環境への対応
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	//COM初期化
	Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);

	Initialize();

	//メインループ
	while (true)
	{
		if (!window->ProcessMessage())
			break;

		Update();
		Draw();
		LateUpdate();
	}

	Finalize();

	CoUninitialize();

	return 0;
}

void Initialize()
{
	//ウィンドウ作成
	WINDOE_CREATE_PARAM window_param;
	window_param.TitleName		= L"Project Cyclops";
	window_param.ClassName		= L"ClassName";
	window_param.ClientWidth	= windowWidth;
	window_param.ClientHeight	= windowHeight;

	window = new Window(window_param);
	window->Initialize();

	Input::SetWindowHwnd(window->GetHwnd());

	//グラフィックスデバイス作成
	GRAPHICS_DEVICE_CREATE_PARAM device_param;
	device_param.BufferCount	= 2;
	device_param.Width			= windowWidth;
	device_param.Height			= windowHeight;
	device_param.RefreshRate	= 0;
	device_param.Windowed		= true;
	device_param.UseHDR			= false;
	device_param.UseMSAA		= true;
	device_param.DebugMode		= true;
	device_param.Hwnd			= window->GetHwnd();

	graphicsDevice = new GraphicsDevice(device_param);
	//デバイスの初期化でデバイスの参照が必要な子Object(Texture)を使用するため
	//即座にセットしなければならない なんか設計が間違っている気が...
	GraphicsDeviceChild::SetDevice(graphicsDevice);
	graphicsDevice->Initialize();

	//ビデオデバイス作成
	video_device = new VideoDevice();

	//オーディオデバイス作成
	audio_device = new AudioDevice();
	AudioDeviceChild::SetDevice(audio_device);
	audio_device->Initialize();
	audio_device->SetMasterVolume(0.0f);

	game_pad = new GamePad();

	//サウンドテスト ※一連の動作をラップした方がいい コンポーネント化とか
	std::thread([&] {
		sp_sound = std::make_shared<Sound>(sound_path, true, true);
		if (sp_sound)
		{
			sp_sound->Play();
			audio_device->AddSound(sp_sound);
		}}
	).detach();
}

void Update()
{
	game_pad->Update();

	auto now_volume = audio_device->GetMasterVolume();
	if (Input::IsKeyDown(KeyCode::UpArrow))		{ audio_device->SetMasterVolume(now_volume + 0.1f); Debug::Log("Volume: " + ToString(audio_device->GetMasterVolume())); }
	if (Input::IsKeyDown(KeyCode::DownArrow))	{ audio_device->SetMasterVolume(now_volume - 0.1f); Debug::Log("Volume: " + ToString(audio_device->GetMasterVolume())); }

	if (Input::IsKeyDown(KeyCode::LeftArrow))	{ sp_sound->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 0.2f); Debug::Log("フィルタ: LowPass"); }
	if (Input::IsKeyDown(KeyCode::RightArrow))	{ sp_sound->SetFilter(XAUDIO2_FILTER_TYPE::HighPassFilter, 0.3f); Debug::Log("フィルタ: HighPass"); }
	if (Input::IsKeyDown(KeyCode::Space))		{ sp_sound->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 1.0f, 1.0f); Debug::Log("フィルタ: リセット"); }

	const Vector2& left = game_pad->Left();
	//Debug::Log("left: " + ToStringV(left));

	const auto& mpos = Input::GetMousePos();
	const auto& mwheeldelta = Input::GetMouseWheelDelta();
	//Debug::Log("Nouse pos: " + ToStringV(mpos));
	//Debug::Log("mwheeldelta: " + ToString(mwheeldelta));

	if (Input::IsMouseDown(MouseButton::Left)) Debug::Log("Left Down.");
	if (Input::IsMouseDown(MouseButton::Middle)) Debug::Log("Middle Down.");
	if (Input::IsMouseDown(MouseButton::Right)) Debug::Log("Right Down.");
	if (Input::IsMouseUp(MouseButton::Left)) Debug::Log("Left Up.");
	if (Input::IsMouseUp(MouseButton::Middle)) Debug::Log("Middle Up.");
	if (Input::IsMouseUp(MouseButton::Right)) Debug::Log("Right Up.");
	//if (Input::IsMousePressed(MouseButton::Left)) Debug::Log("Left Pressed.");
	//if (Input::IsMousePressed(MouseButton::Middle)) Debug::Log("Middle Pressed.");
	//if (Input::IsMousePressed(MouseButton::Right)) Debug::Log("Right Pressed.");

	audio_device->Update(Matrix());
}

void Draw()
{
	graphicsDevice->Begin();

	graphicsDevice->End();
}

void LateUpdate()
{

}

void Finalize()
{
	audio_device->Finalize();
	graphicsDevice->Finalize();
	window->Finalize();

	delete game_pad;
	delete audio_device;
	delete video_device;
	delete graphicsDevice;
	delete window;

	Debug::Log("終了.");
}