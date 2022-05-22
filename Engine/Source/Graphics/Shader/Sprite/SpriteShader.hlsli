//-----------------------------------------------------------------------------
// Name: SpriteShader.hlsli
//
// SpriteShader共通
//-----------------------------------------------------------------------------

//定数バッファ
cbuffer cdObject : register(b4)
{
    float4 m_color;
};

//頂点シェーダが返す値
struct VertexOutput
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
};
