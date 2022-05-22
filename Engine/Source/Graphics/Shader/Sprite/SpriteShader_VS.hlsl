//-----------------------------------------------------------------------------
// File: SpriteShader_VS.hlsl
//
// �摜�`�撸�_�V�F�[�_�[
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "SpriteShader.hlsli"

VertexOutput main(float4 position : POSITION, float2 uv : TEXCOORD0)
{
    VertexOutput ret = (VertexOutput)0;

    //���_���W���ˉe�ϊ�
    ret.m_position = mul(position, g_worldMatrix);
    ret.m_position = mul(ret.m_position, g_projMatrix);

    ret.m_uv = uv;

    return ret;
}
