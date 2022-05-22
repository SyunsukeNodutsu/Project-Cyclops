//-----------------------------------------------------------------------------
// File: ConstantBuffer.hlsli
//
// 全シェーダ共通
//-----------------------------------------------------------------------------

cbuffer cdBehaviour : register(b4)
{
	row_major float4x4 g_worldMatrix;
	float2 g_uvOffset;
	float2 g_uvTiling;
}
cbuffer cdCamera : register(b5)
{
	row_major float4x4 g_viewMatrix;
	row_major float4x4 g_projMatrix;
	row_major float4x4 g_cameraMatrix;
}
cbuffer cdLight : register(b6)
{
	float g_enable;
}
cbuffer cdTime : register(b7)
{
	float g_totalTime;
	float g_deltaTime;
}
