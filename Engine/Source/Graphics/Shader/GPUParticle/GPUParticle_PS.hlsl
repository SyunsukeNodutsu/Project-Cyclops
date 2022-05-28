//-----------------------------------------------------------------------------
// File: GPUParticle_PS.hlsl
//
// ���q�V�~�����[�V���� �s�N�Z���V�F�[�_
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

    return In.m_color;
}
