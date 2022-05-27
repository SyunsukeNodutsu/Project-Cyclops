//-----------------------------------------------------------------------------
// File: GPUParticle_CS.hlsl
//
// ���q�V�~�����[�V���� �v�Z�V�F�[�_
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticle.hlsli"

StructuredBuffer<Particle> BufIn : register(t0); //���� SRV
RWStructuredBuffer<Particle> BufOut : register(u0); //�o�� URV

[numthreads(256, 1, 1)]
void main(const ComputeInput In)
{
    int index = In.g_dispatch.x;
    if (BufIn[index].m_life <= 0) return;

    //���W,�ړ���,�������Ԃ̌v�Z
    float3 result = BufIn[index].m_position + (BufIn[index].m_velocity * g_deltaTime);
    BufOut[index].m_position      = result;
    BufOut[index].m_velocity      = BufIn[index].m_velocity;
    BufOut[index].m_life          = BufIn[index].m_life - g_deltaTime;
    BufOut[index].m_color         = BufIn[index].m_color;
    BufOut[index].m_lifeSpanMax   = BufIn[index].m_lifeSpanMax;

    //�������Ԃɉ����ē����x��ݒ�
    BufOut[index].m_color.a = (BufOut[index].m_life / BufIn[index].m_lifeSpanMax);
    BufOut[index].m_color.a = clamp(BufOut[index].m_color.a, 0, 1);
}
