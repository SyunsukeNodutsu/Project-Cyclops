#include "GrassShader.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GrassShader::GrassShader()
    : m_cpVS(nullptr)
    , m_cpPS(nullptr)
    , m_cpDS(nullptr)
    , m_cpHS(nullptr)
    , m_cpInputLayout(nullptr)
    , m_vertices()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool GrassShader::Initialize()
{
	//頂点シェーダ
	{
		#include "GrassShader_VS.shaderinc"

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
		#include "GrassShader_PS.shaderinc"

		if (FAILED(m_graphicsDevice->m_cpDevice.Get()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpPS.GetAddressOf())))
		{
			Debug::LogError("ピクセルシェーダー作成失敗."); return false;
		}
	}

    return true;
}

//-----------------------------------------------------------------------------
// 開始
//-----------------------------------------------------------------------------
void GrassShader::Begin()
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpContext == nullptr) return;

	m_graphicsDevice->m_cpContext->VSSetShader(m_cpVS.Get(), nullptr, 0);
	m_graphicsDevice->m_cpContext->PSSetShader(m_cpPS.Get(), nullptr, 0);
	m_graphicsDevice->m_cpContext->HSSetShader(m_cpHS.Get(), nullptr, 0);
	m_graphicsDevice->m_cpContext->DSSetShader(m_cpDS.Get(), nullptr, 0);

	m_graphicsDevice->m_cpContext->IASetInputLayout(m_cpInputLayout.Get());

	m_graphicsDevice->DrawVertices(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST, 4, m_vertices.data(), sizeof(Vertex));
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void GrassShader::End()
{

}
