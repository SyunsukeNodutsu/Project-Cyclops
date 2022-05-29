#include "GPUParticleDemo.h"

//-----------------------------------------------------------------------------
// サブシステム初期化直後
//-----------------------------------------------------------------------------
void GPUParticleDemo::OnStart()
{
	//初期カメラ
	m_editorCamera.m_name = "Camera Main";
	m_editorCamera.m_priority = 10.0f;
	m_editorCamera.SetCameraMatrix(Matrix::CreateTranslation(Vector3(0, 0, -3)));

	//エミッターデータ
	m_emitData.m_minPosition = Vector3::Zero;
	m_emitData.m_maxPosition = Vector3::Zero;

	const float vel = 8.0f;
	m_emitData.m_minVelocity = Vector3(-vel, -vel, -vel);
	m_emitData.m_maxVelocity = Vector3(vel, vel, vel);

	m_emitData.m_minLifeSpan = 1.0f;
	m_emitData.m_maxLifeSpan = 6.0f;
	m_emitData.m_color = Vector4(1, 0, 0.4f, 1);

	m_spTexture = std::make_shared<Texture>();
	m_spTexture->Load(L"../Assets/test.png");

	m_pGraphicsDevice->m_spParticleSystem->Emit(m_emitData, 10000, m_spTexture, true);
}

//-----------------------------------------------------------------------------
// サブシステム終了直前
//-----------------------------------------------------------------------------
void GPUParticleDemo::OnEnd()
{
	Debug::Log("終了.");
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void GPUParticleDemo::OnUpdate()
{
	m_editorCamera.Update();

	static float count = 0;
	count += FpsTimer::GetDeltaTime<float>();
	if (count >= 1.0f)
	{
		m_pGraphicsDevice->m_spParticleSystem->Emit(m_emitData, 10000, m_spTexture, true);
		count = 0.0f;
	}
}

//-----------------------------------------------------------------------------
// 3D描画
//-----------------------------------------------------------------------------
void GPUParticleDemo::OnDraw3D()
{
	m_editorCamera.SetToShader();
}

//-----------------------------------------------------------------------------
// 2D描画
//-----------------------------------------------------------------------------
void GPUParticleDemo::OnDraw2D()
{
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GPUParticleDemo::OnLateUpdate()
{
}
