//-----------------------------------------------------------------------------
// File: UnlitShader.hlsli
//
// UnlitShader����
//-----------------------------------------------------------------------------

//���_�V�F�[�_���Ԃ��l
struct VertexOutput
{
    float4 m_position : SV_Position;
    float4 m_color    : TEXCOORD0;
    float2 m_uv       : TEXCOORD1;
};
