//-----------------------------------------------------------------------------
// File: SpriteShader.h
//
// 画像描画シェーダー
//-----------------------------------------------------------------------------
#pragma once

//画像描画シェーダークラス
class SpriteShader : public GraphicsDeviceChild
{
	struct Vertex
	{
		float3 m_position;
		float2 m_uv;
	};

	struct cbColor
	{
		color4 m_color;
	};

public:

	SpriteShader();
	~SpriteShader() = default;

	bool Initialize();

	void Begin(bool linear = false, bool disableZBuffer = true);
	void End();

	void DrawTexture(const Texture* pTexture, float2 pos, float2 size = float2(1.0f), float2 pivot = float2(0.5f));
	
	void DrawLine(const float2 pos1, const float2 pos2, const color4 color);

private:

	ComPtr<ID3D11VertexShader>	m_cpVS;
	ComPtr<ID3D11PixelShader>	m_cpPS;
	ComPtr<ID3D11InputLayout>	m_cpInputLayout;
	ConstantBuffer<cbColor>		m_cbColor0;

	std::array<Vertex, 4>		m_vertices;
	matrix4x4					m_prevProjMatrix;
	bool						m_begin;

	void SetVertices(const Texture* pTexture, float2 pos, float2 pivot = float2(0.5f));

};
