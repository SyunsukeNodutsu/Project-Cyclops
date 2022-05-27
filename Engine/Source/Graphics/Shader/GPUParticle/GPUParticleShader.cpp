#include "GPUParticleShader.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GPUParticleShader::GPUParticleShader()
	: m_cpVS(nullptr)
	, m_cpPS(nullptr)
	, m_cpGS(nullptr)
	, m_cpCS(nullptr)
	, m_cpInputLayout(nullptr)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool GPUParticleShader::Initialize()
{
	//頂点シェーダ
	{
		#include "GPUParticle_VS.shaderinc"

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
		#include "GPUParticle_PS.shaderinc"

		if (FAILED(m_graphicsDevice->m_cpDevice.Get()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpPS.GetAddressOf())))
		{
			Debug::LogError("ピクセルシェーダー作成失敗."); return false;
		}
	}

	//ジオメトリシェーダー
	{
		#include "GPUParticle_GS.shaderinc"

		if (FAILED(m_graphicsDevice->m_cpDevice.Get()->CreateGeometryShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpGS.GetAddressOf())))
		{
			Debug::LogError("ジオメトリシェーダー作成失敗."); return false;
		}
	}

	//計算シェーダ
	{
		#include "GPUParticle_CS.shaderinc"

		if (FAILED(m_graphicsDevice->m_cpDevice.Get()->CreateComputeShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpCS.GetAddressOf())))
		{
			Debug::LogError("ピクセルシェーダー作成失敗."); return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// 開始
//-----------------------------------------------------------------------------
void GPUParticleShader::Begin()
{
	if (m_graphicsDevice == nullptr) return;
	if(m_graphicsDevice->m_cpContext == nullptr) return;

	m_graphicsDevice->m_cpContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_graphicsDevice->m_cpContext->IASetInputLayout(m_cpInputLayout.Get());

	m_graphicsDevice->m_cpContext->VSSetShader(m_cpVS.Get(), 0, 0);
	m_graphicsDevice->m_cpContext->PSSetShader(m_cpPS.Get(), 0, 0);
	m_graphicsDevice->m_cpContext->GSSetShader(m_cpGS.Get(), 0, 0);
	m_graphicsDevice->m_cpContext->CSSetShader(m_cpCS.Get(), 0, 0);
}
