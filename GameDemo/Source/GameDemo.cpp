#include "GameDemo.h"

void GameDemo::OnStart()
{
	//初期カメラ
	m_camera.m_name = "Camera Main";
	m_camera.m_priority = 10.0f;
	m_camera.SetCameraMatrix(Matrix::CreateTranslation(Vector3(0, 0, -3)));

	m_profile.Start("テクスチャ読み込み");
	m_spTexture = std::make_shared<Texture>();
	m_spTexture->Load(L"Assets/test2.jpg");
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
	texture->Load(L"Assets/test.png");

	m_pGraphicsDevice->m_spParticleSystem->Emit(m_emitData, 100000, texture, true);
}

void GameDemo::OnEnd()
{
	Debug::Log("終了.");
}

void GameDemo::OnUpdate()
{
	static Vector3 pos = m_camera.GetCameraMatrix().GetTranslation();
	static constexpr float move_pow = 10.0f;

	if (Input::IsKeyPressed(KeyCode::LeftArrow))	pos.x -= move_pow * (float)FpsTimer::GetDeltaTime();
	if (Input::IsKeyPressed(KeyCode::RightArrow))	pos.x += move_pow * (float)FpsTimer::GetDeltaTime();
	if (Input::IsKeyPressed(KeyCode::UpArrow))		pos.z += move_pow * (float)FpsTimer::GetDeltaTime();
	if (Input::IsKeyPressed(KeyCode::DownArrow))	pos.z -= move_pow * (float)FpsTimer::GetDeltaTime();

	Matrix trans = Matrix::CreateTranslation(pos);
	m_camera.SetCameraMatrix(trans);
}

void GameDemo::OnDraw3D()
{
	m_camera.SetToShader();
}

void GameDemo::OnDraw2D()
{
	m_pGraphicsDevice->m_spShaderManager->m_spriteShader.DrawTexture(m_spTexture.get(), Vector2(0, 0));
}

void GameDemo::OnLateUpdate()
{

}
