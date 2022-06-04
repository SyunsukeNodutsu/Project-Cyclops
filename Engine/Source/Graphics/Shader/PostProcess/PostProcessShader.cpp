#include "PostProcessShader.h"

//-----------------------------------------------------------------------------
// 作成
//-----------------------------------------------------------------------------
void BlurTexture::Create(int width, int height, bool useMSAA)
{
	int divideValue = 2;
	for (int i = 0; i < 5; i++)
	{
		int _width = width / divideValue;
		int _height = height / divideValue;

		m_rt[i][0] = std::make_shared<Texture>();
		m_rt[i][0]->CreateRenderTarget(_height, _width, DXGI_FORMAT_R16G16B16A16_FLOAT);

		m_rt[i][1] = std::make_shared<Texture>();
		m_rt[i][1]->CreateRenderTarget(_height, _width, DXGI_FORMAT_R16G16B16A16_FLOAT);
		divideValue *= 2;
	}
}

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
PostProcessShader::PostProcessShader()
	: m_cpVS(nullptr)
	, m_cpInputLayout(nullptr)
	, m_colorPS(nullptr)
	, m_blurPS(nullptr)
	, m_HBrightPS(nullptr)
	, m_cb8Blur()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool PostProcessShader::Initialize()
{
	//頂点シェーダ/入力レイアウト
	{
		#include "PostProcessShader_VS.shaderinc"

		if (FAILED(m_graphicsDevice->m_cpDevice->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpVS.GetAddressOf())))
		{
			Debug::LogError("頂点シェーダー作成失敗."); return false;
		}

		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		if (FAILED(m_graphicsDevice->m_cpDevice->CreateInputLayout(&layout[0], (UINT)layout.size(), compiledBuffer, sizeof(compiledBuffer), m_cpInputLayout.GetAddressOf())))
		{
			Debug::LogError("CreateInputLayout失敗."); return false;
		}
	}

	//ピクセルシェーダ/Color_PS
	{
		#include "PostProcessShader_Color_PS.shaderinc"

		if (FAILED(m_graphicsDevice->m_cpDevice->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_colorPS.GetAddressOf())))
		{
			Debug::LogError("ピクセルシェーダー作成失敗."); return false;
		}
	}

	//ピクセルシェーダ/Blur_PS
	{
		#include "PostProcessShader_Blur_PS.shaderinc"

		if (FAILED(m_graphicsDevice->m_cpDevice->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_blurPS.GetAddressOf())))
		{
			Debug::LogError("ピクセルシェーダー作成失敗."); return false;
		}
	}

	//ピクセルシェーダ/HBright_PS
	{
		#include "PostProcessShader_HBright_PS.shaderinc"

		if (FAILED(m_graphicsDevice->m_cpDevice->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_HBrightPS.GetAddressOf())))
		{
			Debug::LogError("ピクセルシェーダー作成失敗."); return false;
		}
	}

	if (m_cb8Blur.Create())
	{
		m_cb8Blur.SetToDevice(8, SHADER_STAGE::VS);
		m_cb8Blur.SetToDevice(8, SHADER_STAGE::PS);
	}
	else { Debug::LogError("定数バッファ作成失敗."); return false; }

    return true;
}

//-----------------------------------------------------------------------------
// 加工段階描画
//-----------------------------------------------------------------------------
void PostProcessShader::DrawColor(Texture* texture)
{
	if (texture == nullptr) return;

	m_graphicsDevice->m_cpContext->IASetInputLayout(m_cpInputLayout.Get());

	m_graphicsDevice->m_cpContext->VSSetShader(m_cpVS.Get(), 0, 0);
	m_graphicsDevice->m_cpContext->PSSetShader(m_colorPS.Get(), 0, 0);

	m_graphicsDevice->m_cpContext->PSSetShaderResources(0, 1, texture->SRVAddress());

	//Z判定、Z書き込み無効のステートをセット
	m_graphicsDevice->m_spRendererStatus->SetDepthStencil(false, false);
	m_graphicsDevice->m_spRendererStatus->SetSampler(SS_FilterMode::Linear, SS_AddressMode::Clamp);
	m_graphicsDevice->m_spRendererStatus->SetRasterize(RS_CullMode::CullNone, RS_FillMode::Solid);

	m_graphicsDevice->DrawVertices(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, &vOrthographic[0], sizeof(Vertex));

	m_graphicsDevice->m_spRendererStatus->SetDepthStencil(true, true);
	m_graphicsDevice->m_spRendererStatus->SetSampler(SS_FilterMode::Aniso, SS_AddressMode::Wrap);
	m_graphicsDevice->m_spRendererStatus->SetRasterize(RS_CullMode::Back, RS_FillMode::Solid);

	ID3D11ShaderResourceView* resource_zero = nullptr;
	m_graphicsDevice->m_cpContext->PSSetShaderResources(0, 1, &resource_zero);
}

//-----------------------------------------------------------------------------
// ガウスブラー描画
//-----------------------------------------------------------------------------
void PostProcessShader::BlurDraw(Texture* texture, float2 dir)
{
	//ステート記憶
	ComPtr<ID3D11DepthStencilState> saveDS; UINT saveStencilRef = 0;
	m_graphicsDevice->m_cpContext->OMGetDepthStencilState(&saveDS, &saveStencilRef);

	m_graphicsDevice->m_spRendererStatus->SetDepthStencil(false, false);
	m_graphicsDevice->m_spRendererStatus->SetSampler(SS_FilterMode::Linear, SS_AddressMode::Clamp);

	//サンプリング
	{
		//テクセルサイズ
		float2 ts;
		ts.x = 1.0f / texture->GetWidth();
		ts.y = 1.0f / texture->GetHeight();

		//ずれ分の座標を計算
		constexpr int MAX_SAMPLING = 31;
		constexpr float DISPERSION = 3;
		float totalWeight = 0;
		for (int i = 0; i < MAX_SAMPLING; i++)
		{
			int pt = i - MAX_SAMPLING / 2;
			m_cb8Blur.Work().offset[i].x = dir.x * (pt * ts.x);
			m_cb8Blur.Work().offset[i].y = dir.y * (pt * ts.y);

			//ガウス係数の計算
			float w = exp(-(pt * pt) / (2 * DISPERSION * DISPERSION));
			m_cb8Blur.Work().offset[i].z = w;

			//合計を計算
			totalWeight += w;
		}

		//係数を正規化
		for (int i = 0; i < 31; i++)
			m_cb8Blur.Work().offset[i].z /= totalWeight;

		m_cb8Blur.Write();
	}

	m_graphicsDevice->m_cpContext->PSSetShaderResources(0, 1, texture->SRVAddress());

	m_graphicsDevice->m_cpContext->IASetInputLayout(m_cpInputLayout.Get());

	m_graphicsDevice->m_cpContext->VSSetShader(m_cpVS.Get(), 0, 0);
	m_graphicsDevice->m_cpContext->PSSetShader(m_blurPS.Get(), 0, 0);

	m_graphicsDevice->DrawVertices(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, &vOrthographic[0], sizeof(Vertex));

	ID3D11ShaderResourceView* resource_zero = nullptr;
	m_graphicsDevice->m_cpContext->PSSetShaderResources(0, 1, &resource_zero);

	//ステート復元
	m_graphicsDevice->m_cpContext->OMSetDepthStencilState(saveDS.Get(), saveStencilRef);
	m_graphicsDevice->m_spRendererStatus->SetSampler(SS_FilterMode::Aniso, SS_AddressMode::Wrap);
}

//-----------------------------------------------------------------------------
// ブラー描画
//-----------------------------------------------------------------------------
void PostProcessShader::GenerateBlur(BlurTexture& blurTex, Texture* srcTex)
{
	RestoreRenderTarget rrt{};

	//ビューポート記憶
	D3D11_VIEWPORT saveVP = {};
	UINT numVP = 1;
	m_graphicsDevice->m_cpContext->RSGetViewports(&numVP, &saveVP);

	//サンプリングを繰り返す/輝度の抽出
	for (int i = 0; i < 5; i++)
	{
		m_graphicsDevice->m_cpContext->OMSetRenderTargets(1, blurTex.m_rt[i][0]->RTVAddress(), nullptr);

		D3D11_VIEWPORT vp = { 0.0f, 0.0f, (float)blurTex.m_rt[i][0]->GetWidth(),(float)blurTex.m_rt[i][0]->GetHeight(), 0.0f, 1.0f };
		m_graphicsDevice->m_cpContext->RSSetViewports(1, &vp);

		if (i == 0) DrawColor(srcTex);
		else DrawColor(blurTex.m_rt[i - 1][0].get());

		//縦ぼかし
		m_graphicsDevice->m_cpContext->OMSetRenderTargets(1, blurTex.m_rt[i][1]->RTVAddress(), nullptr);
		BlurDraw(blurTex.m_rt[i][0].get(), float2(1, 0));

		//横ぼかし
		m_graphicsDevice->m_cpContext->OMSetRenderTargets(1, blurTex.m_rt[i][0]->RTVAddress(), nullptr);
		BlurDraw(blurTex.m_rt[i][1].get(), float2(0, 1));
	}

	//記憶したビューポートをもとに復元
	m_graphicsDevice->m_cpContext->RSSetViewports(1, &saveVP);
}

//-----------------------------------------------------------------------------
// フィルタリング
//-----------------------------------------------------------------------------
void PostProcessShader::BrightFiltering(Texture* destRT, Texture* srcTex)
{
	RestoreRenderTarget rrt = {};
	m_graphicsDevice->m_cpContext->OMSetRenderTargets(1, destRT->RTVAddress(), nullptr);

	//ビューポート記憶
	D3D11_VIEWPORT saveVP = {};
	UINT numVP = 1;
	m_graphicsDevice->m_cpContext->RSGetViewports(&numVP, &saveVP);

	//ソーステクスチャに応じたビューポート設定
	D3D11_VIEWPORT vp = { 0.0f, 0.0f,(float)destRT->GetWidth(), (float)destRT->GetHeight(), 0.0f, 1.0f };
	m_graphicsDevice->m_cpContext->RSSetViewports(1, &vp);

	//デプスステンシル記憶
	ID3D11DepthStencilState* saveDS = {};
	UINT saveStencilRef = 0;
	m_graphicsDevice->m_cpContext->OMGetDepthStencilState(&saveDS, &saveStencilRef);

	//サンプラー/デプスステンシル設定
	m_graphicsDevice->m_spRendererStatus->SetDepthStencil(false, false);
	m_graphicsDevice->m_spRendererStatus->SetSampler(SS_FilterMode::Linear, SS_AddressMode::Clamp);

	{
		m_graphicsDevice->m_cpContext->PSSetShaderResources(0, 1, srcTex->SRVAddress());

		m_graphicsDevice->m_cpContext->IASetInputLayout(m_cpInputLayout.Get());

		m_graphicsDevice->m_cpContext->VSSetShader(m_cpVS.Get(), 0, 0);
		m_graphicsDevice->m_cpContext->PSSetShader(m_HBrightPS.Get(), 0, 0);

		m_graphicsDevice->DrawVertices(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, &vOrthographic[0], sizeof(Vertex));
	}

	//NULLリソース設定
	ID3D11ShaderResourceView* nullSRV = nullptr;
	m_graphicsDevice->m_cpContext->PSSetShaderResources(0, 1, &nullSRV);

	//サンプラー/デプスステンシル復元
	m_graphicsDevice->m_spRendererStatus->SetSampler(SS_FilterMode::Aniso, SS_AddressMode::Wrap);
	m_graphicsDevice->m_cpContext->OMSetDepthStencilState(saveDS, saveStencilRef);
	saveDS->Release();

	//記憶したビューポートから復元
	m_graphicsDevice->m_cpContext->RSSetViewports(1, &saveVP);
}
