﻿//-----------------------------------------------------------------------------
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

	GameDemo();
	~GameDemo() = default;

	void Run();

private:

	static Window*			m_pWindow;
	static FpsTimer*		m_pFpsTimer;

	static GraphicsDevice*	m_pGraphicsDevice;
	static AudioDevice*		m_pAudioDevice;
	static VideoDevice*		m_pVideoDevice;

	int						m_windowWidth;
	int						m_windowHeight;
	std::shared_ptr<Sound>	m_spSound;
	std::wstring			m_soundPath;

	std::shared_ptr<Texture>m_spTexture;

private:

	void Initialize();
	void Update();
	void Draw();
	void LateUpdate();
	void Finalize();

};
