#include "ParticleSystem.h"

int ParticleSystem::m_numParticle = 0;

//=============================================================================
// 
// ParticleSystem
// 
//=============================================================================

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ParticleSystem::ParticleSystem()
	: m_spParticleList()
{
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void ParticleSystem::Update()
{
	for (auto itr = m_spParticleList.begin(); itr != m_spParticleList.end();)
	{
		if ((*itr) == nullptr) continue;

		//更新
		(*itr)->Update();

		//除外
		if ((*itr)->IsEnd()) itr = m_spParticleList.erase(itr);
		else ++itr;
	}
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void ParticleSystem::Draw(const bool add)
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_spRendererStatus == nullptr) return;

	m_graphicsDevice->m_spRendererStatus->SetRasterize(RS_CullMode::CullNone, RS_FillMode::Solid);
	if (add) m_graphicsDevice->m_spRendererStatus->SetBlend(BlendMode::Add);
	else m_graphicsDevice->m_spRendererStatus->SetBlend(BlendMode::Alpha);
	
	//エミッター分描画
	for (auto&& particle : m_spParticleList)
		particle->Draw();

	m_graphicsDevice->m_spRendererStatus->SetRasterize(RS_CullMode::Back, RS_FillMode::Solid);
	m_graphicsDevice->m_spRendererStatus->SetBlend(BlendMode::Alpha);
}

//-----------------------------------------------------------------------------
// 発生
//-----------------------------------------------------------------------------
void ParticleSystem::Emit(EmitData data, UINT numParticles, std::shared_ptr<Texture> pTexture, bool loop)
{
	const auto& particle = std::make_shared<ParticleWork>();
	if (particle)
	{
		//テクスチャがnullの場合は1x1の白テクスチャを使用
		if (pTexture == nullptr)
			pTexture = m_graphicsDevice->GetWhiteTex();

		particle->Emit(numParticles, data, pTexture, loop);

		//描画順の関係上 先頭に追加する必要がある
		m_spParticleList.push_front(particle);

		m_numParticle += numParticles;
	}
}

//-----------------------------------------------------------------------------
// 全パーティクルシミュレーション終了
//-----------------------------------------------------------------------------
void ParticleSystem::AllEnd()
{
	for (auto&& particle : m_spParticleList)
		particle->End();

	m_spParticleList.clear();
}



//=============================================================================
// 
// ParticleWork
// 
//=============================================================================

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ParticleWork::ParticleWork()
	: m_pParticle(nullptr)
	, m_data()
	, m_numParticles(0)
	, m_lifeSpan(0.0f)
	, m_isLoop(false)
	, m_generated(false)
	, m_generatedMutex()
	, m_spTexture(nullptr)
	, m_spInputBuffer(nullptr)
	, m_spResultBuffer(nullptr)
	, m_spPositionBuffer(nullptr)
	, m_cpInputSRV(nullptr)
	, m_cpPositionSRV(nullptr)
	, m_cpResultUAV(nullptr)
{
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void ParticleWork::Update()
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpContext == nullptr) return;

	if (!IsGenerated()) return;
	if (m_pParticle == nullptr) return;

	//粒子シミュレーション用データの書き込み
	m_spInputBuffer->WriteData(m_pParticle, sizeof(Particle) * m_numParticles);

	//コンピュートシェーダー実行/粒子のシュミレーション
	{
		ID3D11ShaderResourceView* pSRVs[1] = { m_cpInputSRV.Get() };
		m_graphicsDevice->m_cpContext->CSSetShaderResources(0, 1, pSRVs);
		m_graphicsDevice->m_cpContext->CSSetUnorderedAccessViews(0, 1, m_cpResultUAV.GetAddressOf(), 0);
		m_graphicsDevice->m_cpContext->Dispatch(256, 1, 1);//X次元256でディスパッチ

		//nullリソース設定(解除)
		ID3D11ShaderResourceView* resource_zero = nullptr;
		m_graphicsDevice->m_cpContext->CSSetShaderResources(0, 1, &resource_zero);
	}

	//シミュレーション結果データ バインド
	{
		//CPUアクセス変更(読み込み) -> 結果コピー
		ID3D11Buffer* pResultBufCpy = Buffer::CreateAndCopyToDebugBuf(m_spResultBuffer->Get());

		D3D11_MAPPED_SUBRESOURCE pData;
		if (SUCCEEDED(m_graphicsDevice->m_cpContext->Map(pResultBufCpy, 0, D3D11_MAP_READ, 0, &pData)))
		{
			size_t size = sizeof(Particle) * m_numParticles;
			memcpy_s(m_pParticle, size, pData.pData, size);
			m_graphicsDevice->m_cpContext->Unmap(pResultBufCpy, 0);
			pResultBufCpy->Release();
		}
	}

	//座標データバインド ※頂点シェーダーで使用
	m_spPositionBuffer->WriteData(m_pParticle, sizeof(Particle) * m_numParticles);

	//生存期間の確認
	m_lifeSpan -= FpsTimer::GetDeltaTime<float>();
	if (m_lifeSpan <= 0)
	{
		//ループ処理毎に生成はコスト高 ->シェーダー側でループ処理
		if (!m_isLoop)
			End();
	}
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void ParticleWork::Draw()
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpContext == nullptr) return;

	if (!IsGenerated()) return;
	if (m_pParticle == nullptr) return;

	//リソース設定
	m_graphicsDevice->m_cpContext->PSSetShaderResources(0, 1, m_spTexture->SRVAddress());//テクスチャ/座標(SRV)
	m_graphicsDevice->m_cpContext->VSSetShaderResources(0, 1, m_cpPositionSRV.GetAddressOf());//UAVからの出力結果

	//インスタンシング描画
	m_graphicsDevice->m_cpContext->DrawInstanced(4, m_numParticles, 0, 0);

	//nullリソース設定(解除)
	ID3D11ShaderResourceView* resource_zero = nullptr;
	m_graphicsDevice->m_cpContext->PSSetShaderResources(0, 1, &resource_zero);
	m_graphicsDevice->m_cpContext->VSSetShaderResources(0, 1, &resource_zero);
}

//-----------------------------------------------------------------------------
// 発生
//-----------------------------------------------------------------------------
void ParticleWork::Emit(UINT numParticles, ParticleSystem::EmitData data, std::shared_ptr<Texture> pTexture, bool loop)
{
	if (pTexture == nullptr) return;

	m_numParticles	= numParticles;
	m_data			= data;
	m_isLoop		= loop;
	m_lifeSpan		= data.m_maxLifeSpan;

	m_spTexture = pTexture;

	SetupViews();
	EmitAsync();
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void ParticleWork::End()
{
	for (;;)
	{
		if (!IsGenerated()) continue;

		if (m_pParticle != nullptr)
		{
			delete[] m_pParticle;
			m_pParticle = nullptr;

			//TODO: UAVでLifeSpanが0になった数を詳細に取得し計測
			ParticleSystem::m_numParticle -= m_numParticles;
		}
		break;
	}
}

//-----------------------------------------------------------------------------
// 非同期生成
//-----------------------------------------------------------------------------
void ParticleWork::EmitAsync()
{
	SetGenerated(false);

	std::thread([=]{

		m_pParticle = new Particle[m_numParticles];

		for (int i = 0; i < m_numParticles; i++)
		{
			float posX = RandomSystem::GetRandomReal<float>(m_data.m_minPosition.x, m_data.m_maxPosition.x);
			float posY = RandomSystem::GetRandomReal<float>(m_data.m_minPosition.y, m_data.m_maxPosition.y);
			float posZ = RandomSystem::GetRandomReal<float>(m_data.m_minPosition.z, m_data.m_maxPosition.z);

			float velX = RandomSystem::GetRandomReal<float>(m_data.m_minVelocity.x, m_data.m_maxVelocity.x);
			float velY = RandomSystem::GetRandomReal<float>(m_data.m_minVelocity.y, m_data.m_maxVelocity.y);
			float velZ = RandomSystem::GetRandomReal<float>(m_data.m_minVelocity.z, m_data.m_maxVelocity.z);

			float life = RandomSystem::GetRandomReal<float>(m_data.m_minLifeSpan, m_data.m_maxLifeSpan);

			float colR = RandomSystem::GetRandomReal<float>(0.0f, 1.0f);
			float colG = RandomSystem::GetRandomReal<float>(0.0f, 1.0f);
			float colB = RandomSystem::GetRandomReal<float>(0.0f, 1.0f);

			m_pParticle[i].m_position = float3(posX, posY, posZ);
			m_pParticle[i].m_velocity = float3(velX, velY, velZ);
			m_pParticle[i].m_lifeSpan = life;
			//m_pParticle[i].m_color = m_data.m_color;
			m_pParticle[i].m_color = float4(colR, colG, colB, 1.0f);
			m_pParticle[i].m_lifeSpanMax = m_pParticle[i].m_lifeSpan;
			m_pParticle[i].m_loop = m_isLoop;
		}

		SetGenerated(true);

		}).detach();
}

//-----------------------------------------------------------------------------
// 各ビューの初期設定
//-----------------------------------------------------------------------------
void ParticleWork::SetupViews()
{
	if (m_graphicsDevice == nullptr) return;

	m_spInputBuffer		= std::make_shared<Buffer>();
	m_spPositionBuffer	= std::make_shared<Buffer>();
	m_spResultBuffer	= std::make_shared<Buffer>();

	//シミュレーション入力データ用バッファーの作成
	if (!m_spInputBuffer->CreateStructured(sizeof(Particle), static_cast<UINT>(m_numParticles), false, nullptr))
	{
		Debug::LogError("CreateStructured失敗."); return;
	}

	//計算シェーダからの結果受け取り用バッファーの作成
	if (!m_spResultBuffer->CreateStructured(sizeof(Particle), static_cast<UINT>(m_numParticles), true, nullptr))
	{
		Debug::LogError("CreateStructured失敗."); return;
	}

	//計算結果から座標を取得してそれを入力するバッファーの作成
	if (!m_spPositionBuffer->CreateStructured(sizeof(Particle), static_cast<UINT>(m_numParticles), false, nullptr))
	{
		Debug::LogError("CreateStructured失敗."); return;
	}

	//SRVの生成
	if (FAILED(m_graphicsDevice->CreateBufferSRV(m_spInputBuffer->Get(), m_cpInputSRV.GetAddressOf())))
	{
		Debug::LogError("CreateBufferSRV失敗."); return;
	}
	if (FAILED(m_graphicsDevice->CreateBufferSRV(m_spPositionBuffer->Get(), m_cpPositionSRV.GetAddressOf())))
	{
		Debug::LogError("CreateBufferSRV失敗."); return;
	}

	//UAVの生成
	if (FAILED(m_graphicsDevice->CreateBufferUAV(m_spResultBuffer->Get(), m_cpResultUAV.GetAddressOf())))
	{
		Debug::LogError("CreateBufferUAV失敗."); return;
	}
}
