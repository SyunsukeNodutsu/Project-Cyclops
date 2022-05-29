#include "EditorCamera.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
EditorCamera::EditorCamera()
	: m_position(Vector3::Zero)
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
		float raito = 60.0f;
		if (Input::IsKeyPressed(KeyCode::LeftArrow))	m_rotation.y -= raito * FpsTimer::GetDeltaTime<float>(true);
		if (Input::IsKeyPressed(KeyCode::RightArrow))	m_rotation.y += raito * FpsTimer::GetDeltaTime<float>(true);
		if (Input::IsKeyPressed(KeyCode::UpArrow))		m_rotation.x -= raito * FpsTimer::GetDeltaTime<float>(true);
		if (Input::IsKeyPressed(KeyCode::DownArrow))	m_rotation.x += raito * FpsTimer::GetDeltaTime<float>(true);

		/*if (Input::IsKeyPressed(KeyCode::Space))
		{
			float deltaX = m_mousePosNow.x - m_mousePosOld.x;
			float deltaY = m_mousePosNow.y - m_mousePosOld.y;

			m_rotation.x += deltaY * raito * (float)FpsTimer::GetDeltaTime();
			m_rotation.y += deltaX * raito * (float)FpsTimer::GetDeltaTime();
		}*/
	}

	//移動
	{
		float raito = 6.0f;
		Vector3 axisZ = GetCameraMatrix().GetAxisZ(); axisZ.Normalize();
		Vector3 axisX = GetCameraMatrix().GetAxisX(); axisX.Normalize();

		if (Input::IsKeyPressed(KeyCode::Shift)) raito = 20.0f;
		if (Input::IsKeyPressed(KeyCode::Ctrl)) raito = 2.0f;

		if (Input::IsKeyPressed(KeyCode::W)) m_position += axisZ * (raito * FpsTimer::GetDeltaTime<float>(true));
		if (Input::IsKeyPressed(KeyCode::S)) m_position -= axisZ * (raito * FpsTimer::GetDeltaTime<float>(true));
		if (Input::IsKeyPressed(KeyCode::A)) m_position -= axisX * (raito * FpsTimer::GetDeltaTime<float>(true));
		if (Input::IsKeyPressed(KeyCode::D)) m_position += axisX * (raito * FpsTimer::GetDeltaTime<float>(true));
	}

	Matrix rotation = Matrix::CreateFromYawPitchRoll(DegToRad(m_rotation.y), DegToRad(m_rotation.x), DegToRad(m_rotation.z));
	Matrix trans = Matrix::CreateTranslation(m_position);
	SetCameraMatrix(rotation * trans);

	m_mousePosOld = m_mousePosNow;
}
