//-----------------------------------------------------------------------------
// File: GPUParticle_GS.hlsl
//
// 粒子シミュレーション ジオメトリシェーダ
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticle.hlsli"

[maxvertexcount(4)]
void main(point VertexOutput In[1], inout TriangleStream<VertexOutput> outputStream)
{
	//TODO: 定数バッファ
	const float size = 0.02f;
	const float aspectRatio = 1600.0f / 900.0f;//「.0f」がないと整数として計算される

	float posX = size;
	float posY = posX * aspectRatio;

	//4頂点を カメラに向くように生成
	VertexOutput ret = (VertexOutput)0;

    ret.m_position	= In[0].m_position + float4(-posX, posY, 0, 0);
    ret.m_uv		= float2(0, 0);
    ret.m_color		= In[0].m_color;
    ret.m_wPosition = In[0].m_wPosition;
	outputStream.Append(ret);

	ret.m_position	= In[0].m_position + float4(posX, posY, 0, 0);
	ret.m_uv		= float2(1, 0);
	ret.m_color		= In[0].m_color;
	ret.m_wPosition = In[0].m_wPosition;
	outputStream.Append(ret);

	ret.m_position	= In[0].m_position + float4(-posX, -posY, 0, 0);
	ret.m_uv		= float2(0, 1);
	ret.m_color		= In[0].m_color;
	ret.m_wPosition = In[0].m_wPosition;
	outputStream.Append(ret);

	ret.m_position	= In[0].m_position + float4(posX, -posY, 0, 0);
	ret.m_uv		= float2(1, 1);
	ret.m_color		= In[0].m_color;
	ret.m_wPosition = In[0].m_wPosition;
	outputStream.Append(ret);
}
