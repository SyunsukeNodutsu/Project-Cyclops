﻿//-----------------------------------------------------------------------------
// File: SpriteShader.h
//
// 描画状態の管理 サンプラー,ラスタライザー,ブレンド など
//-----------------------------------------------------------------------------
#pragma once

enum SS_FilterMode; enum SS_AddressMode;
enum RS_CullMode; enum RS_FillMode;
enum BlendMode;

//描画状態クラス
class RendererStatus : public GraphicsDeviceChild
{
public:

	RendererStatus() = default;
	~RendererStatus() = default;

	bool Initialize();

	void SetDepthStencil(bool zUse, bool zWrite);
	bool SetSampler(SS_FilterMode filter, SS_AddressMode address);
	bool SetBlend(BlendMode flag);
	bool SetRasterize(RS_CullMode cull, RS_FillMode fill);

private:

	//エンジンがデフォルトで提供する定数バッファ -------------------->
	struct CBBehaviour
	{
		Matrix m_worldMatrix;
		Vector2 m_uvOffset;
		Vector2 m_uvTiling;
	};
	struct CBCamera
	{
		Matrix m_viewMatrix;
		Matrix m_projMatrix;
		Matrix m_cameraMatrix;
	};
	struct CBLight
	{
		float m_enable;
		float tmp[3];
	};
	struct CBTime
	{
		float m_totalTime = 0.0f; // 総経過時間
		float m_deltaTime = 1.0f; // デルタティック
		float tmp[2];
	};
	//<--------------------エンジンがデフォルトで提供する定数バッファ

	std::map<int, ComPtr<ID3D11SamplerState>>		m_samplerStates;		//サンプラーステートMap
	std::map<int, ComPtr<ID3D11RasterizerState>>	m_rasterizerState;		//ラスタライザーステートMap
	ComPtr<ID3D11DepthStencilState>					m_depthStencilStates[4];//デプスステンシルステート
	ComPtr<ID3D11BlendState>						m_blendStates[3];		//ブレンドステート

public:

	ConstantBuffer<CBBehaviour> m_cb4Behaviour;
	ConstantBuffer<CBCamera>	m_cb5Camera;
	ConstantBuffer<CBLight>		m_cb6Light;
	ConstantBuffer<CBTime>		m_cb7Time;

private:

	bool CreateSampler(SS_FilterMode filter, SS_AddressMode address);
	bool CreateRasterrize(RS_CullMode cull, RS_FillMode fill);
	ComPtr<ID3D11DepthStencilState> CreateDepthStencil(bool zUse, bool zWrite);
	ComPtr<ID3D11BlendState> CreateBlend(BlendMode flag);

};

#pragma region enums
//サンプラーステート
enum SS_FilterMode
{
	Point		= 0x00000001,	//ドット調
	Linear		= 0x00000002,	//線形補完
	Aniso		= 0x00000004,	//異方性
};
enum SS_AddressMode
{
	Wrap		= 0x00010000,	//繰り返し
	Clamp		= 0x00020000,	//端を無視
	Mirror		= 0x00040000,	//反転
};

//ブレンドステート
enum BlendMode
{
	BlendNone	= 0,			//ブレンドしない
	Add,						//加算合成
	Alpha,						//色合成
};

// ラスタライザーステート
enum RS_CullMode
{
	CullNone	= 0x00000001,	//カリングなし
	Back		= 0x00000002,	//裏面カリング
	Front		= 0x00000004,	//表面カリング
};
enum RS_FillMode
{
	Wire		= 0x00000001,	//ワイヤーフレーム表示
	Solid		= 0x00000002,	//塗りつぶし
};
#pragma endregion
