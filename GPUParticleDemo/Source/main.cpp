//-----------------------------------------------------------------------------
// File: main.cpp
//
// GPUParticleのデモシーン
//-----------------------------------------------------------------------------
#include "../../Engine/Source/Pch.h"
#pragma comment(lib, "Engine.lib")

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{


	return 0;
}

/*
class Human : public GameObject
{
public:
	Human();
	void Deserialize(const json11::Json& jsonObj) override;
	void Update() override;
	bool IsGround() const { return m_isGround; }
	void SetAnimation(const char* apAnimName);

private:
	KdVec3								m_position;				// ワールド行列上の座標
	KdVec3								m_rotation;				// ワールド行列上の回転
	KdVec3								m_prevPosition;			// 前フレームの座標
	KdVec3								m_force;				// キャラクターにかかる移動させる力(落下、跳躍、移動)
	float								m_moveSpeed;			// 移動速度
	float								m_rotateAngle;			// 回転角度
	float								m_camRotSpeed;			// カメラの回転速度
	float								m_gravity;				// キャラクターにかかる重力
	bool								m_isGround;				// 着地しているかどうか
	bool								m_isWall;				// 壁に当たっているかどうか
	KdBrendAnimator						m_animator;				// アニメーション管理
	float								m_animationSpeed;

	// 各定数 cppの上側で設定
	static const float					s_allowToStopHeight;	// 歩いて乗り越えられる段差の高さ
	static const float					s_landingHeight;		// 地面から足が離れていても着地していると判定する高さ(坂道など)
	static const float					s_betweenTheWall;		// 壁に当たったと判断するHumanと壁の間の値

	std::shared_ptr<ActionBase> m_spActionState;// 現在の状態

private:
	void UpdateMove();
	void UpdateRotate(const KdVec3& aMoveVec);
	void UpdateAttack();
	void UpdateCamera();
	void UpdateCollision();
	bool CheckGround(float& aDstDistance);
	void CheckBump();
	bool IsChangeMove();
	bool IsChangeJump();
	void ChangeWaith();
	void ChangeMove();
	void ChangeJump();
};

//-----------------------------------------------------------------------------
// File: Human.cpp
// 
// Edit: 2021/01/05 野筒隼輔
//-----------------------------------------------------------------------------
#include "Human.h"
#include "BackMirror.h"
#include "Application/Game/Action/Bullet.h"
#include "../Scene.h"
#include "../../Component/CameraComponent.h"
#include "../../Component/InputComponent.h"
#include "../../Component/ModelComponent.h"

const float Human::s_allowToStopHeight = 0.8f;
const float Human::s_landingHeight = 0.1f;
const float Human::s_betweenTheWall = 0.5f;

Human::Human()
	: m_position({ 0.0f,0.0f,0.0f })
	, m_rotation()
	, m_prevPosition({ 0.0f,0.0f,0.0f })
	, m_force()
	, m_moveSpeed(0.1f)
	, m_rotateAngle(10.0f)
	, m_camRotSpeed(0.1f)
	, m_gravity(0.01f)
	, m_isGround(false)
	, m_isWall(false)
	, m_animationSpeed(1.0f)
{
}

void Human::Deserialize(const json11::Json& jsonObj)
{
	// 基底クラスの初期化(共通の初期化)
	GameObject::Deserialize(jsonObj);

	//--------------------------------------------------
	// 自身の
	//--------------------------------------------------

	// 座標初期化
	m_position = m_mWorld.GetTranslation();

	// 状態 Waitから
	m_spActionState = std::make_shared<ActionWait>();
	if (m_spActionState == nullptr)
	{
		m_spActionState.reset();
		assert(0 && "[Deserialize] : ステートマシン生成失敗");
		return;
	}

	// アニメーション Standから
	SetAnimation("Stand");

	//--------------------------------------------------
	// コンポーネント
	//--------------------------------------------------

	// カメラ
	if (m_spCameraComponent)
	{
		m_spCameraComponent->OffsetMatrix().CreateTranslation(0.0f, 1.5f, -5.0f);
		m_spCameraComponent->OffsetMatrix().RotateX(10.0f * KdToRadians);
		SCENE.SetTargetCamera(m_spCameraComponent);
	}

	// コントローラ
	m_spInputComponent = std::make_shared<ActionGameInputComponent>(*this);

	//--------------------------------------------------
	// バックミラー
	//--------------------------------------------------

	// 生成
	auto backMirror = std::make_shared<BackMirror>();
	if (backMirror)
	{
		backMirror->Deserialize(json11::Json());	// 初期化 ※将来的にはデータを渡すことになりそう
		backMirror->SetOwner(shared_from_this());	// オーナー設定
		SCENE.AddObject(backMirror);				// コンテナに追加
	}

	// シャドウマップ用シェーダにカメラ情報設定
	SHADER.m_genShadowMapShader.SetShadowTarget(m_spCameraComponent);
}

void Human::SetAnimation(const char* apAnimName)
{
	if (!m_spModelComponent) return;

	std::shared_ptr<KdAnimationData> apAnimData = m_spModelComponent->GetAnimation(apAnimName);
	m_animator.SetAnimation(apAnimData);
}

void Human::Update()
{
	// 点光を追加
	KdVec3 pos = m_mWorld.GetTranslation();
	pos.y += 1;
	SHADER.AddPointLight(pos, 5, { 10.0f, 2.5f, 10.0f });

	if (m_position.y <= -100.0f)
	{
		m_position.x = 0.0f;
		m_position.z = 0.0f;
		m_position.y = 10.0f;
	}

	// コントローラ更新
	if (m_spInputComponent) m_spInputComponent->Update();

	// ダッシュ
	if (m_spInputComponent->GetButton(Input::Button::X))
	{
		m_moveSpeed = 0.2f;
		m_animationSpeed = 2.0f;
	}
	else
	{
		m_moveSpeed = 0.1f;
		m_animationSpeed = 0.8f;
	}

	// Escキーが押された
	static bool isPushEscape = false;
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
	{
		if (!isPushEscape)
		{
			m_spInputComponent->ShowCursorToggle();
			isPushEscape = true;
		}
	}
	else if (isPushEscape) isPushEscape = false;

	// カーソルが停止...カメラを操作したくないためreturn
	if (!m_spInputComponent->IsCursorStop()) return;

	// 移動前の座標記憶
	m_prevPosition = m_position;

	// カメラ更新
	UpdateCamera();

	// 状態ごとの更新
	m_spActionState->Update(*this);

	// 重力による落下
	m_force.y -= m_gravity;

	// 移動量確定
	m_position += m_force;

	// 座標の更新が確定した後に当たり判定
	UpdateCollision();

	// ワールド行列を合成
	m_mWorld.CreateRotationX(m_rotation.x);//作る
	m_mWorld.RotateY(m_rotation.y);
	m_mWorld.RotateZ(m_rotation.z);
	m_mWorld.Move(m_position);

	// カメラ更新(回転しないように移動成分のみの行列を作成)
	if (m_spCameraComponent)
	{
		KdMatrix transMatrix = {};
		transMatrix.CreateTranslation(m_position.x, m_position.y, m_position.z);
		m_spCameraComponent->SetCameraMatrix(transMatrix);
	}

	// アニメーションの更新
	m_animator.AdvanceTime(m_spModelComponent->GetChangeableNodes(), m_animationSpeed);

	// 点光を追加
	//KdVec3 pos = m_mWorld.GetTranslation();
	//pos.y += 1;
	//SHADER.AddPointLight(pos, 5, { 100.0f, 20.5f, 100.0f });
}

void Human::UpdateMove()
{
	// カメラ確認
	if (!m_spCameraComponent) return;

	// コントローラ確認
	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::L);
	if (inputMove.LengthSquared() == 0.0f) return;

	// カメラの右方向 * スティックの左右入力 = キャラクターの左右の移動方向
	KdVec3 moveSide = m_spCameraComponent->GetCameraMatrix().GetAxisX() * inputMove.x;

	// カメラの前方向 * スティックの前後入力 = キャラクターの前後の移動方向
	KdVec3 moveForward = m_spCameraComponent->GetCameraMatrix().GetAxisZ() * inputMove.y;

	// 上方向への移動成分はカット
	moveForward.y = 0.0f;

	// 移動ベクトルの計算
	KdVec3 moveVec = moveSide + moveForward;

	// 正規化
	moveVec.Normalize();

	// 回転更新
	UpdateRotate(moveVec);

	// 速度加味
	moveVec *= m_moveSpeed;

	// 移動
	m_force.x = moveVec.x;
	m_force.z = moveVec.z;
}

void Human::UpdateRotate(const KdVec3& aMoveVec)
{
	// 移動しているか
	if (aMoveVec.LengthSquared() == 0.0f) return;

	// 現在のキャラクターの方向ベクトル
	KdVec3 nowDir = m_mWorld.GetAxisZ();
	nowDir.Normalize();

	// atan2(y, x) ... xとy(z)を逆にすると基準線が上向きになる
	float nowRadian = atan2(nowDir.x, nowDir.z);		// 向いている方向の角度算出(ラジアン)
	float targetRadian = atan2(aMoveVec.x, aMoveVec.z);	// 移動方向へのベクトルから角度算出(ラジアン)
	float rotateRadian = targetRadian - nowRadian;			// 差分を計算

	// 180どの角度で数値の切れ目がある ※atan2の結果 ... -180 ～ 180
	if (rotateRadian > M_PI)
	{
		rotateRadian -= 2 * float(M_PI);
	}
	else if (rotateRadian < -M_PI)
	{
		rotateRadian += 2 * float(M_PI);
	}

	// 1回の回転角度をm_rotateAgle以内に収める(クランプ)
	rotateRadian = std::clamp(rotateRadian, -m_rotateAngle * KdToRadians, m_rotateAngle * KdToRadians);

	m_rotation.y += rotateRadian;
}

void Human::UpdateAttack()
{
	// 連続発射防止
	static bool canShot = false;

	// ミサイルの発射
	if (m_spInputComponent->GetButton(Input::Button::B))
	{
		// 連続発射防止
		if (!canShot) return;
		canShot = !canShot;

		// Missile生成
		std::shared_ptr<Bullet> spMissile = std::make_shared<Bullet>();
		if (spMissile == nullptr) return;

		//--------------------------------------------------
		// 情報設定
		//--------------------------------------------------

		// 初期化
		spMissile->Deserialize(KdResFac.GetJSON("Data/Scene/Missile.json"));

		// 縮小させる
		KdMatrix mLaunch = {};
		mLaunch.CreateScalling(0.4f, 0.4f, 0.4f);
		mLaunch *= m_mWorld;
		spMissile->SetMatrix(mLaunch);

		// 少し浮かせる
		KdVec3 pos = m_position;
		pos.y += 1.0f;
		spMissile->SetInfo(shared_from_this(), pos, m_mWorld.GetAxisZ());

		//--------------------------------------------------
		// コンテナに追加
		//--------------------------------------------------
		SCENE.AddObject(spMissile);
	}
	else if (!canShot) canShot = !canShot;
}

void Human::UpdateCamera()
{
	// カメラ確認
	if (!m_spCameraComponent) return;

	// 入力値所得
	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::R);
	KdVec3 rotVec = { inputMove.x, 0.0f, 0.0f };

	// 速度を加味
	rotVec *= m_camRotSpeed;

	// 縦軸で横に回転
	m_spCameraComponent->OffsetMatrix().RotateY(rotVec.x * KdToRadians);
}

void Human::UpdateCollision()
{
	// 下方向への判定
	float distanceFromGround = FLT_MAX;
	if (CheckGround(distanceFromGround))
	{
		// 地面の上にy座標を移動
		m_position.y += s_allowToStopHeight - distanceFromGround;

		// 地面があるので、y方向への移動力は0に
		m_force.y = 0.0f;
	}

	// ぶつかり
	CheckBump();
}

bool Human::CheckGround(float& aDstDistance)
{
	// レイ情報設定
	RayInfo rayInfo = {};
	rayInfo.m_pos = m_position;							// キャラクターの位置を発射地点に
	rayInfo.m_pos.y += s_allowToStopHeight;				// 補正.キャラの足元だと地面に当たらないので少し持ち上げる(乗り越えられる段差の高さ分だけ)
	rayInfo.m_pos.y += m_prevPosition.y - m_position.y;	// 補正.落下中かもしれないので、1フレーム前の座標分も持ち上げる
	rayInfo.m_dir = { 0.0f,-1.0f,0.0f };				// 逆方向へのレイ
	rayInfo.m_maxRange = FLT_MAX;

	// レイの格納用
	KdRayResult finalRayResult = {};

	// 衝突したGameObject
	std::shared_ptr<GameObject> hitObj = nullptr;

	// 全Objectとレイ判定
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		// 自身、地面以外は無視
		if (obj.get() == this) continue;
		if (!(obj->GetTag() & (TAG_StageObject))) continue;

		// 判定
		KdRayResult rayResult = {};
		if (obj->HitCheckByRay(rayInfo, rayResult))
		{
			// 最も当たったところまでの距離が短いものを保持
			if (rayResult.m_distance < finalRayResult.m_distance)
			{
				finalRayResult = rayResult;

				// 当たったオブジェクト設定
				hitObj = obj;
			}
		}
	}

	float distanceFromGround = FLT_MAX;

	// 足元にステージオブジェクトがあった
	if (finalRayResult.m_hit)
	{
		// 地面との距離を算出(補正値も考慮)
		distanceFromGround = finalRayResult.m_distance - (m_prevPosition.y - m_position.y);
	}

	// 上方向に力がかかっていた場合(ジャンプ中とかは着地するはずがない)
	m_isGround = (m_force.y > 0.0f)
		? false
		: distanceFromGround < (s_allowToStopHeight + s_landingHeight);

	// 地面との距離を格納(更新)
	aDstDistance = distanceFromGround;

	// 動くものの上に着地した
	if (hitObj && m_isGround)
	{
		// 相手の一回分の動いた情報を設定
		auto mOncMove = hitObj->GetOneMove();
		auto vOncMove = mOncMove.GetTranslation();

		//
		m_position += vOncMove;
	}

	return m_isGround;
}

void Human::CheckBump()
{
	// 球情報の作成
	SphereInfo info = {};
	info.m_pos = m_position;	// 自身の座標から
	info.m_pos.y += 0.8f;		// 少し持ち上げる
	info.m_radius = 0.4f;		// 半径いいかんじに設定

	// 結果格納用
	SphereResult result = {};

	// 確認用の球描画
	if (SCENE.IsDebugMode()) SCENE.AddDebugSphereLine(info.m_pos, info.m_radius);

	// 全Objectと判定
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		// 自身、地面以外は無視
		if (obj.get() == this) continue;
		if (!(obj->GetTag() & (TAG_StageObject))) continue;

		// 判定
		if (obj->HitCheckBySphereVsMeshconst(info, result))
		{
			m_position += result.m_push;
		}
	}
}

bool Human::IsChangeMove()
{
	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::L);

	if (inputMove.LengthSquared() != 0.0f)
	{
		return true;
	}

	return false;
}

bool Human::IsChangeJump()
{
	if (m_isGround)
	{
		if (m_spInputComponent->GetButton(Input::Button::A))
		{
			return true;
		}
	}

	return false;
}

void Human::ChangeWaith()
{
	m_force.x = 0.0f;
	m_force.z = 0.0f;

	m_spActionState = std::make_shared<ActionWait>();

	SetAnimation("Stand");
}

void Human::ChangeMove()
{
	m_spActionState = std::make_shared<ActionWalk>();
	SetAnimation("Walk");
}

void Human::ChangeJump()
{
	KD_AUDIO.Play("Data/Audio/SE/ItemGet.wav");
	m_force.y = 0.2f;
	m_spActionState = std::make_shared<ActionJump>();
	SetAnimation("Jump");
}

//---------------------------------------------------------------
// ステートマシン: 状態ごとの更新
//---------------------------------------------------------------

void Human::ActionWait::Update(Human& aOwner)
{
	// 攻撃
	aOwner.UpdateAttack();

	// 遷移
	if (aOwner.IsChangeMove())
	{
		aOwner.ChangeMove();
		return;
	}

	// 遷移
	if (aOwner.IsChangeJump())
	{
		aOwner.ChangeJump();
		return;
	}

	return;
	// Human君こっち向かんかい
	KdMatrix viewMat = aOwner.m_spCameraComponent->GetViewMatrix();// ビュー行列
	viewMat.Inverse();
	KdVec3 axisZ = viewMat.GetAxisZ();// 軸をこっちに向ける
	aOwner.UpdateRotate(axisZ);// 回転
}

void Human::ActionWalk::Update(Human& aOwner)
{
	// 移動
	aOwner.UpdateMove();

	// 攻撃
	aOwner.UpdateAttack();

	// 遷移
	if (!aOwner.IsChangeMove())
	{
		aOwner.ChangeWaith();
		return;
	}

	// 遷移
	if (aOwner.IsChangeJump())
	{
		aOwner.ChangeJump();
		return;
	}
}

void Human::ActionRun::Update(Human& aOwner)
{
	// 移動
	aOwner.UpdateMove();

	// 攻撃
	aOwner.UpdateAttack();

	// 遷移
	if (!aOwner.IsChangeMove())
	{
		aOwner.ChangeWaith();
		return;
	}

	// 遷移
	if (aOwner.IsChangeJump())
	{
		aOwner.ChangeJump();
		return;
	}
}

void Human::ActionJump::Update(Human& aOwner)
{
	// 攻撃
	aOwner.UpdateAttack();

	// 遷移
	if (aOwner.m_isGround)
	{
		aOwner.ChangeWaith();
		return;
	}
}
*/