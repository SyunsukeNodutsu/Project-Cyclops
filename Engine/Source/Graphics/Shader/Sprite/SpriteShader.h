﻿//-----------------------------------------------------------------------------
// File: SpriteShader.h
//
// 画像描画シェーダー
//-----------------------------------------------------------------------------
#pragma once
#include "../../GraphicsDeviceChild.h"

//画像描画シェーダークラス
class SpriteShader : public GraphicsDeviceChild
{
	struct Vertex //TODO: 使用する頂点はどこかで数パターン用意しておく
	{
		Vector3 m_position;
		Vector2 m_uv;
	};

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

	void DrawTexture(const Texture* pTexture, Vector2 pos, Vector2 pivot = Vector2(0.5f));

private:

	ComPtr<ID3D11VertexShader>	m_cpVS;
	ComPtr<ID3D11PixelShader>	m_cpPS;
	ComPtr<ID3D11InputLayout>	m_cpInputLayout;

	std::array<Vertex, 4>		m_vertices;
	ConstantBuffer<CBSprite>	m_cb0Sprite;
	Matrix						m_prevProjMatrix;
	bool						m_begin;

	void SetVertex(const Texture* pTexture, Vector2 pos, Vector2 pivot = Vector2(0.5f));

};
