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

	UnlitShader();
	~UnlitShader() = default;

	bool Initialize();

private:

	ComPtr<ID3D11VertexShader>	m_cpVS;
	ComPtr<ID3D11PixelShader>	m_cpPS;
	ComPtr<ID3D11InputLayout>	m_cpInputLayout;

};
