//-----------------------------------------------------------------------------
// File: PostProcessShader.h
//
// 加工シェーダー
//-----------------------------------------------------------------------------
#pragma once

//高輝度抽出用のTexture
struct BlurTexture : public GraphicsDeviceChild
{
	void Create(int width, int height, bool useMSAA);
	std::shared_ptr<Texture> m_rt[5][2];
};

//加工シェーダー
class PostProcessShader : public GraphicsDeviceChild
{
	//頂点単位
	struct Vertex
	{
		Vector3 m_position;
		Vector2 m_uv;
	};

	//正射影用 頂点定義
	Vertex vOrthographic[4] = {
		{ Vector3(-1,-1, 0), Vector2(0, 1) },
		{ Vector3(-1, 1, 0), Vector2(0, 0) },
		{ Vector3( 1,-1, 0), Vector2(1, 1) },
		{ Vector3( 1, 1, 0), Vector2(1, 0) },
	};

	//ブラー用 定数バッファ
	struct cbBlur
	{
		Vector4 offset[31];
	};

public:

	PostProcessShader();
	~PostProcessShader() = default;

	bool Initialize();

	void DrawColor(Texture* texture);
	void BlurDraw(Texture* texture, Vector2 dir);
	void GenerateBlur(BlurTexture& blurTex, Texture* srcTex);
	void BrightFiltering(Texture* destRT, Texture* srcTex);

private:

	ComPtr<ID3D11VertexShader>	m_cpVS;
	ComPtr<ID3D11InputLayout>	m_cpInputLayout;

	ComPtr<ID3D11PixelShader>	m_colorPS;
	ComPtr<ID3D11PixelShader>	m_blurPS;
	ComPtr<ID3D11PixelShader>	m_HBrightPS;

	ConstantBuffer<cbBlur>		m_cb8Blur;

};
