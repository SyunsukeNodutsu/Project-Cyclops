//-----------------------------------------------------------------------------
// File: LensGhostDemo.h
//
// エンジンを使用したAPPの実装デモ
//-----------------------------------------------------------------------------
#pragma once

//エンジン機能
#include "../../Engine/Source/Pch.h"
#pragma comment(lib, "Engine.lib")

//エンジンを使用したAPPのデモクラス
class LensGhostDemo : public ApplicationBase
{
public:

	void OnStart() override;
	void OnEnd() override;

	void OnUpdate() override;
	void OnDraw3D() override;
	void OnDraw2D(SpriteShader& spriteShader) override;
	void OnLateUpdate() override;

private:

	std::shared_ptr<FPSCamera> m_spCamera = nullptr;

};
