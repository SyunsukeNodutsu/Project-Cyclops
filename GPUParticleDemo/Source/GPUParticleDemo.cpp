﻿#include "GPUParticleDemo.h"

//-----------------------------------------------------------------------------
// サブシステム初期化直後
//-----------------------------------------------------------------------------
void GPUParticleDemo::OnStart()
{
	//カメラ
	m_spCamera = std::make_shared<FPSCamera>();
	if (m_spCamera)
	{
		m_spCamera->m_name = "Camera Main";
		m_spCamera->m_priority = 10.0f;
		m_spCamera->SetCameraMatrix(matrix4x4::CreateTranslation(float3(0, 0, -1)));
		m_spCamera->SetLocalPos(float3(0, 0, 0));
		m_pCameraManager->AddCameraList(m_spCamera);
	}

	//エミッターデータ
	static constexpr float pos = 10.0f, vel = 0.4f;
	m_emitData = {
		.m_maxPosition = float3( pos,  pos,  pos),
		.m_minPosition = float3(-pos, -pos, -pos),
		.m_maxVelocity = float3( vel,  vel,  vel),
		.m_minVelocity = float3(-vel, -vel, -vel),
		.m_maxLifeSpan = 20.0f,
		.m_minLifeSpan = 10.0f,
		.m_color = float4(0, 1, 0, 1),
	};

	//m_spParticleTexture = std::make_shared<Texture>();
	//m_spParticleTexture->Load(L"../Assets/Circle.png");

	m_pGraphicsDevice->m_spParticleSystem->Emit(m_emitData, 800, nullptr, true);
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
	m_spCamera->SetCameraMatrix(matrix4x4::CreateTranslation(0, 0, -10));
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
void GPUParticleDemo::OnDraw2D(SpriteShader& spriteShader)
{
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GPUParticleDemo::OnLateUpdate()
{
}
