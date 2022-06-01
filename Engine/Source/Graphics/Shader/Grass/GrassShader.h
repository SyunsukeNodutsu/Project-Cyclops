//-----------------------------------------------------------------------------
// File: GrassShader.h
//
// 草描画シェーダー
//-----------------------------------------------------------------------------
#pragma once

//草シェーダー
class GrassShader : public GraphicsDeviceChild
{
public:

	GrassShader();
	~GrassShader() = default;

	bool Initialize();

private:

	ComPtr<ID3D11VertexShader>	m_cpVS;
	ComPtr<ID3D11PixelShader>	m_cpPS;
	ComPtr<ID3D11InputLayout>	m_cpInputLayout;

};
