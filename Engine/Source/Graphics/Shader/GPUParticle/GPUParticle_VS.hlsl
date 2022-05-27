//-----------------------------------------------------------------------------
// File: GPUParticle_VS.hlsl
//
// 粒子シミュレーション 頂点シェーダ
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticle.hlsli"

StructuredBuffer<Particle> BufIn : register(t0);//SRV

VertexOutput main(float4 position : POSITION, float2 uv : TEXCOORD0, uint InstanceID : SV_InstanceID)
{
    VertexOutput ret = (VertexOutput)0;
    position.w = 1;

    //計算シェーダーの結果からシミュレーション
    position.xyz += BufIn[InstanceID].m_position;
    ret.m_color = BufIn[InstanceID].m_color;

    //座標変換
    ret.m_position = mul(position, g_worldMatrix);
    ret.m_wPosition = ret.m_position.xyz;
    ret.m_position = mul(ret.m_position, g_viewMatrix);
    ret.m_position = mul(ret.m_position, g_projMatrix);

    ret.m_uv = uv * g_uvTiling + g_uvOffset;

    return ret;
}
