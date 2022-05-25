//-----------------------------------------------------------------------------
// File: GPUParticle.hlsli
//
// GPUParticle共通
//-----------------------------------------------------------------------------

//定数バッファ
cbuffer cdObject : register(b2)
{
    float4 g_color;
};

//頂点シェーダが返す値
struct VertexOutput
{
    float4 m_position : SV_Position;
    float2 m_uv       : TEXCOORD0;
};
