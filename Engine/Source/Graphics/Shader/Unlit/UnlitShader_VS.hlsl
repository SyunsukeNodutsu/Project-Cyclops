//-----------------------------------------------------------------------------
// File: UnlitShader_VS.hlsl
//
// ���v�Z���� ���_�V�F�[�_�[
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "UnlitShader.hlsli"

VertexOutput main(float4 position : POSITION, float2 uv : TEXCOORD0, float4 color : COLOR)
{
    VertexOutput ret = (VertexOutput)0;
    position.w = 1;

    ret.m_position = mul(position, g_worldMatrix);
    ret.m_position = mul(ret.m_position, g_viewMatrix);
    ret.m_position = mul(ret.m_position, g_projMatrix);

    ret.m_uv = uv * g_uvTiling + g_uvOffset;
    ret.m_color = color;

    return ret;
}
