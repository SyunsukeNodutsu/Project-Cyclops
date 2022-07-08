//-----------------------------------------------------------------------------
// File: ShaderManager.h
//
// シェーダー管理マネージャー
//-----------------------------------------------------------------------------
#pragma once
#include "GPUParticle/GPUParticleShader.h"
#include "Lit/LitShader.h"
#include "LensGhost/LensGhostShader.h"
#include "Grass/GrassShader.h"
#include "PostProcess/PostProcessShader.h"
#include "Unlit/UnlitShader.h"
#include "Sprite/SpriteShader.h"

//シェーダー管理クラス
class ShaderManager
{
public:

	ShaderManager() = default;
	~ShaderManager() = default;

	void Initialize();

	SpriteShader		m_spriteShader;
	LitShader			m_litShader;
	LensGhostShader		m_lensGhostShader;
	GrassShader			m_grassShader;
	PostProcessShader	m_postProcessShader;
	UnlitShader			m_unlitShader;
	GPUParticleShader	m_GPUParticleShader;

};
