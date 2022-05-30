#include "FPSCamera.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
FPSCamera::FPSCamera()
	: m_sensitivity(Vector2(10.0f, 8.0f))
	, m_degAngle(Vector3::Zero)
	, m_minAngleX(-360)
	, m_maxAngleX(360)
	, m_startFrame(false)
{
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void FPSCamera::Update()
{
	static const Vector2Int set_center = Vector2Int(800, 450);

	const Vector2Int& nowPos = m_startFrame ? set_center : Input::GetMousePos();

	Vector2Int mouseMove = Vector2Int(0, 0);
	mouseMove.x = nowPos.x - set_center.x;
	mouseMove.y = nowPos.y - set_center.y;

	Input::SetMousePos(set_center);

	m_degAngle.x += static_cast<float>(mouseMove.y) * m_sensitivity.y * FpsTimer::GetDeltaTime<float>(true);
	m_degAngle.y += static_cast<float>(mouseMove.x) * m_sensitivity.x * FpsTimer::GetDeltaTime<float>(true);

	//角度制御
	m_degAngle.x = std::clamp(m_degAngle.x, m_minAngleX, m_maxAngleX);

	if (m_startFrame)
	{
		//即座に視点が移動しないように
		static int count = 0;
		count++;
		if (count > 1) { count = 0; m_startFrame = false; }
	}
}

//-----------------------------------------------------------------------------
// 使用開始
//-----------------------------------------------------------------------------
void FPSCamera::OnUseStart()
{
	Input::SetMouseHide(true);
	m_startFrame = true;

	Debug::Log("OnUseStart()");
}

//-----------------------------------------------------------------------------
// 使用終了
//-----------------------------------------------------------------------------
void FPSCamera::OnUseEnd()
{
	Input::SetMouseHide(false);
}

//-----------------------------------------------------------------------------
// FPSカメラのカメラ行列を設定
//-----------------------------------------------------------------------------
void FPSCamera::SetCameraMatrix(const Matrix& cameraMatrix)
{
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
