//-----------------------------------------------------------------------------
// File: TPSCamera.h
//
// TPS視点操作カメラ
//-----------------------------------------------------------------------------
#pragma once

//TPS操作カメラクラス
class TPSCamera : public FPSCamera
{
public:

	void SetCameraMatrix(const Matrix& worldMatrix) override;
	void SetViewPoint(const Vector3& v) { m_viewPoint = v; }

private:

	Vector3 m_viewPoint;//注視点

};
