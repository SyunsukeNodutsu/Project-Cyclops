//-----------------------------------------------------------------------------
// File: GPUParticle_VS.hlsl
//
// 
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticle.hlsli"

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}