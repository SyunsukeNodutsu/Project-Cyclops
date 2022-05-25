//-----------------------------------------------------------------------------
// File: GPUParticle.hlsli
//
// GPUParticle共通
//-----------------------------------------------------------------------------

//定数バッファ ※SRV使用
struct cdObject
{
    float3  position;
    float   lifeSpanMax;
    float3  velocity;
    float   life;
    float4  color;
    row_major float4x4 worldMatrix;
};

//頂点シェーダが返す値
struct VertexOutput
{
    float4 m_position   : SV_Position;//射影座標
    float2 m_uv         : TEXCOORD0;
    float4 m_color      : TEXCOORD1;
    float3 m_wPosition  : TEXCOORD2;//ワールド3D座標
};

//計算シェーダーに渡される値
struct ComputeInput
{
    uint3 m_groupThread : SV_GroupThreadID;
    uint3 m_group       : SV_GroupID;
    uint  m_groupIndex  : SV_GroupIndex;
    uint3 m_dispatch    : SV_DispatchThreadID;
};
