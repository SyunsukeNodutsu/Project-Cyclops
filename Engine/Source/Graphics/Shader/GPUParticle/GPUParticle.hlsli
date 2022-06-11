//-----------------------------------------------------------------------------
// File: GPUParticle.hlsli
//
// GPUParticle共通
//-----------------------------------------------------------------------------

//定数バッファ ※SRV使用
struct Particle
{
    float3  m_position;
    float   m_lifeSpanMax;
    float3  m_velocity;
    float   m_life;
    float4  m_color;
    float   m_loop;
    float3  tmp;
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
    uint3 g_groupThread : SV_GroupThreadID;
    uint3 g_group       : SV_GroupID;
    uint  g_groupIndex  : SV_GroupIndex;
    uint3 g_dispatch    : SV_DispatchThreadID;
};
