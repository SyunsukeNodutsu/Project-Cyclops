//-----------------------------------------------------------------------------
// File: SpriteShader_VS.hlsl
//
// 画像描画頂点シェーダー
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "SpriteShader.hlsli"

VertexOutput main(float4 position : POSITION, float2 uv : TEXCOORD0)
{
    VertexOutput ret = (VertexOutput)0;

    //上の2点
    if (uv.y == 0.0f)// or uv.y >= 0.9f
    {
        position.x += sin(g_totalTime) * 100;
    }

    //頂点座標を射影変換
    ret.m_position = mul(position, g_worldMatrix);
    ret.m_position = mul(ret.m_position, g_projMatrix);

    ret.m_uv = uv;

    return ret;
}
