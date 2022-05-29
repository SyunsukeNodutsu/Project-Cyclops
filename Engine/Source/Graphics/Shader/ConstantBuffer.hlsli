//-----------------------------------------------------------------------------
// File: ConstantBuffer.hlsli
//
// 全シェーダ共通
//-----------------------------------------------------------------------------

//定数バッファ
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

//ACESフィルムトーン
float3 ACESFilmicTone(float3 color)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;

	return saturate((color * (a * color + b)) / (color * (c * color + d) + e));
}
