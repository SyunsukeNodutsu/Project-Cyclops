//-----------------------------------------------------------------------------
// File: GameDemo.h
//
// エンジンを使用したAPPの実装デモ
//-----------------------------------------------------------------------------
#pragma once

//エンジン機能
#include "../../Engine/Source/Pch.h"
#pragma comment(lib, "Engine.lib")

//エンジンを使用したAPPのデモクラス
class GameDemo : public ApplicationBase
{
public:

	void OnStart() override;
	void OnEnd() override;

	void OnUpdate() override;
	void OnDraw3D() override;
	void OnDraw2D() override;
	void OnLateUpdate() override;

private:

	Profile m_profile;
	std::shared_ptr<FPSCamera> m_spFpsCamera = nullptr;
	std::shared_ptr<Texture> m_spTexture = nullptr;

};
