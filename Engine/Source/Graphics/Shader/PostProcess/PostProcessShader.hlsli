//-----------------------------------------------------------------------------
// File: PostProcessShader.hlsli
//
// PostProcessShader共通
//-----------------------------------------------------------------------------

//頂点シェーダが返す値
struct VertexOutput
{
    float4 position : SV_Position;  //射影座標
    float2 uv       : TEXCOORD0;    //UV座標
};

//ブラー用定数バッファ
cbuffer cbBlur : register(b8)
{
    float4 g_offset[31]; //ピクセルのずれ分
};
