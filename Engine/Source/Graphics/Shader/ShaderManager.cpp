#include "ShaderManager.h"

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool ShaderManager::Initialize()
{
    if (!m_GPUParticleShader.Initialize()) { Debug::Log("シェーダー(GPUParticleShader)初期化失敗."); return false; }
    if (!m_spriteShader.Initialize()) { Debug::Log("シェーダー(SpriteShader)初期化失敗."); return false; }

    return true;
}
