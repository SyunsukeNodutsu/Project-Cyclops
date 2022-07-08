//-----------------------------------------------------------------------------
// File: LensGhostShader.h
//
// レンズゴースト描画シェーダー
//-----------------------------------------------------------------------------
#pragma once

//レンズゴースト描画シェーダークラス
class LensGhostShader : public GraphicsDeviceChild
{
public:

	LensGhostShader();
	~LensGhostShader() = default;

	bool Initialize();
	void Begin();

private:

	ComPtr<ID3D11VertexShader>	m_cpVS;
	ComPtr<ID3D11PixelShader>	m_cpPS;
	ComPtr<ID3D11InputLayout>	m_cpInputLayout;

};
