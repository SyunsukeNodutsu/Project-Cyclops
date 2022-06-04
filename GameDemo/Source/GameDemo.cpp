#include "GameDemo.h"

//-----------------------------------------------------------------------------
// サブシステム初期化直後
//-----------------------------------------------------------------------------
void GameDemo::OnStart()
{
	//カメラ
	m_spCamera = std::make_shared<FPSCamera>();
	if (m_spCamera)
	{
		m_spCamera->m_name = "Camera Main";
		m_spCamera->m_priority = 10.0f;
		m_spCamera->SetCameraMatrix(matrix4x4::CreateTranslation(float3(0, 0, -1)));
		m_spCamera->SetLocalPos(float3::Zero);
		m_pCameraManager->AddCameraList(m_spCamera);
	}

	m_profile.Start("Load texture.");
	m_spTexture = std::make_shared<Texture>();
	m_spTexture->Load(L"../Assets/100Alligator.jpg");
	m_profile.End();
}

//-----------------------------------------------------------------------------
// サブシステム終了直前
//-----------------------------------------------------------------------------
void GameDemo::OnEnd()
{
	Debug::Log("終了.");
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void GameDemo::OnUpdate()
{
	m_spCamera->SetCameraMatrix(matrix4x4::CreateTranslation(0, 0, -10));
}

//-----------------------------------------------------------------------------
// 3D描画
//-----------------------------------------------------------------------------
void GameDemo::OnDraw3D()
{
}

//-----------------------------------------------------------------------------
// 2D描画
//-----------------------------------------------------------------------------
void GameDemo::OnDraw2D(SpriteShader& spriteShader)
{
	spriteShader.DrawTexture(m_spTexture.get(), float2(100, -80), float2(0.2f, 0.2f));
	spriteShader.DrawTexture(m_spTexture.get(), float2(800, 400), float2(0.2f, 0.2f));
	spriteShader.DrawTexture(m_spTexture.get(), float2(-500, 200), float2(0.2f, 0.2f));

	spriteShader.DrawLine(float2(0, 400), float2(400, 400), color4::Green);
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GameDemo::OnLateUpdate()
{
}
