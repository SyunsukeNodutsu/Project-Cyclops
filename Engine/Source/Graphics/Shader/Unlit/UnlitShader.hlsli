//-----------------------------------------------------------------------------
// File: UnlitShader.hlsli
//
// UnlitShader共通
//-----------------------------------------------------------------------------

//頂点シェーダが返す値
struct VertexOutput
{
    float4 m_position : SV_Position;
    float4 m_color    : TEXCOORD0;
    float2 m_uv       : TEXCOORD1;
};
