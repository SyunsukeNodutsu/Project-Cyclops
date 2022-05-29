﻿//-----------------------------------------------------------------------------
// File: ShaderManager.h
//
// シェーダー管理マネージャー
//-----------------------------------------------------------------------------
#pragma once
#include "GPUParticle/GPUParticleShader.h"
#include "Lit/LitShader.h"
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
	PostProcessShader	m_postProcessShader;
	UnlitShader			m_unlitShader;
	GPUParticleShader	m_GPUParticleShader;

};
