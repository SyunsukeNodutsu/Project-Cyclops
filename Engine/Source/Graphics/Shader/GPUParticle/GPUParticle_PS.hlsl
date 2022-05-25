//-----------------------------------------------------------------------------
// File: GPUParticle_PS.hlsl
//
// 粒子シミュレーション ピクセルシェーダ
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticle.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_samplerState : register(s0);

float4 main(VertexOutput In) : SV_TARGET
{
    if (In.m_color.a <= 0) discard;

    float4 texcolor = g_texture.Sample(g_samplerState, In.m_uv);
    if (texcolor.a <= 0) discard;

    return texcolor * In.m_color * 1.2f;
}
