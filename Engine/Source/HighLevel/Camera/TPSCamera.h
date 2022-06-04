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

	void SetCameraMatrix(const matrix4x4& worldMatrix) override;
	void SetViewPoint(const float3& v) { m_viewPoint = v; }

private:

	float3 m_viewPoint;//注視点

};
