#include "GameDemo.h"

//-----------------------------------------------------------------------------
// サブシステム初期化直後
//-----------------------------------------------------------------------------
void GameDemo::OnStart()
{
	//初期カメラ
	m_camera.m_name = "Camera Main";
	m_camera.m_priority = 10.0f;
	m_camera.SetCameraMatrix(Matrix::CreateTranslation(Vector3(0, 0, -3)));

	m_profile.Start("テクスチャ読み込み");
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
	static Vector3 pos = m_camera.GetCameraMatrix().GetTranslation();
	static constexpr float move_pow = 10.0f;

	if (Input::IsKeyPressed(KeyCode::LeftArrow))	pos.x -= move_pow * FpsTimer::GetDeltaTime<float>(true);
	if (Input::IsKeyPressed(KeyCode::RightArrow))	pos.x += move_pow * FpsTimer::GetDeltaTime<float>(true);
	if (Input::IsKeyPressed(KeyCode::UpArrow))		pos.z += move_pow * FpsTimer::GetDeltaTime<float>(true);
	if (Input::IsKeyPressed(KeyCode::DownArrow))	pos.z -= move_pow * FpsTimer::GetDeltaTime<float>(true);

	Matrix trans = Matrix::CreateTranslation(pos);
	m_camera.SetCameraMatrix(trans);
}

//-----------------------------------------------------------------------------
// 3D描画
//-----------------------------------------------------------------------------
void GameDemo::OnDraw3D()
{
	//TOOD: エンジン側で
	m_camera.SetToShader();
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
