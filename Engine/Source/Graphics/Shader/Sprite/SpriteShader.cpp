#include "SpriteShader.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
SpriteShader::SpriteShader()
	: m_cpVS(nullptr)
	, m_cpPS(nullptr)
	, m_cpInputLayout(nullptr)
	, m_cb0Sprite()
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
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
	if (m_cb0Sprite.Create())
	{
		m_cb0Sprite.SetToDevice(2, SHADER_STAGE::VS);
		m_cb0Sprite.SetToDevice(2, SHADER_STAGE::PS);
		m_cb0Sprite.Work().m_color = Vector4(0, 0, 0, 0);
		m_cb0Sprite.Write();
	}
	else { Debug::Log("定数バッファ(Sprite)作成失敗."); return false; }

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
void SpriteShader::DrawTexture(const Texture* pTexture, Vector2 pos, Vector2 pivot)
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpContext == nullptr) return;
	if (m_graphicsDevice->m_spRendererStatus == nullptr) return;

	if (pTexture == nullptr) return;
	if (!m_begin) { Debug::Log("エラー：Begin()が呼ばれていません."); return; }

	//解像度に応じて拡縮
	const float rateX = m_graphicsDevice->m_viewport.Width / 1280;
	const float rateY = m_graphicsDevice->m_viewport.Height / 720;
	Matrix world; world.CreateScalling(rateX, rateY, 0);

	m_graphicsDevice->m_spRendererStatus->m_cb4Behaviour.Work().m_worldMatrix = world;
	m_graphicsDevice->m_spRendererStatus->m_cb4Behaviour.Write();

	m_graphicsDevice->m_cpContext->VSSetShaderResources(0, 1, pTexture->SRVAddress());
	m_graphicsDevice->m_cpContext->PSSetShaderResources(0, 1, pTexture->SRVAddress());

	SetVertex(pTexture, pos, pivot);

	m_graphicsDevice->DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, m_vertices.data(), sizeof(Vertex));

	//TODO: 関数化
	ID3D11ShaderResourceView* resource_zero = nullptr;
	m_graphicsDevice->m_cpContext->VSSetShaderResources(0, 1, &resource_zero);
	m_graphicsDevice->m_cpContext->PSSetShaderResources(0, 1, &resource_zero);
}

//-----------------------------------------------------------------------------
// 頂点情報設定
//-----------------------------------------------------------------------------
void SpriteShader::SetVertex(const Texture* pTexture, Vector2 pos, Vector2 pivot)
{
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
	m_vertices[0] = Vertex(Vector3(x_01, y_01, 0), Vector2(0, 1));
	m_vertices[1] = Vertex(Vector3(x_01, y_02, 0), Vector2(0, 0));
	m_vertices[2] = Vertex(Vector3(x_02, y_01, 0), Vector2(1, 1));
	m_vertices[3] = Vertex(Vector3(x_02, y_02, 0), Vector2(1, 0));
}
