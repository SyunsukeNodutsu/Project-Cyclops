#include "SpriteShader.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
SpriteShader::SpriteShader()
	: m_cpVS(nullptr)
	, m_cpPS(nullptr)
	, m_cpInputLayout(nullptr)
	, m_cbSprite()
	, m_prevProjMatrix()
	, m_begin(false)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool SpriteShader::Initialize()
{
	//頂点シェーダ
	{
		#include "SpriteShader_VS.shaderinc"

		if (FAILED(m_graphicsDevice->m_cpDevice.Get()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpVS.GetAddressOf())))
		{
			Debug::Log("頂点シェーダー作成失敗."); return false;
		}

		//頂点インプットレイアウト作成
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		if (FAILED(m_graphicsDevice->m_cpDevice.Get()->CreateInputLayout(&layout[0], (UINT)layout.size(), compiledBuffer, sizeof(compiledBuffer), m_cpInputLayout.GetAddressOf())))
		{
			Debug::Log("CreateInputLayout失敗."); return false;
		}
	}

	//ピクセルシェーダ
	{
		#include "SpriteShader_PS.shaderinc"

		if (FAILED(m_graphicsDevice->m_cpDevice.Get()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpPS.GetAddressOf())))
		{
			Debug::Log("ピクセルシェーダー作成失敗."); return false;
		}
	}

	//定数バッファ作成
	if (!m_cbSprite.Create())
	{
		Debug::Log("定数バッファ作成失敗."); return false;
	}
	m_cbSprite.SetToDevice(0, SHADER_STAGE::VS);
	m_cbSprite.SetToDevice(0, SHADER_STAGE::PS);

	return true;
}

//-----------------------------------------------------------------------------
// 描画開始
//-----------------------------------------------------------------------------
void SpriteShader::Begin(bool linear, bool disableZBuffer)
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpContext == nullptr) return;
	if (m_graphicsDevice->m_spRendererStatus == nullptr) return;

	//TODO: 定数バッファ書き込み

	//ステート設定
	if (disableZBuffer) m_graphicsDevice->m_spRendererStatus->SetDepthStencil(false, false);
	if (linear) m_graphicsDevice->m_spRendererStatus->SetSampler(SS_FilterMode::Linear, SS_AddressMode::Wrap);
	else m_graphicsDevice->m_spRendererStatus->SetSampler(SS_FilterMode::Point, SS_AddressMode::Wrap);
	m_graphicsDevice->m_spRendererStatus->SetRasterize(RS_CullMode::CullNone, RS_FillMode::Solid);

	//シェーダー設定
	m_graphicsDevice->m_cpContext->VSSetShader(m_cpVS.Get(), 0, 0);
	m_graphicsDevice->m_cpContext->PSSetShader(m_cpPS.Get(), 0, 0);
	m_graphicsDevice->m_cpContext->IASetInputLayout(m_cpInputLayout.Get());
}

//-----------------------------------------------------------------------------
// 描画終了
//-----------------------------------------------------------------------------
void SpriteShader::End()
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_spRendererStatus == nullptr) return;

	m_graphicsDevice->m_spRendererStatus->SetDepthStencil(true, true);
	m_graphicsDevice->m_spRendererStatus->SetSampler(SS_FilterMode::Aniso, SS_AddressMode::Wrap);
	m_graphicsDevice->m_spRendererStatus->SetRasterize(RS_CullMode::Back, RS_FillMode::Solid);
}

//-----------------------------------------------------------------------------
// テクスチャ描画
//-----------------------------------------------------------------------------
void SpriteShader::DrawTexture(const Texture* texture, Vector2Int pos)
{
	
}
