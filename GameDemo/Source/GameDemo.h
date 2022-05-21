//-----------------------------------------------------------------------------
// File: GameDemo.h
//
// エンジンを使用したAPPの実装デモ
//-----------------------------------------------------------------------------
#pragma once

//エンジン機能
#include "../../Engine/Source/Pch.h"
#pragma comment(lib, "Engine.lib")

//エンジンを使用したAPPのデモクラス
class GameDemo
{
public:

	GameDemo() = default;
	~GameDemo() = default;

	void Run();

private:

	static Window*			m_pWindow;
	static GraphicsDevice*	m_pGraphicsDevice;
	static AudioDevice*		m_pAudioDevice;
	static VideoDevice*		m_pVideoDevice;
	static FpsTimer*		m_pFpsTimer;

	int						m_windowWidth = 1280;
	int						m_windowHeight = 720;
	std::shared_ptr<Sound>	m_spSound = nullptr;
	std::wstring			m_soundPath = L"Assets/Sunshine.mp3";

private:

	void Initialize();
	void Update();
	void Draw();
	void LateUpdate();
	void Finalize();

};
