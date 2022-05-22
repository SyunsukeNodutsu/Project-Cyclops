#include "RendererStatus.h"

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool RendererStatus::Initialize()
{
    SetDepthStencil(true, true);
    SetSampler(SS_FilterMode::Aniso, SS_AddressMode::Wrap);
    SetBlend(BlendMode::Alpha);
    SetRasterize(RS_CullMode::Back, RS_FillMode::Solid);

    return true;
}

//-----------------------------------------------------------------------------
// デプスステンシルステート設定
//-----------------------------------------------------------------------------
void RendererStatus::SetDepthStencil(bool zUse, bool zWrite)
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpContext == nullptr) return;

	int useIdx = -1;
	if (zUse && zWrite)		useIdx = 0;//Zバッファ使用   深度書き込みON
	if (zUse && !zWrite)	useIdx = 1;//Zバッファ使用   深度書き込みOFF
	if (!zUse && zWrite)	useIdx = 2;//Zバッファ不使用 深度書き込みON
	if (!zUse && !zWrite)	useIdx = 3;//Zバッファ不使用 深度書き込みOFF

	if (useIdx < 0) return;

	//まだ作成されていない
	if (m_depthStencilStates[useIdx] == nullptr)
		m_depthStencilStates[useIdx] = CreateDepthStencil(zUse, zWrite);

	if (m_depthStencilStates[useIdx] == nullptr)
		return;

	m_graphicsDevice->m_cpContext.Get()->OMSetDepthStencilState(m_depthStencilStates[useIdx].Get(), 0);
}

//-----------------------------------------------------------------------------
// サンプラーステート設定
//-----------------------------------------------------------------------------
bool RendererStatus::SetSampler(SS_FilterMode filter, SS_AddressMode address)
{
	if (m_graphicsDevice == nullptr) return false;
	if (m_graphicsDevice->m_cpContext == nullptr) return false;

	//MAP確認 新規作成
	if (m_samplerStates.find(filter | address) == m_samplerStates.end())
	{
		if (!CreateSampler(filter, address))
			return false;
	}
	
	//TODO: ステージ
	m_graphicsDevice->m_cpContext.Get()->VSSetSamplers(0, 1, m_samplerStates[filter | address].GetAddressOf());
	m_graphicsDevice->m_cpContext.Get()->PSSetSamplers(0, 1, m_samplerStates[filter | address].GetAddressOf());
	m_graphicsDevice->m_cpContext.Get()->CSSetSamplers(0, 1, m_samplerStates[filter | address].GetAddressOf());
	m_graphicsDevice->m_cpContext.Get()->GSSetSamplers(0, 1, m_samplerStates[filter | address].GetAddressOf());
	m_graphicsDevice->m_cpContext.Get()->DSSetSamplers(0, 1, m_samplerStates[filter | address].GetAddressOf());

	return true;
}

//-----------------------------------------------------------------------------
// ブレンドモード設定
//-----------------------------------------------------------------------------
bool RendererStatus::SetBlend(BlendMode flag)
{
	if (m_graphicsDevice == nullptr) return false;
	if (m_graphicsDevice->m_cpContext == nullptr) return false;

	if (m_blendStates[flag] == nullptr)
		m_blendStates[flag] = CreateBlend(flag);

	if (m_blendStates[flag] == nullptr)
		return false;

	float a[4]{ 0,0,0,0 };
	m_graphicsDevice->m_cpContext.Get()->OMSetBlendState(m_blendStates[flag].Get(), a, 0xFFFFFFFF);

	return true;
}

//-----------------------------------------------------------------------------
// ラスタライザー設定
//-----------------------------------------------------------------------------
bool RendererStatus::SetRasterize(RS_CullMode cull, RS_FillMode fill)
{
	if (m_graphicsDevice == nullptr) return false;
	if (m_graphicsDevice->m_cpContext == nullptr) return false;

	if (m_rasterizerState.find(cull | fill) == m_rasterizerState.end())
	{
		if (!CreateRasterrize(cull, fill))
			return false;
	}

	m_graphicsDevice->m_cpContext.Get()->RSSetState(m_rasterizerState[(cull | fill)].Get());

	return true;
}

//-----------------------------------------------------------------------------
// サンプラーステート作成
//-----------------------------------------------------------------------------
bool RendererStatus::CreateSampler(SS_FilterMode filter, SS_AddressMode address)
{
	if (m_graphicsDevice == nullptr) return false;
	if (m_graphicsDevice->m_cpContext == nullptr) return false;

	D3D11_SAMPLER_DESC desc{};
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	//フィルタリングモード
	switch (filter)
	{
	case SS_FilterMode::Linear:
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;

	case SS_FilterMode::Point:
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;

	case SS_FilterMode::Aniso:
		desc.Filter = D3D11_FILTER_ANISOTROPIC;
		desc.MaxAnisotropy = 4;
		break;
	}

	//アドレッシングモード
	switch (address)
	{
	case SS_AddressMode::Wrap:
		desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		break;

	case SS_AddressMode::Mirror:
		desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
		break;

	case SS_AddressMode::Clamp:
		desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	}

	ComPtr<ID3D11SamplerState> state = nullptr;
	if (FAILED(m_graphicsDevice->m_cpDevice.Get()->CreateSamplerState(&desc, state.GetAddressOf())))
	{
		Debug::Log("エラー：サンプラーステート作成失敗"); return false;
	}

	//登録/追加
	m_samplerStates[(filter | address)] = state;

	return true;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool RendererStatus::CreateRasterrize(RS_CullMode cull, RS_FillMode fill)
{
	if (m_graphicsDevice == nullptr) return false;
	if (m_graphicsDevice->m_cpContext == nullptr) return false;

	D3D11_RASTERIZER_DESC desc{};

	//カリングモード
	switch (cull)
	{
	case RS_CullMode::CullNone: desc.CullMode = D3D11_CULL_NONE; break;
	case RS_CullMode::Front: desc.CullMode = D3D11_CULL_FRONT; break;
	case RS_CullMode::Back: desc.CullMode = D3D11_CULL_BACK; break;
	}
	//塗りつぶし設定
	switch (fill)
	{
	case RS_FillMode::Solid: desc.FillMode = D3D11_FILL_SOLID; break;
	case RS_FillMode::Wire:  desc.FillMode = D3D11_FILL_WIREFRAME; break;
	}

	desc.FrontCounterClockwise	= FALSE;
	desc.DepthBias				= 0;
	desc.DepthBiasClamp			= 0;
	desc.SlopeScaledDepthBias	= 0;
	desc.DepthClipEnable		= TRUE;
	desc.ScissorEnable			= FALSE;
	desc.MultisampleEnable		= TRUE;
	desc.AntialiasedLineEnable	= FALSE;

	ComPtr<ID3D11RasterizerState> state = nullptr;
	if (FAILED(m_graphicsDevice->m_cpDevice.Get()->CreateRasterizerState(&desc, state.GetAddressOf())))
	{
		Debug::Log("エラー：ラスタライザーステート作成失敗"); return false;
	}

	m_rasterizerState[(cull | fill)] = state;

	return true;
}

//-----------------------------------------------------------------------------
// デプスステンシルステート作成
//-----------------------------------------------------------------------------
ComPtr<ID3D11DepthStencilState> RendererStatus::CreateDepthStencil(bool zUse, bool zWrite)
{
	if (m_graphicsDevice == nullptr) return nullptr;
	if (m_graphicsDevice->m_cpContext == nullptr) return nullptr;

	D3D11_DEPTH_STENCIL_DESC desc{};

	//深度テスト
	desc.DepthEnable = zUse;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;//判定方法

	//深度書き込み
	desc.DepthWriteMask = zWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;

	//Stencil使用せず
	desc.StencilEnable = FALSE;
	desc.StencilReadMask = 0;
	desc.StencilWriteMask = 0;

	ComPtr<ID3D11DepthStencilState> state = nullptr;
	if (FAILED(m_graphicsDevice->m_cpDevice.Get()->CreateDepthStencilState(&desc, &state)))
	{
		Debug::Log("エラー：デプスステンシルステート作成失敗"); return nullptr;
	}

	return state;
}

//-----------------------------------------------------------------------------
// ブレンドステート作成
//-----------------------------------------------------------------------------
ComPtr<ID3D11BlendState> RendererStatus::CreateBlend(BlendMode flag)
{
	if (m_graphicsDevice == nullptr) return nullptr;
	if (m_graphicsDevice->m_cpContext == nullptr) return nullptr;

	D3D11_BLEND_DESC desc{};
	desc.RenderTarget[0].BlendEnable = TRUE;
	//書き込みマスク ALL...RGBA全て出力
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	desc.IndependentBlendEnable = FALSE;
	desc.AlphaToCoverageEnable = FALSE;

	switch (flag)
	{
	case BlendMode::Alpha:
		//色の合成方法
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		//アルファの合成方法
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		break;

	case BlendMode::Add://加算ブレンド
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		break;

	case BlendMode::BlendNone://ブレンドしない
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		break;
	}

	ComPtr<ID3D11BlendState> state = nullptr;
	if (FAILED(m_graphicsDevice->m_cpDevice.Get()->CreateBlendState(&desc, state.GetAddressOf())))
	{
		Debug::Log("エラー：ブレンドステート作成失敗."); return nullptr;
	}

	return state;
}
