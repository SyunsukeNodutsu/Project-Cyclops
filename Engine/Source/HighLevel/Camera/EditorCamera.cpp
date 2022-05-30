#include "EditorCamera.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
EditorCamera::EditorCamera()
	: m_position(Vector3(0, 0, -4))
	, m_rotation(Vector3::Zero)
	, m_mousePosOld(0, 0)
	, m_mousePosNow(0, 0)
{
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void EditorCamera::Update()
{
	m_mousePosNow = Input::GetMousePos();

	//回転
	{
		float raito = 10.0f;
		if (Input::IsMousePressed(MouseButton::Right))
		{
			int deltaX = m_mousePosNow.x - m_mousePosOld.x;
			int deltaY = m_mousePosNow.y - m_mousePosOld.y;

			m_rotation.x += static_cast<float>(deltaY) * raito * FpsTimer::GetDeltaTime<float>(true);
			m_rotation.y += static_cast<float>(deltaX) * raito * FpsTimer::GetDeltaTime<float>(true);
		}
	}

	//移動
	{
		float raito = 1.0f;
		if (Input::IsMousePressed(MouseButton::Middle))
		{
			int deltaX = m_mousePosNow.x - m_mousePosOld.x;
			int deltaY = m_mousePosNow.y - m_mousePosOld.y;

			m_position -= Left() * (deltaX * raito * FpsTimer::GetDeltaTime<float>(true));
			m_position += Up() * (deltaY * raito * FpsTimer::GetDeltaTime<float>(true));
		}

		raito = 20.0f;
		int wheel = Input::GetMouseWheelDelta();
		if (wheel != 0)
			m_position += Forward() * (wheel * raito * FpsTimer::GetDeltaTime<float>(true));
	}

	Matrix rotation = Matrix::CreateFromYawPitchRoll(DegToRad(m_rotation.y), DegToRad(m_rotation.x), DegToRad(m_rotation.z));
	Matrix trans = Matrix::CreateTranslation(m_position);
	SetCameraMatrix(rotation * trans);

	m_mousePosOld = m_mousePosNow;
}
