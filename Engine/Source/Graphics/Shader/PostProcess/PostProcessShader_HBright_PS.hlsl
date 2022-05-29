//-----------------------------------------------------------------------------
// File: PostProcess_HBright_PS.hlsl
// 
// 
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "PostProcessShader.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_samplerState : register(s0);

float4 main(VertexOutput In) : SV_TARGET
{
    float4 tex_color = g_texture.Sample(g_samplerState, In.uv);
    if (tex_color.a <= 0) discard;

    tex_color.rgb = max(0, tex_color.rgb - 1.0f);

    //トーンフィルタ
    tex_color.rgb = ACESFilmicTone(tex_color.rgb * 1.0f);

    return tex_color;
}
