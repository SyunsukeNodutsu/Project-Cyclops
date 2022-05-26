//-----------------------------------------------------------------------------
// File: LitShader.h
//
// ライトの影響を受けるシェーダー
//-----------------------------------------------------------------------------
#pragma once

//ライトの影響を受けるシェーダー
class LitShader : public GraphicsDeviceChild
{
public:

	LitShader();
	~LitShader() = default;

	bool Initialize();

private:

	ComPtr<ID3D11VertexShader>	m_cpVS;
	ComPtr<ID3D11PixelShader>	m_cpPS;
	ComPtr<ID3D11InputLayout>	m_cpInputLayout;

};
