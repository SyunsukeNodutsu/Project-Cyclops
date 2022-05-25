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
    if (BufIn[index].life <= 0) return;

    //座標,移動量,生存期間の計算
    float3 test;
    test.x = BufOut[index].worldMatrix._41;
    test.y = BufOut[index].worldMatrix._42;
    test.z = BufOut[index].worldMatrix._43;
    float3 result = test + (BufIn[index].velocity * g_deltaTime);

    BufOut[index].position      = result;
    BufOut[index].velocity      = BufIn[index].velocity;
    BufOut[index].life          = BufIn[index].life - g_deltaTime;
    BufOut[index].color         = BufIn[index].color;
    BufOut[index].lifeSpanMax   = BufIn[index].lifeSpanMax;

    BufOut[index].worldMatrix = BufIn[index].worldMatrix;
    BufOut[index].worldMatrix._41 = result.x;
    BufOut[index].worldMatrix._42 = result.y;
    BufOut[index].worldMatrix._43 = result.z;

    //生存期間に応じて透明度を設定
    BufOut[index].color.a = (BufOut[index].life / BufIn[index].lifeSpanMax);
    BufOut[index].color.a = clamp(BufOut[index].color.a, 0, 1);
}