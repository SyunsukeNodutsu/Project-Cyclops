﻿//-----------------------------------------------------------------------------
// File: GPUParticleShader.h
//
// 粒子シミュレーションシェーダー
//-----------------------------------------------------------------------------
#pragma once

//GPU粒子シェーダークラス
class GPUParticleShader : public GraphicsDeviceChild
{
	struct Vertex
	{
		Vector3 m_position;
		Vector2 m_uv;
	};

public:

	GPUParticleShader();
	~GPUParticleShader() = default;

	bool Initialize();

	void Begin();

//private:

	ComPtr<ID3D11VertexShader>	m_cpVS;
	ComPtr<ID3D11PixelShader>	m_cpPS;
	ComPtr<ID3D11ComputeShader>	m_cpCS;
	ComPtr<ID3D11InputLayout>	m_cpInputLayout;

	std::shared_ptr<Buffer>		m_spVertexBuffer;

};