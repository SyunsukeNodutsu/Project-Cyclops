#include "FPSCamera.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
FPSCamera::FPSCamera()
	: m_sensitivity(float2(10.0f, 8.0f))
	, m_degAngle(float3::Zero)
	, m_minAngleX(-360.0f)
	, m_maxAngleX(360.0f)
	, m_startFrame(false)
	, m_frameCount(0)
{
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void FPSCamera::Update()
{
	static const float2 set_center = float2(800, 450);

	const float2& nowPos = m_startFrame ? set_center : Input::GetMousePos();

	float2 mouseMove = float2(0, 0);
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
		m_frameCount++;
		if (m_frameCount > 1) m_startFrame = false;
	}
}

//-----------------------------------------------------------------------------
// 使用開始
//-----------------------------------------------------------------------------
void FPSCamera::OnUseStart()
{
	Input::SetMouseHide(true);
	m_startFrame = true;
	m_frameCount = 0;
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
void FPSCamera::SetCameraMatrix(const matrix4x4& cameraMatrix)
{
	matrix4x4 trans = trans.CreateTranslation(m_localPos);
	matrix4x4 rotation = GetRotationMatrix();

	Camera::SetCameraMatrix(rotation * trans * cameraMatrix);
}

//-----------------------------------------------------------------------------
// 回転行列を返す
//-----------------------------------------------------------------------------
const matrix4x4 FPSCamera::GetRotationMatrix()
{
	return matrix4x4::CreateFromYawPitchRoll(DegToRad(m_degAngle.y), DegToRad(m_degAngle.x), DegToRad(m_degAngle.z));
}

//-----------------------------------------------------------------------------
// Y軸の回転行列を返す
//-----------------------------------------------------------------------------
const matrix4x4 FPSCamera::GetRotationYMatrix()
{
	return matrix4x4::CreateRotationX(DegToRad(m_degAngle.y));
}
