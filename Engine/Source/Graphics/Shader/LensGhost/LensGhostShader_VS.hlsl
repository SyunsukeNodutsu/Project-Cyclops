//-----------------------------------------------------------------------------
// File: LensGhostShader_VS.hlsl
//
// �����Y�S�[�X�g ���_�V�F�[�_�[
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "LensGhost.hlsli"

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}
