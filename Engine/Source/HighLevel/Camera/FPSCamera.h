//-----------------------------------------------------------------------------
// File: FPSCamera.h
//
// FPS視点操作カメラ
//-----------------------------------------------------------------------------
#pragma once

//FPS操作カメラクラス
class FPSCamera : public Camera
{
public:

	FPSCamera();
	~FPSCamera() = default;

	void SetCameraMatrix(const matrix4x4& cameraMat) override;
	void Update() override;

	void OnUseStart() override;
	void OnUseEnd() override;

	void SetLocalPos(float3 pos) { m_localPos = pos; }
	void SetClampAngleX(float minAng, float maxAng) { m_minAngleX = minAng; m_maxAngleX = maxAng; }
	void SetAngle(float2 angle) { m_degAngle = float3(angle.x, angle.y, m_degAngle.z); }

	const matrix4x4 GetRotationMatrix();
	const matrix4x4 GetRotationYMatrix();
	const float3& GetRotationAngles() const { return m_degAngle; }

	const float GetRotationYAngle() const { return 0; }

protected:

	float3 m_localPos;

private:

	float2		m_sensitivity;	//感度
	float3		m_degAngle;		//各軸の回転情報
	float		m_minAngleX;	//上向きの制限
	float		m_maxAngleX;	//下向きの制限
	bool		m_startFrame;	//使用開始の際にマウス座標の取得を1フレーム飛ばす

};
