#include "EditorCamera.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
EditorCamera::EditorCamera()
	: m_position(Vector3(0, 0, -4))
	, m_rotation(Vector3::Zero)
	, m_moveSpeed(1.0f)
	, m_zoomSpeed(20.0f)
	, m_rotSpeed(10.0f)
	, m_mousePosNow(Vector2Int(0, 0))
	, m_mousePosOld(Vector2Int(0, 0))
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
		int deltaX = m_mousePosNow.x - m_mousePosOld.x;
		int deltaY = m_mousePosNow.y - m_mousePosOld.y;

		m_position -= Left() * (deltaX * m_moveSpeed * FpsTimer::GetDeltaTime<float>(true));
		m_position += Up() * (deltaY * m_moveSpeed * FpsTimer::GetDeltaTime<float>(true));
	}

	//ズーム移動
	if (int wheel = Input::GetMouseWheelDelta(); wheel != 0)
	{
		m_position += Forward() * (wheel * m_zoomSpeed * FpsTimer::GetDeltaTime<float>(true));
	}

	//回転
	if (Input::IsMousePressed(MouseButton::Right))
	{
		int deltaX = m_mousePosNow.x - m_mousePosOld.x;
		int deltaY = m_mousePosNow.y - m_mousePosOld.y;

		m_rotation.x += static_cast<float>(deltaY) * m_rotSpeed * FpsTimer::GetDeltaTime<float>(true);
		m_rotation.y += static_cast<float>(deltaX) * m_rotSpeed * FpsTimer::GetDeltaTime<float>(true);
	}

	//カメラ行列作成
	Matrix rotation = Matrix::CreateFromYawPitchRoll(DegToRad(m_rotation.y), DegToRad(m_rotation.x), DegToRad(m_rotation.z));
	Matrix trans = Matrix::CreateTranslation(m_position);
	SetCameraMatrix(rotation * trans);

	m_mousePosOld = m_mousePosNow;
}
