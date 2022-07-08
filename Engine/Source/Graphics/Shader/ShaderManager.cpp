#include "ShaderManager.h"

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void ShaderManager::Initialize()
{
    if (!m_spriteShader.Initialize())       Debug::LogError("シェーダー(SpriteShader)初期化失敗.");
    if (!m_litShader.Initialize())          Debug::LogError("シェーダー(LitShader)初期化失敗.");
    if (!m_lensGhostShader.Initialize())    Debug::LogError("シェーダー(LensGhostShader)初期化失敗.");
    if (!m_grassShader.Initialize())        Debug::LogError("シェーダー(GrassShader)初期化失敗.");
    if (!m_postProcessShader.Initialize())  Debug::LogError("シェーダー(PostProcessShader)初期化失敗.");
    if (!m_unlitShader.Initialize())        Debug::LogError("シェーダー(UnlitShader)初期化失敗.");
    if (!m_GPUParticleShader.Initialize())  Debug::LogError("シェーダー(GPUParticleShader)初期化失敗.");
}
