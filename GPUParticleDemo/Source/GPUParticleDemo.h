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
class GPUParticleDemo : public ApplicationBase
{
public:

	void OnStart() override;
	void OnEnd() override;

	void OnUpdate() override;
	void OnDraw3D() override;
	void OnDraw2D(SpriteShader& spriteShader) override;
	void OnLateUpdate() override;

private:

	ParticleSystem::EmitData m_emitData;
	std::shared_ptr<Texture> m_spParticleTexture = nullptr;

};
