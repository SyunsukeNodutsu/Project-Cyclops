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

	//エミッターデータ
	static constexpr float pos = 10.0f, vel = 0.4f;
	m_emitData = {
		.m_maxPosition = Vector3(pos,  pos,  pos),
		.m_minPosition = Vector3(-pos, -pos, -pos),
		.m_maxVelocity = Vector3(vel,  vel,  vel),
		.m_minVelocity = Vector3(-vel, -vel, -vel),
		.m_maxLifeSpan = 20.0f,
		.m_minLifeSpan = 10.0f,
		.m_color = Vector4(0, 1, 0, 1),
	};

	m_spParticleTexture = std::make_shared<Texture>();
	m_spParticleTexture->Load(L"../Assets/Circle.png");

	m_pGraphicsDevice->m_spParticleSystem->Emit(m_emitData, 100000, m_spParticleTexture, true);
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
	//spriteShader.DrawTexture(m_spTexture.get(), Vector2::Zero);
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GameDemo::OnLateUpdate()
{
}
