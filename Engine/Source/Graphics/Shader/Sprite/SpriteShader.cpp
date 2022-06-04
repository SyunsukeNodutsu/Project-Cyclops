#include "SpriteShader.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
SpriteShader::SpriteShader()
	: m_cpVS(nullptr)
	, m_cpPS(nullptr)
	, m_cpInputLayout(nullptr)
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
			Debug::LogError("頂点シェーダー作成失敗."); return false;
		}

		//頂点インプットレイアウト作成
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		if (FAILED(m_graphicsDevice->m_cpDevice.Get()->CreateInputLayout(&layout[0], (UINT)layout.size(), compiledBuffer, sizeof(compiledBuffer), m_cpInputLayout.GetAddressOf())))
		{
			Debug::LogError("CreateInputLayout失敗."); return false;
		}
	}

	//ピクセルシェーダ
	{
		#include "SpriteShader_PS.shaderinc"

		if (FAILED(m_graphicsDevice->m_cpDevice.Get()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpPS.GetAddressOf())))
		{
			Debug::LogError("ピクセルシェーダー作成失敗."); return false;
		}
	}

	if (m_cbColor0.Create())
	{
		m_cbColor0.SetToDevice(0, SHADER_STAGE::PS);
	}
	else { Debug::LogError("定数バッファ(Color)作成失敗."); return false; }

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

	if (m_begin) return;
	m_begin = true;

	m_prevProjMatrix = m_graphicsDevice->m_spRendererStatus->m_cb5Camera.Get().m_projMatrix;

	//正射影行列をGPUに転送
	const auto& orthographic = DirectX::XMMatrixOrthographicLH(m_graphicsDevice->m_viewport.Width, m_graphicsDevice->m_viewport.Height, 0, 1);
	m_graphicsDevice->m_spRendererStatus->m_cb5Camera.Work().m_projMatrix = orthographic;
	m_graphicsDevice->m_spRendererStatus->m_cb5Camera.Write();

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

	if (!m_begin) return;
	m_begin = false;

	//射影行列を復元
	m_graphicsDevice->m_spRendererStatus->m_cb5Camera.Work().m_projMatrix = m_prevProjMatrix;
	m_graphicsDevice->m_spRendererStatus->m_cb5Camera.Write();

	//ステートを復元
	m_graphicsDevice->m_spRendererStatus->SetDepthStencil(true, true);
	m_graphicsDevice->m_spRendererStatus->SetSampler(SS_FilterMode::Aniso, SS_AddressMode::Wrap);
	m_graphicsDevice->m_spRendererStatus->SetRasterize(RS_CullMode::Back, RS_FillMode::Solid);
}

//-----------------------------------------------------------------------------
// テクスチャ描画
//-----------------------------------------------------------------------------
void SpriteShader::DrawTexture(const Texture* pTexture, float2 pos, float2 size, float2 pivot)
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpContext == nullptr) return;
	if (m_graphicsDevice->m_spRendererStatus == nullptr) return;

	if (pTexture == nullptr) return;
	if (!m_begin) { Debug::LogError("Begin()が呼ばれていません."); return; }

	//基準とするウィンドウのサイズ
	static constexpr float window_w = 1600;
	static constexpr float window_h = 900;

	//解像度に応じて拡縮
	const float rateX = m_graphicsDevice->m_viewport.Width  / window_w * size.x;
	const float rateY = m_graphicsDevice->m_viewport.Height / window_h * size.y;
	m_graphicsDevice->m_spRendererStatus->m_cb4Behaviour.Work().m_worldMatrix = matrix4x4::CreateScale(rateX, rateY, 1);
	m_graphicsDevice->m_spRendererStatus->m_cb4Behaviour.Write();

	m_graphicsDevice->m_cpContext->VSSetShaderResources(0, 1, pTexture->SRVAddress());
	m_graphicsDevice->m_cpContext->PSSetShaderResources(0, 1, pTexture->SRVAddress());

	SetVertices(pTexture, pos, pivot);

	m_cbColor0.Work().m_color = color4(1, 1, 1, 1);
	m_cbColor0.Write();

	m_graphicsDevice->DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, m_vertices.data(), sizeof(Vertex));

	//TODO: 関数化
	ID3D11ShaderResourceView* resource_zero = nullptr;
	m_graphicsDevice->m_cpContext->VSSetShaderResources(0, 1, &resource_zero);
	m_graphicsDevice->m_cpContext->PSSetShaderResources(0, 1, &resource_zero);
}

//-----------------------------------------------------------------------------
// 2D線を描画
//-----------------------------------------------------------------------------
void SpriteShader::DrawLine(const float2 pos1, const float2 pos2, const color4 color)
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpContext == nullptr) return;

	if (!m_begin) { Debug::LogError("Begin()が呼ばれていません."); return; }

	//アルファテスト回避
	m_graphicsDevice->m_cpContext->VSSetShaderResources(0, 1, m_graphicsDevice->GetWhiteTex()->SRVAddress());
	m_graphicsDevice->m_cpContext->PSSetShaderResources(0, 1, m_graphicsDevice->GetWhiteTex()->SRVAddress());

	m_graphicsDevice->m_spRendererStatus->m_cb4Behaviour.Work().m_worldMatrix = matrix4x4::CreateScale(1, 1, 1);
	m_graphicsDevice->m_spRendererStatus->m_cb4Behaviour.Write();

	m_cbColor0.Work().m_color = color;
	m_cbColor0.Write();

	Vertex vertex[] = {
		{ {pos1.x, pos1.y, 0},	{0, 0} },
		{ {pos2.x, pos2.y, 0},	{1, 0} },
	};

	m_graphicsDevice->DrawVertices(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, 2, vertex, sizeof(Vertex));

	ID3D11ShaderResourceView* resource_zero = nullptr;
	m_graphicsDevice->m_cpContext->VSSetShaderResources(0, 1, &resource_zero);
	m_graphicsDevice->m_cpContext->PSSetShaderResources(0, 1, &resource_zero);
}

//-----------------------------------------------------------------------------
// 頂点情報設定
//-----------------------------------------------------------------------------
void SpriteShader::SetVertices(const Texture* pTexture, float2 pos, float2 pivot)
{
	pivot.x = std::clamp(pivot.x, 0.0f, 1.0f);
	pivot.y = std::clamp(pivot.y, 0.0f, 1.0f);

	const float width	= static_cast<float>(pTexture->GetWidth());
	const float height	= static_cast<float>(pTexture->GetHeight());

	float x_01 = pos.x;
	float y_01 = pos.y;
	float x_02 = pos.x + width;
	float y_02 = pos.y + height;

	//基準点
	x_01 -= pivot.x * width;
	x_02 -= pivot.x * width;
	y_01 -= pivot.y * height;
	y_02 -= pivot.y * height;

	//左上 -> 右上 -> 左下 -> 右下
	m_vertices[0] = Vertex(float3(x_01, y_01, 0), float2(0, 1));
	m_vertices[1] = Vertex(float3(x_01, y_02, 0), float2(0, 0));
	m_vertices[2] = Vertex(float3(x_02, y_01, 0), float2(1, 1));
	m_vertices[3] = Vertex(float3(x_02, y_02, 0), float2(1, 0));
}
