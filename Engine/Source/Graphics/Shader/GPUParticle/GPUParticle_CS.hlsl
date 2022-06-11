//-----------------------------------------------------------------------------
// File: GPUParticle_CS.hlsl
//
// 粒子シミュレーション 計算シェーダ
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticle.hlsli"

StructuredBuffer<Particle> BufIn : register(t0); //入力 SRV
RWStructuredBuffer<Particle> BufOut : register(u0); //出力 URV

[numthreads(256, 1, 1)]
void main(const ComputeInput In)
{
    int index = In.g_dispatch.x;
    if (BufIn[index].m_life <= 0) return;

    //座標,移動量,生存期間の計算
    float3 result = BufIn[index].m_position + (BufIn[index].m_velocity * g_deltaTime);
    BufOut[index].m_position      = result;
    BufOut[index].m_velocity      = BufIn[index].m_velocity;
    BufOut[index].m_life          = BufIn[index].m_life - g_deltaTime;
    BufOut[index].m_color         = BufIn[index].m_color;
    BufOut[index].m_lifeSpanMax   = BufIn[index].m_lifeSpanMax;
    
    if (!BufOut[index].m_loop)
    {
        if (BufOut[index].m_life <= 0.0f)
        {
            BufOut[index].m_life = BufOut[index].m_lifeSpanMax;
            BufOut[index].m_velocity *= -1;
        }
    }
    
    //生存期間に応じて透明度を設定
    BufOut[index].m_color.a = (BufOut[index].m_life / BufIn[index].m_lifeSpanMax);
	BufOut[index].m_color.a = saturate(BufOut[index].m_color.a);
}
