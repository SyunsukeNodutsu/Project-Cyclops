//-----------------------------------------------------------------------------
// File: GrassShader.h
//
// 草描画シェーダー
//-----------------------------------------------------------------------------
#pragma once

//草シェーダー
class GrassShader : public GraphicsDeviceChild
{
	struct Vertex
	{
		float3 m_position;
	};

public:

	GrassShader();
	~GrassShader() = default;

	bool Initialize();

	void Begin();
	void End();

private:

	ComPtr<ID3D11VertexShader>	m_cpVS;
	ComPtr<ID3D11PixelShader>	m_cpPS;
	ComPtr<ID3D11DomainShader>	m_cpDS;
	ComPtr<ID3D11HullShader>	m_cpHS;
	ComPtr<ID3D11InputLayout>	m_cpInputLayout;

	std::array<Vertex, 4>		m_vertices;

};
