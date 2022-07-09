//-----------------------------------------------------------------------------
// File: LensGhostShader.hlsli
//
// LensGhost共通
//-----------------------------------------------------------------------------

//頂点シェーダが返す値
struct VertexOutput
{
    float4 m_position : SV_Position; //射影座標
    float2 m_uv : TEXCOORD0; //UV座標
};
