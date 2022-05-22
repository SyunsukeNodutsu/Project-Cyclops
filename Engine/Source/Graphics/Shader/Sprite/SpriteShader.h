//-----------------------------------------------------------------------------
// File: SpriteShader.h
//
// 画像描画シェーダー
//-----------------------------------------------------------------------------
#pragma once
#include "../../GraphicsDeviceChild.h"

//画像描画シェーダークラス
class SpriteShader : public GraphicsDeviceChild
{
	struct CBSprite
	{
		Vector4 m_color;
	};

public:

	SpriteShader();
	~SpriteShader() = default;

	bool Initialize();

	void Begin(bool linear = false, bool disableZBuffer = true);
	void End();

	void DrawTexture(const Texture* texture, Vector2Int pos);

private:

	ComPtr<ID3D11VertexShader>	m_cpVS;
	ComPtr<ID3D11PixelShader>	m_cpPS;
	ComPtr<ID3D11InputLayout>	m_cpInputLayout;

	ConstantBuffer<CBSprite>	m_cbSprite;
	Matrix						m_prevProjMatrix;
	bool						m_begin;

};
