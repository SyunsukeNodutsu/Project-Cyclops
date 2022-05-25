//-----------------------------------------------------------------------------
// File: ShaderManager.h
//
// シェーダー管理マネージャー
//-----------------------------------------------------------------------------
#pragma once
#include "GPUParticle/GPUParticle.h"
#include "Sprite/SpriteShader.h"

//シェーダー管理クラス
class ShaderManager
{
public:

	ShaderManager() = default;
	~ShaderManager() = default;

	bool Initialize();

	SpriteShader m_spriteShader;
	GPUParticle m_GPUParticleShader;

};
