//-----------------------------------------------------------------------------
// File: GPUParticle_VS.hlsl
//
// ���q�V�~�����[�V���� ���_�V�F�[�_
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticle.hlsli"

StructuredBuffer<Particle> particle : register(t2);//SRV

VertexOutput main(float4 position : POSITION, float2 uv : TEXCOORD0, uint InstanceID : SV_InstanceID)
{
    VertexOutput ret = (VertexOutput)0;
    position.w = 1;

    //�v�Z�V�F�[�_�[�̌��ʂ���V�~�����[�V����
    position.xyz += particle[InstanceID].position;
    ret.m_color = particle[InstanceID].color;

    //���W�ϊ�
    //ret.m_position = mul(position, g_worldMatrix);
    ret.m_position = mul(position, particle[InstanceID].worldMatrix);
    ret.m_wPosition = ret.m_position.xyz;
    ret.m_position = mul(ret.m_position, g_viewMatrix);
    ret.m_position = mul(ret.m_position, g_projMatrix);

    ret.m_uv = uv * g_uvTiling + g_uvOffset;

    return ret;
}
