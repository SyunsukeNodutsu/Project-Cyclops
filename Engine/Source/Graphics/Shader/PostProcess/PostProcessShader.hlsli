//-----------------------------------------------------------------------------
// File: PostProcessShader.hlsli
//
// PostProcessShader����
//-----------------------------------------------------------------------------

//���_�V�F�[�_���Ԃ��l
struct VertexOutput
{
    float4 position : SV_Position;  //�ˉe���W
    float2 uv       : TEXCOORD0;    //UV���W
};

//�u���[�p�萔�o�b�t�@
cbuffer cbBlur : register(b8)
{
    float4 g_offset[31]; //�s�N�Z���̂��ꕪ
};
