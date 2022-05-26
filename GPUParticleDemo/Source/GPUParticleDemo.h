//-----------------------------------------------------------------------------
// File: GPUParticleDemo.h
//
// エンジンを使用したAPPの実装デモ
//-----------------------------------------------------------------------------
#pragma once

//エンジン機能
#include "../../Engine/Source/Pch.h"
#pragma comment(lib, "Engine.lib")

//エンジンを使用したAPPのデモクラス
class GPUParticleDemo
{
public:

	GPUParticleDemo();
	~GPUParticleDemo() = default;

	void Run();

private:

	static Window*			m_pWindow;
	static FpsTimer*		m_pFpsTimer;

	static GraphicsDevice*	m_pGraphicsDevice;
	static AudioDevice*		m_pAudioDevice;
	static ImGuiProfile*	m_pImGuiProfile;

	int						m_windowWidth;
	int						m_windowHeight;

	Profile					m_profile;
	Camera					m_camera;

	ParticleSystem::EmitData m_emitData;

private:

	void Initialize();
	void Update();
	void Draw();
	void LateUpdate();
	void Finalize();

};
