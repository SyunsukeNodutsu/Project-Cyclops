//-----------------------------------------------------------------------------
// File: LitShader_PS.hlsl
//
// Litピクセルシェーダー
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "LitShader.hlsli"

float4 main(VertexOutput In) : SV_TARGET
{
	return float4(1, 1, 1, 1);
}
