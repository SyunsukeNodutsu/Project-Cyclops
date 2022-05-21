﻿#include "GameDemo.h"

Window*			GameDemo::m_pWindow			= nullptr;
GraphicsDevice* GameDemo::m_pGraphicsDevice = nullptr;
AudioDevice*	GameDemo::m_pAudioDevice	= nullptr;
VideoDevice*	GameDemo::m_pVideoDevice	= nullptr;
FpsTimer*		GameDemo::m_pFpsTimer		= nullptr;

//-----------------------------------------------------------------------------
// 実行大本
//-----------------------------------------------------------------------------
void GameDemo::Run()
{
	Initialize();

	while (true)
	{
		if (!m_pWindow->ProcessMessage()) break;

		Update();
		Draw();
		LateUpdate();
	}

	Finalize();
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void GameDemo::Initialize()
{
	std::chrono::system_clock::time_point start, end;
	start = std::chrono::system_clock::now();

	//サブシステムを適切な順番で生成 -------------------------------->
	WINDOE_CREATE_PARAM window_param = {
		.TitleName		= L"Project Cyclops",
		.ClassName		= L"ClassName",
		.ClientWidth	= m_windowWidth,
		.ClientHeight	= m_windowHeight,
	};
	m_pWindow = new Window(window_param);
	m_pWindow->Initialize();
	
	GRAPHICS_DEVICE_CREATE_PARAM device_param = {
		.BufferCount = 2,
		.Width = m_windowWidth,
		.Height = m_windowHeight,
		.RefreshRate = 0,
		.Windowed = true,
		.UseHDR = false,
		.UseMSAA = false,
		.DebugMode = true,
		.Hwnd = m_pWindow->GetHwnd(),
	};
	m_pGraphicsDevice = new GraphicsDevice(device_param);

	m_pVideoDevice = new VideoDevice();
	m_pAudioDevice = new AudioDevice();
	m_pFpsTimer	= new FpsTimer();

	GraphicsDeviceChild::SetDevice(m_pGraphicsDevice);
	AudioDeviceChild::SetDevice(m_pAudioDevice);
	Input::SetWindowHwnd(m_pWindow->GetHwnd());

	m_pGraphicsDevice->Initialize();
	m_pAudioDevice->Initialize();
	//<-------------------------サブシステムを適切な順番で生成ここまで

	end = std::chrono::system_clock::now();//計測終了時間
	auto time = end - start;//処理に要した時間
	
	auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
	Debug::Log("サブシステムの初期化に要した時間(ミリ秒): " + ToString(msec));
	//2022/05/21: 250～270ミリ秒

	m_pAudioDevice->SetMasterVolume(0.6f);

	std::thread([&] {
		m_spSound = std::make_shared<Sound>(m_soundPath, true, true);
		if (m_spSound)
		{
			m_spSound->Play();
			m_pAudioDevice->AddSound(m_spSound);
		}}
	).detach();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void GameDemo::Update()
{
	GamePad::Update();
	m_pFpsTimer->Tick();

	auto now_volume = m_pAudioDevice->GetMasterVolume();
	if (Input::IsKeyDown(KeyCode::UpArrow))		{ m_pAudioDevice->SetMasterVolume(now_volume + 0.1f); Debug::Log("Volume: " + ToString(m_pAudioDevice->GetMasterVolume())); }
	if (Input::IsKeyDown(KeyCode::DownArrow))	{ m_pAudioDevice->SetMasterVolume(now_volume - 0.1f); Debug::Log("Volume: " + ToString(m_pAudioDevice->GetMasterVolume())); }

	if (Input::IsKeyDown(KeyCode::LeftArrow))	{ m_spSound->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 0.2f);		Debug::Log("フィルタ: LowPass"); }
	if (Input::IsKeyDown(KeyCode::RightArrow))	{ m_spSound->SetFilter(XAUDIO2_FILTER_TYPE::HighPassFilter, 0.3f);		Debug::Log("フィルタ: HighPass"); }
	if (Input::IsKeyDown(KeyCode::Space))		{ m_spSound->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 1.0f, 1.0f);	Debug::Log("フィルタ: リセット"); }

	if (Input::IsKeyDown(KeyCode::R)) m_pGraphicsDevice->ToggleScreen(true);

	m_pAudioDevice->Update(Matrix());
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void GameDemo::Draw()
{
	m_pGraphicsDevice->Begin();

	m_pGraphicsDevice->End();
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GameDemo::LateUpdate()
{
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void GameDemo::Finalize()
{
	m_pAudioDevice->Finalize();
	m_pGraphicsDevice->Finalize();
	m_pWindow->Finalize();

	delete m_pFpsTimer;
	delete m_pAudioDevice;
	delete m_pVideoDevice;
	delete m_pGraphicsDevice;
	delete m_pWindow;

	Debug::Log("終了.");
}
