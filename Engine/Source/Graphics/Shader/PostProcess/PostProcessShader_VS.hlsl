//-----------------------------------------------------------------------------
// File: PostProcessShader_VS.hlsl
// 
// 
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "PostProcessShader.hlsli"

VertexOutput main(float4 pos : POSITION, float2 uv : TEXCOORD0)
{
    VertexOutput ret = (VertexOutput)0;
    ret.position = pos;
    ret.uv = uv;

    return ret;
}
