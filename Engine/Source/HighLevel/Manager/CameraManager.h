//-----------------------------------------------------------------------------
// File: CameraManager.h
//
// カメラマネージャー 複数のカメラを管理
//-----------------------------------------------------------------------------
#pragma once

//カメラの遷移モード
enum class ChangeMode
{
	Immediate,	//遷移アニメーションなし
	Liner,		//線形
	//TODO: 追加
};

//カメラのマネージャークラス
class CameraManager
{
public:

	CameraManager();
	~CameraManager() = default;

	void Update();
	void SetToShader();

	void AddCameraList(std::shared_ptr<Camera> camera);

	const std::list<std::shared_ptr<Camera>>& GetCameraList() const { return m_spCameraList; }
	const std::shared_ptr<Camera>& GetUseCamera() const { return m_spUseCamera; }
	const std::shared_ptr<Camera> SearchCamera(const std::string& name) const;

private:

	std::list<std::shared_ptr<Camera>>	m_spCameraList;
	std::shared_ptr<Camera>				m_spUseCamera;
	std::shared_ptr<EditorCamera>		m_spEditCamera;
	bool								m_editMode;

private:

	void CheckPriority();

};
