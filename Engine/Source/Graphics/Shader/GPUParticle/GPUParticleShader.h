//-----------------------------------------------------------------------------
// File: GPUParticleShader.h
//
// 粒子シミュレーションシェーダー
// ジオメトリシェーダーで頂点をビルボード対応生成
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
	void End();

private:

	ComPtr<ID3D11VertexShader>	m_cpVS;
	ComPtr<ID3D11PixelShader>	m_cpPS;
	ComPtr<ID3D11GeometryShader>m_cpGS;
	ComPtr<ID3D11ComputeShader>	m_cpCS;
	ComPtr<ID3D11InputLayout>	m_cpInputLayout;

};
