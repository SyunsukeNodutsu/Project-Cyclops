//-----------------------------------------------------------------------------
// File: GPUParticle.hlsli
//
// GPUParticle����
//-----------------------------------------------------------------------------

//�萔�o�b�t�@
cbuffer cdObject : register(b2)
{
    float4 g_color;
};

//���_�V�F�[�_���Ԃ��l
struct VertexOutput
{
    float4 m_position : SV_Position;
    float2 m_uv       : TEXCOORD0;
};
