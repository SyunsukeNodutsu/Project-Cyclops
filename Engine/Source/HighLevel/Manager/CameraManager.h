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

	void Initialize();

	void Update();
	void SetToShader();

	void AddCameraList(std::shared_ptr<Camera> camera);

	const std::list<std::shared_ptr<Camera>>& GetCameraList() const { return m_spCameraList; }
	const std::shared_ptr<Camera>& GetUseCamera() const { return m_spUseCamera; }
	const std::shared_ptr<Camera> SearchCamera(const std::string& name) const;

	const bool IsNowDolly() const { return m_nowDolly; }

private:

	std::list<std::shared_ptr<Camera>>	m_spCameraList;	//カメラの一覧
	std::shared_ptr<Camera>				m_spUseCamera;	//使用しているカメラ
	std::shared_ptr<Camera>				m_spPrevCamera;	//切り替わり以前に使用していたカメラ
	std::shared_ptr<EditorCamera>		m_spEditCamera;	//編集カメラ
	bool								m_editMode;		//編集中かどうか(EditCameraを使用しているかどうか)

	ChangeMode							m_changeMode;	//切り替わりモード
	std::shared_ptr<Camera>				m_spDollyCamera;//切り替わりの際に使用するカメラ
	float								m_changeTime;	//カメラが切り替わるまでにかかる時間
	float								m_progress;		//切り替わりの進行具合
	bool								m_nowDolly;		//カメラが切り替わり中かどうか

private:

	void CheckPriority();
	void UpdateDollyCamera();

};
