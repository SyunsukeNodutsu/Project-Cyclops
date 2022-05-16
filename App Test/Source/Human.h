//-----------------------------------------------------------------------------
// File: Human.h
// 
// Edit: 2021/01/05 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "../GameObject.h"

class BackMirror;

//-----------------------------------------------------------------------------
// Name: class Human
// Desc: 人型クラス ステートマシン実装 GameObject継承
//-----------------------------------------------------------------------------
class Human : public GameObject
{
public:
	//--------------------------------------------------
	// Name: Human()
	// Desc: コンストラクタ
	//--------------------------------------------------
	Human();

	//--------------------------------------------------
	// Name: Deserialize()
	// Desc: 初期化
	//--------------------------------------------------
	void Deserialize(const json11::Json& jsonObj) override;

	//--------------------------------------------------
	// Name: Update()
	// Desc: 更新
	//--------------------------------------------------
	void Update() override;

	//--------------------------------------------------
	// Name: IsGround()
	// Desc: 地面に衝突しているか
	//--------------------------------------------------
	bool IsGround() const { return m_isGround; }

	//--------------------------------------------------
	// Name: SetAnimation
	// Desc: アニメーション設定
	//--------------------------------------------------
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

private:
	//--------------------------------------------------
	// Name: UpdateMove()
	// Desc: 移動更新
	//--------------------------------------------------
	void UpdateMove();

	//--------------------------------------------------
	// Name: UpdateRotate()
	// Desc: 回転更新
	//--------------------------------------------------
	void UpdateRotate(const KdVec3& aMoveVec);

	//--------------------------------------------------
	// Name: UpdateAttack()
	// Desc: 攻撃更新
	//--------------------------------------------------
	void UpdateAttack();

	//--------------------------------------------------
	// Name: UpdateCamera()
	// Desc: カメラ更新
	//--------------------------------------------------
	void UpdateCamera();

	//--------------------------------------------------
	// Name: UpdateCollision()
	// Desc: 衝突判定更新
	//--------------------------------------------------
	void UpdateCollision();

	//--------------------------------------------------
	// Name: CheckGround()
	// Desc: 地面との衝突判定
	//--------------------------------------------------
	bool CheckGround(float& aDstDistance);

	//--------------------------------------------------
	// Name: CheckBump()
	// Desc: ぶつかり判定
	//--------------------------------------------------
	void CheckBump();

	//--------------------------------------------------
	// Name: IsChangeMove()
	// Desc: 待機状態に遷移するかどうか
	//--------------------------------------------------
	bool IsChangeMove();

	//--------------------------------------------------
	// Name: IsChangeJump()
	// Desc: ジャンプ状態に遷移するかどうか
	//--------------------------------------------------
	bool IsChangeJump();

	//--------------------------------------------------
	// Name: ChangeWaith()
	// Desc: 待機に変更
	//--------------------------------------------------
	void ChangeWaith();

	//--------------------------------------------------
	// Name: ChangeMove()
	// Desc: 移動に変更
	//--------------------------------------------------
	void ChangeMove();

	//--------------------------------------------------
	// Name: ChangeJump()
	// Desc: ジャンプに変更
	//--------------------------------------------------
	void ChangeJump();

private:
	//--------------------------------------------------
	//
	// ステートマシン: 状態管理
	//
	//--------------------------------------------------

	//--------------------------------------------------
	// Name: class ActionBase
	//--------------------------------------------------
	class ActionBase
	{
	public:
		virtual void Update(Human& aOwner) = 0;
	};

	//--------------------------------------------------
	// Name: class ActionBase
	//--------------------------------------------------
	class ActionWait : public ActionBase
	{
	public:
		void Update(Human& aOwner) override;
	};

	//--------------------------------------------------
	// Name: class ActionWalk
	//--------------------------------------------------
	class ActionWalk : public ActionBase
	{
	public:
		void Update(Human& aOwner) override;
	};

	//--------------------------------------------------
	// Name: class ActionRun
	//--------------------------------------------------
	class ActionRun : public ActionBase
	{
	public:
		void Update(Human& aOwner) override;
	};

	//--------------------------------------------------
	// Name: class ActionJump ※直前 上昇中 下降中 でわけたらもっといい
	//--------------------------------------------------
	class ActionJump : public ActionBase
	{
	public:
		void Update(Human& aOwner) override;
	};

	// 現在の状態
	std::shared_ptr<ActionBase> m_spActionState;
};