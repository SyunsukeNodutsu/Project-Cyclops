#include "EnginePch.h"
#pragma comment(lib, "Engine.lib")

//アプリケーション必須デバイス
static Window*			window = nullptr;
static GraphicsDevice*	graphicsDevice = nullptr;
static AudioDevice*		audio_device = nullptr;
static VideoDevice*		video_device = nullptr;

static constexpr int windowWidth = 896;
static constexpr int windowHeight = 672;

std::shared_ptr<Sound> sp_sound = nullptr;
std::wstring sound_path = L"../Assets/Sunshine.mp3";

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
	GraphicsDeviceChild::SetDevice(graphicsDevice);
	//graphicsDevice->Initialize();

	//ビデオデバイス作成
	video_device = new VideoDevice();

	//オーディオデバイス作成
	audio_device = new AudioDevice();
	AudioDeviceChild::SetDevice(audio_device);
	audio_device->Initialize();
	audio_device->SetMasterVolume(0.2f);

	//サウンドテスト ※一連の動作をラップした方がいい コンポーネント化とか
	sp_sound = std::make_shared<Sound>();
	if (sp_sound)
	{
		sp_sound->Load(sound_path, true, true);
		sp_sound->Play();
		//sp_sound->SetPitch(2.0f);

		//サウンドリストに追加
		audio_device->AddSound(sp_sound);
	}
}

void Update()
{
	auto now_volume = audio_device->GetMasterVolume();
	if (Input::IsKeyDown(KeyCode::UpArrow))		{ audio_device->SetMasterVolume(now_volume + 0.1f); Debug::Log("Volume: " + ToString(audio_device->GetMasterVolume())); }
	if (Input::IsKeyDown(KeyCode::DownArrow))	{ audio_device->SetMasterVolume(now_volume - 0.1f); Debug::Log("Volume: " + ToString(audio_device->GetMasterVolume())); }

	if (Input::IsKeyDown(KeyCode::LeftArrow))	{ sp_sound->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 0.2f);		Debug::Log("フィルタ: LowPass"); }
	if (Input::IsKeyDown(KeyCode::RightArrow))	{ sp_sound->SetFilter(XAUDIO2_FILTER_TYPE::HighPassFilter, 0.3f);		Debug::Log("フィルタ: HighPass"); }
	if (Input::IsKeyDown(KeyCode::Space))		{ sp_sound->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 1.0f, 1.0f);	Debug::Log("フィルタ: リセット"); }

	if (Input::IsKeyDown(KeyCode::Keyboard1)) { sp_sound->SetPan(-1.0f); Debug::Log("パン: 左"); }
	if (Input::IsKeyDown(KeyCode::Keyboard2)) { sp_sound->SetPan( 1.0f); Debug::Log("パン: 右"); }
	if (Input::IsKeyDown(KeyCode::Keyboard3)) { sp_sound->SetPan( 0.0f); Debug::Log("パン: 中央"); }

	audio_device->Update(Matrix());
}

void Draw()
{
	//graphicsDevice->Begin();

	//graphicsDevice->End();
}

void LateUpdate()
{

}

void Finalize()
{
	audio_device->Finalize();
	//graphicsDevice->Finalize();
	window->Finalize();

	delete audio_device;
	delete video_device;
	delete graphicsDevice;
	delete window;

	Debug::Log("終了.");
}
