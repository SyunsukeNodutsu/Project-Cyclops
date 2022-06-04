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
		m_spCamera->SetCameraMatrix(Matrix::CreateTranslation(Vector3(0, 0, -1)));
		m_spCamera->SetLocalPos(Vector3(0, 0, 0));
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
	m_spCamera->SetCameraMatrix(Matrix::CreateTranslation(0, 0, -10));
	Debug::AddDebugSphere(Vector3::Zero, 1.0f, Vector4(256, 0, 0, 256));
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
	spriteShader.DrawTexture(m_spTexture.get(), Vector2::Zero);
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GameDemo::OnLateUpdate()
{
}
