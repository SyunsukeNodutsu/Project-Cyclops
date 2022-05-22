//-----------------------------------------------------------------------------
// File: SpriteShader_PS.hlsl
//
// 画像描画ピクセルシェーダー
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "SpriteShader.hlsli"

Texture2D g_inputTexture : register(t0);
SamplerState g_samplerState : register(s0);

float4 main(VertexOutput In) : SV_TARGET
{
     float4 color = g_inputTexture.Sample(g_samplerState, In.m_uv);
    if (color.a <= 0.0f) discard;

    return color/* * g_color*/;
}
