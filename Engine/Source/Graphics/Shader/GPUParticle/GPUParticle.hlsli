//-----------------------------------------------------------------------------
// File: GPUParticle.hlsli
//
// GPUParticle����
//-----------------------------------------------------------------------------

//�萔�o�b�t�@ ��SRV�g�p
struct Particle
{
    float3  position;
    float   lifeSpanMax;
    float3  velocity;
    float   life;
    float4  color;
    row_major float4x4 worldMatrix;
};

//���_�V�F�[�_���Ԃ��l
struct VertexOutput
{
    float4 m_position   : SV_Position;//�ˉe���W
    float2 m_uv         : TEXCOORD0;
    float4 m_color      : TEXCOORD1;
    float3 m_wPosition  : TEXCOORD2;//���[���h3D���W
};

//�v�Z�V�F�[�_�[�ɓn�����l
struct ComputeInput
{
    uint3 g_groupThread : SV_GroupThreadID;
    uint3 g_group       : SV_GroupID;
    uint  g_groupIndex  : SV_GroupIndex;
    uint3 g_dispatch    : SV_DispatchThreadID;
};
