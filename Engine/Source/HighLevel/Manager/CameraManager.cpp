#include "CameraManager.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
CameraManager::CameraManager()
	: m_spCameraList()
	, m_spUseCamera(nullptr)
	, m_spEditCamera(nullptr)
	, m_editMode(false)
{
	m_spEditCamera = std::make_shared<EditorCamera>();
	if (m_spEditCamera)
	{
		m_spEditCamera->m_name = "EditorCamera";
		m_spEditCamera->m_priority = 100.0f;
		m_spEditCamera->m_enable = true;
		m_spEditCamera->SetCameraMatrix(Matrix::CreateTranslation(0, 0, -4));

		AddCameraList(m_spEditCamera);
	}
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

	if (m_editMode) m_spEditCamera->Update();

	CheckPriority();
}

//-----------------------------------------------------------------------------
// 定数バッファをシェーダにセット
//-----------------------------------------------------------------------------
void CameraManager::SetToShader()
{
	if (m_spUseCamera == nullptr) return;
	m_spUseCamera->SetToShader();
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
	Debug::LogError("Camera not found.");
	return nullptr;
}

//-----------------------------------------------------------------------------
// 優先度の確認
//-----------------------------------------------------------------------------
void CameraManager::CheckPriority()
{
	float priorityTmp = FLT_MIN;
	for (auto& camera : m_spCameraList)
	{
		camera->m_enable = false;

		//優先度が高い方(以上)を設定
		if (camera->m_priority >= priorityTmp)
		{
			priorityTmp = camera->m_priority;
			m_spUseCamera = camera;
		}
	}

	if (m_spUseCamera)
		m_spUseCamera->m_enable = true;
}
