//-----------------------------------------------------------------------------
// File: LensGhostShader_VS.hlsl
//
// レンズゴースト 頂点シェーダー
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "LensGhost.hlsli"

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}
