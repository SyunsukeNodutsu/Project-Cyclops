#include "EditorCamera.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
EditorCamera::EditorCamera()
	: m_position(float3(0, 0, -4))
	, m_rotation(float3::Zero)
	, m_moveSpeed(1.0f)
	, m_zoomSpeed(20.0f)
	, m_rotSpeed(10.0f)
	, m_mousePosNow(float2::Zero)
	, m_mousePosOld(float2::Zero)
{
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void EditorCamera::Update()
{
	m_mousePosNow = Input::GetMousePos();

	//移動
	if (Input::IsMousePressed(MouseButton::Middle))
	{
		float deltaX = m_mousePosNow.x - m_mousePosOld.x;
		float deltaY = m_mousePosNow.y - m_mousePosOld.y;

		m_position -= Right() * (deltaX * m_moveSpeed * FpsTimer::GetDeltaTime<float>(true));
		m_position += Up() * (deltaY * m_moveSpeed * FpsTimer::GetDeltaTime<float>(true));
	}

	//ズーム移動
	if (int wheel = Input::GetMouseWheelDelta(); wheel != 0)
	{
		m_position += Backward() * (wheel * m_zoomSpeed * FpsTimer::GetDeltaTime<float>(true));
	}

	//回転
	if (Input::IsMousePressed(MouseButton::Right))
	{
		float deltaX = m_mousePosNow.x - m_mousePosOld.x;
		float deltaY = m_mousePosNow.y - m_mousePosOld.y;

		m_rotation.x += deltaY * m_rotSpeed * FpsTimer::GetDeltaTime<float>(true);
		m_rotation.y += deltaX * m_rotSpeed * FpsTimer::GetDeltaTime<float>(true);
	}

	//カメラ行列作成
	matrix4x4 rotation = matrix4x4::CreateFromYawPitchRoll(DegToRad(m_rotation.y), DegToRad(m_rotation.x), DegToRad(m_rotation.z));
	matrix4x4 trans = matrix4x4::CreateTranslation(m_position);
	SetCameraMatrix(rotation * trans);

	m_mousePosOld = m_mousePosNow;

	Debug::LogClear();
	Debug::Log("m_position: " + ToStringV(m_position));
	Debug::Log("m_rotation: " + ToStringV(m_rotation));
}
