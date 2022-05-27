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
void ParticleSystem::Draw()
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_spRendererStatus == nullptr) return;

	m_graphicsDevice->m_spRendererStatus->SetRasterize(RS_CullMode::CullNone, RS_FillMode::Solid);
	m_graphicsDevice->m_spRendererStatus->SetBlend(BlendMode::Add);

	m_graphicsDevice->m_spShaderManager->m_GPUParticleShader.Begin();

	//エミッター分描画
	for (auto&& particle : m_spParticleList)
		particle->Draw();

	m_graphicsDevice->m_spRendererStatus->SetRasterize(RS_CullMode::Back, RS_FillMode::Solid);
	m_graphicsDevice->m_spRendererStatus->SetBlend(BlendMode::Alpha);
}

//-----------------------------------------------------------------------------
// 発生
//-----------------------------------------------------------------------------
void ParticleSystem::Emit(EmitData data, UINT numParticles, bool loop)
{
	const auto& particle = std::make_shared<ParticleWork>();
	if (particle)
	{
		particle->Emit(numParticles, data, loop);

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
	, m_loop(false)
	, m_generated(false)
	, m_generatedMutex()
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
	//TODO: エミッターを複数作成した場合に エミッター毎にDispatchするのはよくない
	{
		ID3D11ShaderResourceView* pSRVs[1] = { m_cpInputSRV.Get() };
		m_graphicsDevice->m_cpContext->CSSetShaderResources(0, 1, pSRVs);
		m_graphicsDevice->m_cpContext->CSSetShader(m_graphicsDevice->m_spShaderManager->m_GPUParticleShader.m_cpCS.Get(), 0, 0);
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
	m_lifeSpan -= static_cast<float>(FpsTimer::GetDeltaTime());
	if (m_lifeSpan <= 0)
	{
		if (m_loop) Emit(m_numParticles, m_data, m_loop);
		else End();
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
	//m_graphicsDevice->m_cpContext->PSSetShaderResources(0, 1, m_psTexture->SRVAddress());//テクスチャ/座標(SRV)
	m_graphicsDevice->m_cpContext->VSSetShaderResources(0, 1, m_cpPositionSRV.GetAddressOf());//UAVからの出力結果

	//インスタンシング描画
	m_graphicsDevice->m_cpContext->DrawInstanced(4, m_numParticles, 0, 0);

	//nullリソース設定(解除)
	ID3D11ShaderResourceView* resourceZero = nullptr;
	//m_graphicsDevice->m_cpContext->PSSetShaderResources(0, 1, &resourceZero);
	m_graphicsDevice->m_cpContext->VSSetShaderResources(0, 1, &resourceZero);
}

//-----------------------------------------------------------------------------
// 発生
//-----------------------------------------------------------------------------
void ParticleWork::Emit(UINT numParticles, ParticleSystem::EmitData data, bool loop)
{
	m_numParticles	= numParticles;
	m_data			= data;
	m_loop			= loop;
	m_lifeSpan		= data.m_maxLifeSpan;

	//TODO: テクスチャ

	SetupViews();

	SetGenerated(false);

	//非同期で粒子を生成
	std::thread([=] {
		//擬似乱数生成器の初期化
		std::random_device seed_gen;
		std::mt19937 engine(seed_gen());

		//一様実数分布
		//TOOD: コストがバカ高そう...発生に関しても計算シェーダーでよさそう
		//TOOD: 乱数生成装置をクラス化
		//TOOD: 最小値が最大値を上回ってないかの確認(expression invalid min max arguments for uniform_real)
		std::uniform_real_distribution<float> distr_pos_x(data.m_minPosition.x, data.m_maxPosition.x);
		std::uniform_real_distribution<float> distr_pos_y(data.m_minPosition.y, data.m_maxPosition.y);
		std::uniform_real_distribution<float> distr_pos_z(data.m_minPosition.z, data.m_maxPosition.z);

		std::uniform_real_distribution<float> distr_vel_x(data.m_minVelocity.x, data.m_maxVelocity.x);
		std::uniform_real_distribution<float> distr_vel_y(data.m_minVelocity.y, data.m_maxVelocity.y);
		std::uniform_real_distribution<float> distr_vel_z(data.m_minVelocity.z, data.m_maxVelocity.z);

		std::uniform_real_distribution<float> distr_life(data.m_minLifeSpan, data.m_maxLifeSpan);
		std::uniform_real_distribution<float> distr_col(0.0f, 1.0f);

		//粒子生成
		//TODO: アロケータ自作しないとまずい
		m_pParticle = new Particle[m_numParticles];
		for (int i = 0; i < m_numParticles; i++)
		{
			m_pParticle[i].m_position		= Vector3(distr_pos_x(engine), distr_pos_y(engine), distr_pos_z(engine));
			m_pParticle[i].m_velocity		= Vector3(distr_vel_x(engine), distr_vel_y(engine), distr_vel_z(engine));
			m_pParticle[i].m_lifeSpan		= distr_life(engine);
			//m_pParticle[i].m_color			= data.m_color;
			m_pParticle[i].m_color			= Vector4(distr_col(engine), distr_col(engine), distr_col(engine), 1);
			m_pParticle[i].m_lifeSpanMax	= m_pParticle[i].m_lifeSpan;
		}

		SetGenerated(true);

		}).detach();
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
// 各ビューの初期設定
//-----------------------------------------------------------------------------
void ParticleWork::SetupViews()
{
	if (m_graphicsDevice == nullptr) return;

	m_spInputBuffer = std::make_shared<Buffer>();
	m_spPositionBuffer = std::make_shared<Buffer>();
	m_spResultBuffer = std::make_shared<Buffer>();

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
