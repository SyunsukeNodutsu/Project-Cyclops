#include "GameDemo.h"

//-----------------------------------------------------------------------------
// サブシステム初期化直後
//-----------------------------------------------------------------------------
void GameDemo::OnStart()
{
	//初期カメラ
	m_spFpsCamera = std::make_shared<FPSCamera>();
	if (m_spFpsCamera)
	{
		m_spFpsCamera->m_name = "Camera Main";
		m_spFpsCamera->m_priority = 10.0f;
		m_spFpsCamera->SetCameraMatrix(Matrix::CreateTranslation(Vector3(0, 0, -10)));
		m_spFpsCamera->SetLocalPos(Vector3(0, 0, 0));
		m_pCameraManager->AddCameraList(m_spFpsCamera);
	}

	m_profile.Start("Load texture.");
	m_spTexture = std::make_shared<Texture>();
	m_spTexture->Load(L"../Assets/wani.jpg");
	m_profile.End();

	//エミッターデータ
	ParticleSystem::EmitData m_emitData;
	m_emitData.m_minPosition = Vector3::Zero;
	m_emitData.m_maxPosition = Vector3::Zero;

	const float vel = 2.0f;
	m_emitData.m_minVelocity = Vector3(-vel, -vel, -vel);
	m_emitData.m_maxVelocity = Vector3(vel, vel, vel);

	m_emitData.m_minLifeSpan = 1.0f;
	m_emitData.m_maxLifeSpan = 6.0f;
	m_emitData.m_color = Vector4(1, 0.5f, 0, 1);

	const auto& texture = std::make_shared<Texture>();
	texture->Load(L"../Assets/test.png");

	m_pGraphicsDevice->m_spParticleSystem->Emit(m_emitData, 100000, texture, true);
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
	m_spFpsCamera->Update();
	m_spFpsCamera->SetCameraMatrix(Matrix::CreateTranslation(0, 0, -10));
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
void GameDemo::OnDraw2D()
{
	//m_pGraphicsDevice->m_spShaderManager->m_spriteShader.DrawTexture(m_spTexture.get(), Vector2::Zero);
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GameDemo::OnLateUpdate()
{

}
