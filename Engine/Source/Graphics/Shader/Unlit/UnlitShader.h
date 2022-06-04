//-----------------------------------------------------------------------------
// File: UnlitShader.h
//
// ライトの影響を受けないシェーダー
//-----------------------------------------------------------------------------
#pragma once

//ライトの影響を受けないシェーダー
class UnlitShader : public GraphicsDeviceChild
{
public:

	struct Vertex
	{
		Vector3 m_position;
		Vector2 m_uv;
		Vector4 m_color;
	};

	UnlitShader();
	~UnlitShader() = default;

	bool Initialize();
	void Begin();

	void DrawVertices(const std::vector<Vertex>& vertices, D3D_PRIMITIVE_TOPOLOGY topology);

private:

	ComPtr<ID3D11VertexShader>	m_cpVS;
	ComPtr<ID3D11PixelShader>	m_cpPS;
	ComPtr<ID3D11InputLayout>	m_cpInputLayout;

};
