//-----------------------------------------------------------------------------
// File: SpriteShader_VS.hlsl
//
// �摜�`�撸�_�V�F�[�_�[
//-----------------------------------------------------------------------------
#include "SpriteShader.hlsli"

float4 main(float4 position : POSITION, float2 uv : TEXCOORD0) : SV_POSITION
{
	return position;
}
