#include "GameDemo.h"

Window*			GameDemo::m_pWindow			= nullptr;
FpsTimer*		GameDemo::m_pFpsTimer		= nullptr;

GraphicsDevice* GameDemo::m_pGraphicsDevice = nullptr;
AudioDevice*	GameDemo::m_pAudioDevice	= nullptr;
ImGuiProfile*	GameDemo::m_pImGuiProfile	= nullptr;

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GameDemo::GameDemo()
	: m_windowWidth(1600)
	, m_windowHeight(900)
	, m_spTexture(nullptr)
	, m_profile()
	, m_camera()
{
}

//-----------------------------------------------------------------------------
// 実行大本
//-----------------------------------------------------------------------------
void GameDemo::Run()
{
	Initialize();

	while (true)
	{
		if (!m_pWindow->ProcessMessage()) break;

		Update();
		Draw();
		LateUpdate();
	}

	Finalize();
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void GameDemo::Initialize()
{
	//ウィンドウ作成
	WINDOE_CREATE_PARAM window_param = {
		.TitleName		= L"Project Cyclops",
		.ClassName		= L"ClassName",
		.ClientWidth	= m_windowWidth,
		.ClientHeight	= m_windowHeight,
	};
	m_pWindow = new Window(window_param);
	m_pWindow->Initialize();
	//サブシステムインターフェースに単一のHWNDを設定
	ISubsystem::SetHWND(m_pWindow->GetHwnd());

	m_pFpsTimer = new FpsTimer();
	
	//サブシステムを適切な順番で生成 -------------------------------->
	//いずれこの一連の初期化はエンジンが提供します
	m_profile.Start("サブシステム起動");
	GRAPHICS_DEVICE_CREATE_PARAM device_param = {
		.BufferCount	= 2,
		.Width			= m_windowWidth,
		.Height			= m_windowHeight,
		.RefreshRate	= 0,
		.Windowed		= true,
		.UseHDR			= false,
		.UseMSAA		= false,
		.DebugMode		= true,
	};
	m_pGraphicsDevice = new GraphicsDevice(device_param);
	m_pAudioDevice = new AudioDevice();
	m_pImGuiProfile = new ImGuiProfile();

	GraphicsDeviceChild::SetDevice(m_pGraphicsDevice);
	AudioDeviceChild::SetDevice(m_pAudioDevice);

	m_pGraphicsDevice->Initialize();
	m_pAudioDevice->Initialize();
	m_pImGuiProfile->Initialize();

	m_pImGuiProfile->SetWindow(m_pWindow);
	m_pImGuiProfile->SetFpsTimer(m_pFpsTimer);
	m_pImGuiProfile->SetGraphicsDevice(m_pGraphicsDevice);
	m_pImGuiProfile->SetAudioDevice(m_pAudioDevice);

	m_profile.End();
	//<-------------------------サブシステムを適切な順番で生成ここまで

	//初期カメラ
	m_camera.m_name = "Camera Main";
	m_camera.m_priority = 10.0f;
	m_camera.SetCameraMatrix(Matrix::CreateTranslation(Vector3(0, 0, -3)));

	m_profile.Start("テクスチャ読み込み");
	m_spTexture = std::make_shared<Texture>();
	m_spTexture->Load(L"Assets/test2.jpg");
	m_profile.End();

	//エミッターデータ
	ParticleSystem::EmitData m_emitData;
	m_emitData.m_minPosition = Vector3::Zero;
	m_emitData.m_maxPosition = Vector3::Zero;

	const float vel = 2.0f;
	m_emitData.m_minVelocity = Vector3(-vel, -vel, -vel);
	m_emitData.m_maxVelocity = Vector3(vel, vel, vel);

	m_emitData.m_minLifeSpan = 1.0f;
	m_emitData.m_maxLifeSpan = 6.0f;
	m_emitData.m_color = Vector4(1, 0.5f, 0, 1);

	const auto& texture = std::make_shared<Texture>();
	texture->Load(L"Assets/test.png");

	m_pGraphicsDevice->m_spParticleSystem->Emit(m_emitData, 100000, texture, true);
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void GameDemo::Update()
{
	m_pFpsTimer->Tick();

	m_pGraphicsDevice->m_spRendererStatus->m_cb7Time.Work().m_deltaTime = (float)FpsTimer::GetDeltaTime();
	m_pGraphicsDevice->m_spRendererStatus->m_cb7Time.Work().m_totalTime = (float)FpsTimer::GetTotalTime();
	m_pGraphicsDevice->m_spRendererStatus->m_cb7Time.Write();

	static Vector3 pos = m_camera.GetCameraMatrix().GetTranslation();
	static constexpr float move_pow = 10.0f;

	if (Input::IsKeyPressed(KeyCode::LeftArrow)) pos.x -= move_pow * (float)FpsTimer::GetDeltaTime();
	if (Input::IsKeyPressed(KeyCode::RightArrow)) pos.x += move_pow * (float)FpsTimer::GetDeltaTime();
	if (Input::IsKeyPressed(KeyCode::UpArrow)) pos.z += move_pow * (float)FpsTimer::GetDeltaTime();
	if (Input::IsKeyPressed(KeyCode::DownArrow)) pos.z -= move_pow * (float)FpsTimer::GetDeltaTime();

	Matrix trans = Matrix::CreateTranslation(pos);
	m_camera.SetCameraMatrix(trans);
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void GameDemo::Draw()
{
	static float x = 0.0f, y = 0.0f;

	m_camera.SetToShader();

	m_pGraphicsDevice->Begin();

	//3D
	{
		m_pGraphicsDevice->m_spShaderManager->m_GPUParticleShader.Begin();

		m_pGraphicsDevice->m_spParticleSystem->Update();
		m_pGraphicsDevice->m_spParticleSystem->Draw();

		m_pGraphicsDevice->m_spShaderManager->m_GPUParticleShader.End();
	}

	//2D
	{
		m_pGraphicsDevice->m_spShaderManager->m_spriteShader.Begin();

		//x = sinf((float)FpsTimer::GetTotalTime()) * 200.0f;
		//y = tanf((float)FpsTimer::GetTotalTime()) * 100.0f;
		m_pGraphicsDevice->m_spShaderManager->m_spriteShader.DrawTexture(m_spTexture.get(), Vector2(x, y));

		m_pGraphicsDevice->m_spShaderManager->m_spriteShader.End();
	}

	m_pImGuiProfile->DrawProfileMonitor();

	m_pGraphicsDevice->End();
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GameDemo::LateUpdate()
{
	m_pAudioDevice->Update(Matrix());
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void GameDemo::Finalize()
{
	m_pImGuiProfile->Finalize();
	m_pAudioDevice->Finalize();
	m_pGraphicsDevice->Finalize();
	m_pWindow->Finalize();

	SafeDelete(m_pFpsTimer);
	SafeDelete(m_pImGuiProfile);
	SafeDelete(m_pAudioDevice);
	SafeDelete(m_pGraphicsDevice);
	SafeDelete(m_pWindow);

	Debug::Log("終了.");
}

/*

#pragma once

#include "../GameObject.h"

class Missile : public GameObject
{
public:

	// コンストラクタ
	Missile();

	// デストラクタ
	~Missile();

	// 初期化
	void Deserialize(const json11::Json& jsonObj) override;

	// 更新
	void Update() override;

	// 半透明物描画
	void DrawEffect() override;

	// オーナーの設定
	inline void SetOwner(const std::shared_ptr<GameObject>& spOwner) { m_wpOwner = spOwner; }

	// 目標の設定
	inline void SetTarget(const std::shared_ptr<GameObject>& spTarget) { m_wpTarget = spTarget; }

private:
	KdVec3						m_prevPos;		// 1フレーム前の座標
	std::weak_ptr<GameObject>	m_wpOwner;		// 発射したGameObject
	float						m_speed;		// 速度
	int							m_lifeSpan;		// 生存期間
	std::weak_ptr<GameObject>	m_wpTarget;		// ターゲットのGameObject
	KdTrailPolygon				m_trailSmoke;	// 煙の軌跡ポリゴン
	float						m_trailRotate;	// 煙の軌跡の回転

private:

	// 衝突判定
	void UpdateCollision();

	//  軌跡の更新
	void UpdateTrail();

	// 爆発
	void Explosion();
};

#include "Aircraft.h"
#include "Missile.h"
#include "EffectObject.h"
#include "AnimationEffect.h"

#include "Application/main.h"
#include "../Scene.h"
#include "../../Component/ModelComponent.h"

Missile::Missile()
	: m_prevPos({ 0.0f,0.0f,0.0f })
	, m_wpOwner({})
	, m_speed(0.5f)
	, m_lifeSpan(0)
	, m_wpTarget({})
	, m_trailSmoke({})
	, m_trailRotate(0.0f)
{
}

Missile::~Missile() {}

void Missile::Deserialize(const json11::Json& jsonObj)
{
	// 生存期間の設定
	m_lifeSpan = APP.m_maxFps * 10;

	// 基底クラスのDeserialize
	GameObject::Deserialize(jsonObj);

	// JsonObject確認
	if (jsonObj.is_null()) { return; }

	// 速度
	if (!jsonObj["Speed"].is_null()) m_speed = static_cast<float>(jsonObj["Speed"].number_value());

	// テクスチャ設定
	m_trailSmoke.SetTexture(KdResFac.GetTexture("Data/Texture/smokeline2.png"));// 煙
}

void Missile::Update()
{
	// 生存確認
	if (!m_alive) return;

	// 生存期間確認.おらもう終わるよ...
	if (--m_lifeSpan <= 0)
	{
		Explosion();
		Destroy();
		return;
	}

	// ターゲットをshared_ptr化 ※weak_ptrは使用時に基本的にlockする
	auto target = m_wpTarget.lock();
	if (target)
	{
		// 情報設定
		KdVec3 targetPos = target->GetMatrix().GetTranslation();// 目標の座標
		KdVec3 vTarget = targetPos - m_mWorld.GetTranslation();	// 自身からターゲットへのベクトル
		KdVec3 vZ = m_mWorld.GetAxisZ();						// 自分のZ方向(前方向)

		// 単位ベクトル化 ※拡大率が入っていると計算がおかしくなる
		vTarget.Normalize();
		vZ.Normalize();

		// 今の前方向のベクトルをターゲット方向のベクトルに指定の角度を向ける
		vZ.Complement(vTarget, 1.5f);

		// 求めた新しい軸をミサイルの前方向にセットする
		m_mWorld.SetAxisZ(vZ);
	}

	// 移動設定
	KdVec3 move = m_mWorld.GetAxisZ();
	move.Normalize();
	move *= m_speed;// 速度考慮

	// 移動前の座標を覚える
	m_prevPos = m_mWorld.GetTranslation();

	// 移動
	m_mWorld.Move(move);

	// 判定
	UpdateCollision();

	// 軌跡の更新
	UpdateTrail();
}

void Missile::DrawEffect()
{
	// 生存確認
	if (!m_alive) return;

	SHADER.m_effectShader.SetWorldMatrix(KdMatrix());
	SHADER.m_effectShader.WriteToCB();

	m_trailSmoke.DrawBillboard(0.5f);
}

void Missile::UpdateCollision()
{
	// 発射した主人のshared_ptr所得
	auto spOwner = m_wpOwner.lock();

	// 球判定用
	SphereInfo sphereInfo = {};
	sphereInfo.m_pos = m_mWorld.GetTranslation();
	sphereInfo.m_radius = 2.0f;

	// レイ判定用
	RayInfo rayInfo = {};
	rayInfo.m_pos = m_mWorld.GetTranslation();
	rayInfo.m_dir = m_mWorld.GetTranslation() - m_prevPos;
	rayInfo.m_maxRange = rayInfo.m_dir.Length();
	rayInfo.m_dir.Normalize();

	// レイ判定の結果を受け取る
	KdRayResult rayResult;

	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		// 自身は無視
		if (obj.get() == this) { continue; }

		// 主人は無視
		if (obj.get() == spOwner.get()) { continue; }

		// TAG_Characterとは球判定
		bool hit = false;
		if (obj.get()->GetTag() & TAG_Character)
		{
			// 球判定
			hit = obj->HitCheckBySphere(sphereInfo);
			if (hit)
			{
				// ダウンキャスト ※危険だし、重たいのでこれがたくさん発生する場合は設計が間違っている
				std::shared_ptr<Aircraft> aircraft = std::dynamic_pointer_cast<Aircraft>(obj);
				if (aircraft)
				{
					// ダメージ通知
					aircraft->OnNotfiy_Damage(5);
				}
			}
		}

		// TAG_StageObjectとはレイ判定
		if (obj.get()->GetTag() & TAG_StageObject)
		{
			hit = obj->HitCheckByRay(rayInfo, rayResult);
		}

		// 当たったら
		if (hit)
		{
			Explosion();
			Destroy();
		}
	}
}

void Missile::UpdateTrail()
{
	// 軌跡の座標を先頭に追加
	m_trailSmoke.AddPoint(m_mWorld);

	// 軌跡の数制限(100以前の軌跡は消去)
	const int LIST_MAX = 100;
	if (m_trailSmoke.GetNumPoints() > LIST_MAX)
	{
		// 一番後ろを消す
		m_trailSmoke.DeletePoint_Back();
	}
}

void Missile::Explosion()
{
	// アニメーションエフェクトをインスタンス化
	std::shared_ptr<AnimationEffect> effect = std::make_shared<AnimationEffect>();

	// 情報を設定
	effect->SetAnimationInfo(KdResFac.GetTexture("Data/Texture/Explosion00.png"), 10.0f, 5, 5,static_cast<float>(rand() % 360), 0.5f, false);

	// 場所をミサイル(自身)の位置に合わせる
	effect->SetMatrix(m_mWorld);

	// リストに追加
	SCENE.AddObject(effect);
}


// 渡されたベクトルに自身を向ける aDegreeで上限を設定
	inline void Complement(const KdVec3& aTarget, float aDegree)
	{
		// 回転軸作成(この軸で回転する)
		KdVec3 vRotAxis = KdVec3::Cross(*this, aTarget);

		// ベクトルの内積
		float dot = KdVec3::Dot(*this, aTarget);

		// 誤差を補正(クランプ)
		dot = std::clamp(dot, -1.0f, 1.0f);

		// ベクトル間の角度
		float radian = acos(dot);

		// 角度制限
		const float RAD_LIMIT = aDegree * KdToRadians;
		if (radian > RAD_LIMIT) radian = RAD_LIMIT;

		// KdMatrix使えないからXMMATRIXで
		DirectX::XMMATRIX matrix = {};
		matrix = DirectX::XMMatrixRotationAxis(vRotAxis, radian);

		// 軸回転行列をベクトルに変換
		TransformNormal(matrix);
	}

*/