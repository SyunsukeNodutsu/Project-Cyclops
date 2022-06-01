//-----------------------------------------------------------------------------
// File: GrassShader_VS.hlsl
//
// 草描画 頂点シェーダー
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GrassShader.hlsli"

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}
