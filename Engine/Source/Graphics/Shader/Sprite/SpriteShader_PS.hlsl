//-----------------------------------------------------------------------------
// File: SpriteShader_PS.hlsl
//
// �摜�`��s�N�Z���V�F�[�_�[
//-----------------------------------------------------------------------------
#include "SpriteShader.hlsli"

Texture2D g_inputTexture : register(t0);
SamplerState g_samplerState : register(s0);

float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
