//-----------------------------------------------------------------------------
// File: GrassShader_VS.hlsl
//
// ���`�� ���_�V�F�[�_�[
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GrassShader.hlsli"

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}
