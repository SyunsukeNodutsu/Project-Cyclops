//-----------------------------------------------------------------------------
// File: SpriteShader.hlsli
//
// SpriteShader����
//-----------------------------------------------------------------------------

//���_�V�F�[�_���Ԃ��l
struct VertexOutput
{
    float4 m_position : SV_Position;
    float2 m_uv       : TEXCOORD0;
};

cbuffer cdColor : register(b0)
{
    float4 g_color;
}
