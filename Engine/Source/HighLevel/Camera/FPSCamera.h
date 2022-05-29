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

	void Update();

	void SetCameraMatrix(const Matrix& cameraMat) override;

	void SetLocalPos(Vector3 pos) { m_localPos = pos; }
	void SetClampAngleX(float minAng, float maxAng) { m_minAngleX = minAng; m_maxAngleX = maxAng; }
	void SetAngle(Vector2 angle) { m_degAngle = Vector3(angle.x, angle.y, m_degAngle.z); }

	const Matrix GetRotationMatrix();
	const Matrix GetRotationYMatrix();
	const Vector3& GetRotationAngles() const { return m_degAngle; }

	const float GetRotationYAngle() const { return 0; }

protected:

	Vector3 m_localPos;

private:

	//各軸の回転情報
	Vector3 m_degAngle;

	//X軸の回転を制限する
	float m_minAngleX; //上向きの制限
	float m_maxAngleX; //下向きの制限

	Vector2 m_sensitivity;//感度

};
