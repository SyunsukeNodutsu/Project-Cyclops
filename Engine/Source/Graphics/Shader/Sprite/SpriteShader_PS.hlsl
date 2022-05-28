//-----------------------------------------------------------------------------
// File: SpriteShader_PS.hlsl
//
// �摜�`��s�N�Z���V�F�[�_�[
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "SpriteShader.hlsli"

Texture2D g_inputTexture : register(t0);
SamplerState g_samplerState : register(s0);

float4 main(VertexOutput In) : SV_TARGET
{
    float4 color = g_inputTexture.Sample(g_samplerState, In.m_uv);
    if (color.a <= 0.0f) discard;

    //���m�N��
    float y = 0.299f * color.r + 0.587f * color.g + 0.114f * color.b;
    float3 mono_col = float3(y, y, y);
    color.xyz = lerp(color.xyz, mono_col, abs(cos(g_totalTime)));

    //���������C�v
    //float t = (int)fmod(In.m_position.x, 72.0f);
    //clip(t - lerp(0.0f, 72.0f, abs(cos(g_totalTime))));

    //�c�������C�v
    //t = (int)fmod(In.m_position.y, 72.0f);
    //clip(t - lerp(0.0f, 72.0f, abs(cos(g_totalTime))));

    return color * g_color;
}
