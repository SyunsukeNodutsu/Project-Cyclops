//-----------------------------------------------------------------------------
// File: UnlitShader_PS.hlsl
//
// 光計算無し ピクセルシェーダー
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "UnlitShader.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_samplerState : register(s0);

float4 main(VertexOutput In) : SV_TARGET
{
    return In.m_color;

    /*float4 color = g_texture.Sample(g_samplerState, In.m_uv) * In.m_color;
    if (color.a <= 0.0f) discard;

	return color;*/
}
