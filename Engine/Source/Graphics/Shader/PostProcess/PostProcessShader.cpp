#include "PostProcessShader.h"

//-----------------------------------------------------------------------------
// 
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
// 
//-----------------------------------------------------------------------------
PostProcessShader::PostProcessShader()
	: m_cpVS(nullptr)
	, m_cpInputLayout(nullptr)
	, m_colorPS(nullptr)
	, m_blurPS(nullptr)
	, m_HBrightPS(nullptr)
	, m_cb0Blur()
{
}

//-----------------------------------------------------------------------------
// 
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

    return false;
}

//-----------------------------------------------------------------------------
// 
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
// 
//-----------------------------------------------------------------------------
void PostProcessShader::BlurDraw(Texture* texture, Vector2 dir)
{

}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void PostProcessShader::GenerateBlur(BlurTexture& blurTex, Texture* srcTex)
{

}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void PostProcessShader::BrightFiltering(Texture* destRT, Texture* srcTex)
{

}
