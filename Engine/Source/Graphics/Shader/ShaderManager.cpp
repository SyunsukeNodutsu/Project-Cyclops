﻿#include "ShaderManager.h"

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool ShaderManager::Initialize()
{
    if (!m_spriteShader.Initialize()) { Debug::Log("シェーダー(SpriteShader)初期化失敗."); return false; }

    return true;
}
