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
	, m_dollyRow(true)
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
		m_spEditCamera->m_priority = FLT_MAX;
		m_spEditCamera->SetCameraMatrix(matrix4x4::CreateTranslation(0, 0, -4));

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

	CheckPriority();

	if (m_nowDolly) UpdateDollyCamera();
	else m_spUseCamera->Update();
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
	if (m_nowDolly) return;//TODO: フェード同様 ドリー中も上書き可能に(今のままでもできるかも)
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

	const matrix4x4& mStart = m_spPrevCamera->GetCameraMatrix();
	const matrix4x4& mEnd = m_spUseCamera->GetCameraMatrix();

	const float3& vStart = mStart.Translation();
	const float3& vEnd = mEnd.Translation();

	//座標補間
	const float3& vDir = vEnd - vStart;
	const float3& vNow = vStart + (vDir * m_progress);

	//回転補間
	const quaternion& qStart = quaternion::CreateFromRotationMatrix(mStart);
	const quaternion& qEnd = quaternion::CreateFromRotationMatrix(mEnd);

	const quaternion& qSlerp = quaternion::Slerp(qStart, qEnd, m_progress);
	const matrix4x4& rotMatrix = matrix4x4::CreateFromQuaternion(qSlerp);

	//TODO: 画角などの補間も必要かも

	//ドリーカメラのカメラ行列確定
	matrix4x4 cameraMatrix = rotMatrix;
	cameraMatrix.Translation(vNow);
	m_spDollyCamera->SetCameraMatrix(cameraMatrix);

	//進行度更新
	float t = m_dollyTimer.GetElapsedSeconds<float>() * (m_dollyRow ? 1.0f : FpsTimer::GetTimeScale<float>());

	//TODO: 必要なイージングのみでいい
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

	if (m_progress >= 0.9999f)//桁落ち防止で1.0ちょうどではなく0.9999
	{
		//切り替え終了
		m_spUseCamera->OnUseStart();

		m_progress = 0.0f;
		m_nowDolly = false;

		m_spPrevCamera = m_spUseCamera;
	}
}
