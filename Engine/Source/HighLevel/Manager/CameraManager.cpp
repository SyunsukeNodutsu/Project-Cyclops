#include "CameraManager.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
CameraManager::CameraManager()
	: m_spCameraList()
	, m_spUseCamera(nullptr)
	, m_spPrevCamera(nullptr)
	, m_spEditCamera(nullptr)
	, m_editMode(true)
	, m_changeMode(ChangeMode::Immediate)
	, m_dollyTimer()
	, m_spDollyCamera(nullptr)
	, m_changeTime(1.0f)
	, m_progress(0.0f)
	, m_nowDolly(false)
	, m_dollyRow(false)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void CameraManager::Initialize()
{
	//編集カメラ初期化
	m_spEditCamera = std::make_shared<EditorCamera>();
	if (m_spEditCamera)
	{
		m_spEditCamera->m_name = "EditorCamera";
		m_spEditCamera->m_priority = 100.0f;
		m_spEditCamera->SetCameraMatrix(Matrix::CreateTranslation(0, 0, -4));

		AddCameraList(m_spEditCamera);
	}

	//ドリーカメラ初期化
	m_spDollyCamera = std::make_shared<Camera>();
	if (m_spDollyCamera)
	{
		m_spDollyCamera->m_name = "DollyCamera";
	}

	m_spPrevCamera = m_spEditCamera;
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void CameraManager::Update()
{
	if (Input::IsKeyDown(KeyCode::ESC))
	{
		m_spEditCamera->m_priority = m_editMode ? FLT_MIN : FLT_MAX;
		m_editMode = !m_editMode;
	}

	if (!m_nowDolly)
	{
		CheckPriority();
		m_spUseCamera->Update();
	}
	else
	{
		UpdateDollyCamera();
	}
}

//-----------------------------------------------------------------------------
// 定数バッファをシェーダにセット
//-----------------------------------------------------------------------------
void CameraManager::SetToShader()
{
	if (m_spUseCamera == nullptr) return;

	if (m_nowDolly) m_spDollyCamera->SetToShader();
	else m_spUseCamera->SetToShader();
}

//-----------------------------------------------------------------------------
// カメラをリストに追加
//-----------------------------------------------------------------------------
void CameraManager::AddCameraList(std::shared_ptr<Camera> spCamera)
{
	if (spCamera == nullptr) return;
	m_spCameraList.push_back(spCamera);
	CheckPriority();
}

//-----------------------------------------------------------------------------
// カメラを名前検索
//-----------------------------------------------------------------------------
const std::shared_ptr<Camera> CameraManager::SearchCamera(const std::string& name) const
{
	for (auto& camera : m_spCameraList)
	{
		if (camera->m_name.compare(name) == 0)
			return camera;
	}
	Debug::LogError("Camera Not found.");
	return nullptr;
}

//-----------------------------------------------------------------------------
// 優先度の確認
//-----------------------------------------------------------------------------
void CameraManager::CheckPriority()
{
	if (m_spCameraList.size() < 1) return;

	for (float priorityTmp = FLT_MIN; auto& camera : m_spCameraList)
	{
		//優先度が高い方(以上)を設定
		if (camera->m_priority >= priorityTmp)
		{
			priorityTmp = camera->m_priority;
			m_spUseCamera = camera;
		}
	}

	if (m_spUseCamera == nullptr) { Debug::LogError("Camera Not found."); return; }

	if (m_spPrevCamera && (m_spUseCamera != m_spPrevCamera))
	{
		m_spPrevCamera->OnUseEnd();

		//ドリー開始
		m_spDollyCamera->SetCameraMatrix(m_spPrevCamera->GetCameraMatrix());
		m_dollyTimer.Record();
		m_nowDolly = true;
	}
}

//-----------------------------------------------------------------------------
// カメラの切り替え更新
//-----------------------------------------------------------------------------
void CameraManager::UpdateDollyCamera()
{
	if (!m_nowDolly) return;

	const Matrix& startMatrix = m_spPrevCamera->GetCameraMatrix();
	const Matrix& endMatrix = m_spUseCamera->GetCameraMatrix();

	const Vector3& startPos = startMatrix.GetTranslation();
	const Vector3& endPos = endMatrix.GetTranslation();

	//座標補間
	const Vector3& dir = endPos - startPos;
	const Vector3& nowPos = startPos + (dir * m_progress);

	//回転補間
	const DirectX::XMVECTOR& qStart = DirectX::XMQuaternionRotationMatrix(startMatrix);
	const DirectX::XMVECTOR& qEnd = DirectX::XMQuaternionRotationMatrix(endMatrix);

	const DirectX::XMVECTOR& qSlerp = DirectX::XMQuaternionSlerp(qStart, qEnd, m_progress);
	const Matrix& rotMatrix = DirectX::XMMatrixRotationQuaternion(qSlerp);

	Matrix cameraMatrix = rotMatrix;
	cameraMatrix.SetTranslation(nowPos);

	//ドリーカメラのカメラ行列確定
	m_spDollyCamera->SetCameraMatrix(cameraMatrix);

	//進行度更新
	float t = m_dollyTimer.GetElapsedSeconds<float>() * (m_dollyRow ? 1.0f : FpsTimer::GetTimeScale<float>());

	switch (m_changeMode)
	{
	case ChangeMode::Immediate:		m_progress = 1.0f; break;
	case ChangeMode::Liner:			m_progress = Easing::Linear(t, m_changeTime, 0.0f, 1.0f);		break;
	case ChangeMode::QuadIn:		m_progress = Easing::QuadIn(t, m_changeTime, 0.0f, 1.0f);		break;
	case ChangeMode::QuadOut:		m_progress = Easing::QuadOut(t, m_changeTime, 0.0f, 1.0f);		break;
	case ChangeMode::QuadInOut:		m_progress = Easing::QuadInOut(t, m_changeTime, 0.0f, 1.0f);	break;
	case ChangeMode::CubicIn:		m_progress = Easing::CubicIn(t, m_changeTime, 0.0f, 1.0f);		break;
	case ChangeMode::CubicOut:		m_progress = Easing::CubicOut(t, m_changeTime, 0.0f, 1.0f);		break;
	case ChangeMode::CubicInOut:	m_progress = Easing::CubicInOut(t, m_changeTime, 0.0f, 1.0f);	break;
	}

	if (m_progress >= 1.0f)
	{
		//切り替え終了
		m_spUseCamera->OnUseStart();

		m_progress = 0.0f;
		m_nowDolly = false;

		Debug::Log("Change done: " + m_spUseCamera->m_name + ", prev: " + m_spPrevCamera->m_name);
		m_spPrevCamera = m_spUseCamera;
	}
}
