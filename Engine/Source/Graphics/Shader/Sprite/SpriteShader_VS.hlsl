//-----------------------------------------------------------------------------
// File: SpriteShader_VS.hlsl
//
// 画像描画頂点シェーダー
//-----------------------------------------------------------------------------
#include "SpriteShader.hlsli"

float4 main(float4 position : POSITION, float2 uv : TEXCOORD0) : SV_POSITION
{
	return position;
}
