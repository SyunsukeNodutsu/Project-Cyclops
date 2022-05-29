#include "GPUParticleDemo.h"

//-----------------------------------------------------------------------------
// サブシステム初期化直後
//-----------------------------------------------------------------------------
void GPUParticleDemo::OnStart()
{
	//エミッターデータ
	const float pos = 10.0f;
	m_emitData.m_minPosition = Vector3(-pos, -pos, -pos);
	m_emitData.m_maxPosition = Vector3(pos, pos, pos);

	const float vel = 0.4f;
	m_emitData.m_minVelocity = Vector3(-vel, -vel, -vel);
	m_emitData.m_maxVelocity = Vector3(vel, vel, vel);

	m_emitData.m_minLifeSpan = 10.0f;
	m_emitData.m_maxLifeSpan = 20.0f;
	m_emitData.m_color = Vector4(0, 1, 0, 1);

	m_spParticleTexture = std::make_shared<Texture>();
	m_spParticleTexture->Load(L"../Assets/Circle.png");

	m_pGraphicsDevice->m_spParticleSystem->Emit(m_emitData, 100000, m_spParticleTexture, true);
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
}

//-----------------------------------------------------------------------------
// 3D描画
//-----------------------------------------------------------------------------
void GPUParticleDemo::OnDraw3D()
{
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
