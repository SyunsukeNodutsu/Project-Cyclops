//-----------------------------------------------------------------------------
// File: SpriteShader.hlsli
//
// SpriteShader共通
//-----------------------------------------------------------------------------

//頂点シェーダが返す値
struct VertexOutput
{
    float4 m_position : SV_Position;
    float2 m_uv       : TEXCOORD0;
};

cbuffer cdColor : register(b0)
{
    float4 g_color;
}
