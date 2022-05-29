#include "FPSCamera.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
FPSCamera::FPSCamera()
	: m_degAngle(Vector3::Zero)
	, m_minAngleX(-360)
	, m_maxAngleX(360)
	, m_sensitivity(Vector2(10.0f, 8.0f))
{
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void FPSCamera::Update()
{
	if (!m_enable) return;

	static const Vector2Int set_center = Vector2Int(800, 450);

	const Vector2Int& nowPos = Input::GetMousePos();

	Vector2Int mouseMove = Vector2Int(0, 0);
	mouseMove.x = nowPos.x - set_center.x;
	mouseMove.y = nowPos.y - set_center.y;

	Input::SetMousePos(set_center);

	m_degAngle.x += static_cast<float>(mouseMove.y) * m_sensitivity.y * FpsTimer::GetDeltaTime<float>(true);
	m_degAngle.y += static_cast<float>(mouseMove.x) * m_sensitivity.x * FpsTimer::GetDeltaTime<float>(true);

	//角度制御
	m_degAngle.x = std::clamp(m_degAngle.x, m_minAngleX, m_maxAngleX);
}

//-----------------------------------------------------------------------------
// FPSカメラのカメラ行列を設定
//-----------------------------------------------------------------------------
void FPSCamera::SetCameraMatrix(const Matrix& cameraMatrix)
{
	//TODO: m_enableの確認入れてもいいかも

	Matrix trans = trans.CreateTranslation(m_localPos);
	Matrix rotation = GetRotationMatrix();

	Camera::SetCameraMatrix(rotation * trans * cameraMatrix);
}

//-----------------------------------------------------------------------------
// 回転行列を返す
//-----------------------------------------------------------------------------
const Matrix FPSCamera::GetRotationMatrix()
{
	return Matrix::CreateFromYawPitchRoll(DegToRad(m_degAngle.y), DegToRad(m_degAngle.x), DegToRad(m_degAngle.z));
}

//-----------------------------------------------------------------------------
// Y軸の回転行列を返す
//-----------------------------------------------------------------------------
const Matrix FPSCamera::GetRotationYMatrix()
{
	return Matrix::CreateRotationX(DegToRad(m_degAngle.y));
}
