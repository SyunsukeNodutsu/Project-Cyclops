#include "UnlitShader.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
UnlitShader::UnlitShader()
	: m_cpVS(nullptr)
	, m_cpPS(nullptr)
	, m_cpInputLayout(nullptr)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool UnlitShader::Initialize()
{
	if (m_graphicsDevice == nullptr) return false;
	if (m_graphicsDevice->m_cpContext == nullptr) return false;

	//頂点シェーダ
	{
		#include "UnlitShader_VS.shaderinc"

		if (FAILED(m_graphicsDevice->m_cpDevice->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpVS.GetAddressOf())))
		{
			Debug::LogError("頂点シェーダー作成失敗."); return false;
		}

		//頂点インプットレイアウト作成
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		if (FAILED(m_graphicsDevice->m_cpDevice->CreateInputLayout(&layout[0], (UINT)layout.size(), compiledBuffer, sizeof(compiledBuffer), m_cpInputLayout.GetAddressOf())))
		{
			Debug::LogError("CreateInputLayout失敗."); return false;
		}
	}

	//ピクセルシェーダ
	{
		#include "UnlitShader_PS.shaderinc"

		if (FAILED(m_graphicsDevice->m_cpDevice->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpPS.GetAddressOf())))
		{
			Debug::LogError("ピクセルシェーダー作成失敗."); return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// 開始
//-----------------------------------------------------------------------------
void UnlitShader::Begin()
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpContext == nullptr) return;

	m_graphicsDevice->m_cpContext->VSSetShader(m_cpVS.Get(), 0, 0);
	m_graphicsDevice->m_cpContext->IASetInputLayout(m_cpInputLayout.Get());

	m_graphicsDevice->m_cpContext->PSSetShader(m_cpPS.Get(), 0, 0);
}

//-----------------------------------------------------------------------------
// 頂点一覧描画
//-----------------------------------------------------------------------------
void UnlitShader::DrawVertices(const std::vector<Vertex>& vertices, D3D_PRIMITIVE_TOPOLOGY topology)
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpContext == nullptr) return;

	//m_graphicsDevice->m_cpContext->IASetPrimitiveTopology(topology);
	m_graphicsDevice->DrawVertices(topology, (int)vertices.size(), &vertices[0], sizeof(Vertex));
}
