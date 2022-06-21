//-----------------------------------------------------------------------------
// File: ParticleSystem.h
//
// 粒子シミュレーションシステム
// 
// TODO: エミッターを複数作成した場合に エミッター毎にDispatchするのはよくない
// TOOD: コストがバカ高そう(発生に関しても計算シェーダー,アロケータ自作)
//-----------------------------------------------------------------------------
#pragma once

class ParticleWork;

//粒子シミュレーションシステムクラス
class ParticleSystem : public GraphicsDeviceChild
{
public:

	//Emitの際に設定する初期情報
	struct EmitData
	{
		float3 m_maxPosition = float3::Zero;
		float3 m_minPosition = float3::Zero;
		float3 m_maxVelocity = float3::Zero;
		float3 m_minVelocity = float3::Zero;
		float m_maxLifeSpan = 0.0f;
		float m_minLifeSpan = 0.0f;
		float4 m_color = float4::Zero;
	};

public:

	ParticleSystem();
	~ParticleSystem() = default;

	void Update();
	void Draw(const bool add);

	void Emit(EmitData data, UINT numParticles, std::shared_ptr<Texture> pTexture, bool loop = false);
	void AllEnd();

	static int m_numParticle;

private:

	std::list<std::shared_ptr<ParticleWork>> m_spParticleList;

};

//エミッター単位
class ParticleWork : public GraphicsDeviceChild
{
	//粒子単位 定数バッファ(SRV)
	struct Particle
	{
		float3 m_position;
		float	m_lifeSpanMax;
		float3 m_velocity;
		float	m_lifeSpan;
		float4 m_color;
		float m_loop;
		float tmp[3];
	};

public:

	ParticleWork();
	~ParticleWork() { End(); }

	void Update();
	void Draw();

	void Emit(UINT numParticles, ParticleSystem::EmitData data, std::shared_ptr<Texture> pTexture, bool loop = false);
	void End();

	bool IsEnd() { return (m_pParticle == nullptr && IsGenerated()); }

private:

	Particle*							m_pParticle;		//粒子情報(GPUとのやり取りを行う)
	ParticleSystem::EmitData			m_data;
	int									m_numParticles;		//粒子の数
	float								m_lifeSpan;
	bool								m_isLoop;
	bool								m_generated;		//生成完了
	std::mutex							m_generatedMutex;

	std::shared_ptr<Texture>			m_spTexture;		//テクスチャ

	std::shared_ptr<Buffer>				m_spInputBuffer;	//シミュレーション準備データ(入力SRV)
	std::shared_ptr<Buffer>				m_spResultBuffer;	//シミュレーション結果データ(出力UAV)
	std::shared_ptr<Buffer>				m_spPositionBuffer;

	ComPtr<ID3D11ShaderResourceView>	m_cpInputSRV;
	ComPtr<ID3D11ShaderResourceView>	m_cpPositionSRV;
	ComPtr<ID3D11UnorderedAccessView>	m_cpResultUAV;

private:

	void EmitAsync();
	void SetupViews();//ビューはthreadsafeではないのでデバイス作成元スレッドで行う
	void SetGenerated(bool done) { std::lock_guard<std::mutex> lock(m_generatedMutex); m_generated = done; }
	bool IsGenerated() { std::lock_guard<std::mutex> lock(m_generatedMutex); return m_generated; }

};
