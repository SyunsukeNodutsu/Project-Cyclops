#include "LensGhostShader.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
LensGhostShader::LensGhostShader()
	: m_cpVS(nullptr)
	, m_cpPS(nullptr)
	, m_cpInputLayout(nullptr)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool LensGhostShader::Initialize()
{
	//頂点シェーダ
	{
		#include "LensGhostShader_VS.shaderinc"

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
		#include "LensGhostShader_PS.shaderinc"

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
void LensGhostShader::Begin()
{

}
