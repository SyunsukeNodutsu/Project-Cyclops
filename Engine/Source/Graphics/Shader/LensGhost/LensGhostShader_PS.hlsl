//-----------------------------------------------------------------------------
// File: LensGhostShader_PS.hlsl
//
// レンズゴースト ピクセルシェーダー
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "LensGhost.hlsli"

float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
