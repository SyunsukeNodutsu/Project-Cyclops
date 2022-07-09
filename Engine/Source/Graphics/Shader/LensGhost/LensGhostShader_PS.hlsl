//-----------------------------------------------------------------------------
// File: LensGhostShader_PS.hlsl
//
// �����Y�S�[�X�g �s�N�Z���V�F�[�_�[
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "LensGhostShader.hlsli"

Texture2D g_inputTexture : register(t0);
SamplerState g_samplerState : register(s0);

uniform int numGhosts; //�S�[�X�g�T���v����
uniform float numGhostDispersal; //���U�W��

float4 main(VertexOutput In) : SV_TARGET
{
    float4 result = float4(0.0f, 0.0f, 0.0f, 1.0f);
 
    float2 size;
    g_inputTexture.GetDimensions(size.x, size.y);
    
    float2 texcoord = -In.m_uv + float2(1.0f, 1.0f);
    float2 texelSize = 1.0f / size;
    
    const float2 ghostVec = (float2(0.5f, 0.5f) - texcoord) * numGhostDispersal;
    
    for (uint i = 0; i < numGhosts; i++)
    {
        float2 offset = frac(texcoord + ghostVec * float(i));
      
        float weight = length(float2(0.5f, 0.5f) - offset) / length(float2(0.5, 0.5f));
        weight = pow(1.0 - weight, 10.0);
  
        result += g_inputTexture.Sample(g_samplerState, offset) * weight;
    }

    return result;
}
