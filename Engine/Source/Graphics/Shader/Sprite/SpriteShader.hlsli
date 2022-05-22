//-----------------------------------------------------------------------------
// Name: SpriteShader.hlsli
//
// SpriteShader����
//-----------------------------------------------------------------------------

//�萔�o�b�t�@
cbuffer cdObject : register(b4)
{
    float4 m_color;
};

//���_�V�F�[�_���Ԃ��l
struct VertexOutput
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
};
