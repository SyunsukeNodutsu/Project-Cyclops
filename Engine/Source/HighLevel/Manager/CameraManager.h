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
	QuadIn,
	QuadOut,
	QuadInOut,
	CubicIn,
	CubicOut,
	CubicInOut,
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

	void SetChangeMode(ChangeMode mode) { m_changeMode = mode; }
	void SetChangeTime(float time) { m_changeTime = time; }
	void SetDollyRow(bool row) { m_dollyRow = row; }

private:

	std::list<std::shared_ptr<Camera>>	m_spCameraList;	//カメラの一覧
	std::shared_ptr<Camera>				m_spUseCamera;	//使用しているカメラ
	std::shared_ptr<Camera>				m_spPrevCamera;	//切り替わり以前に使用していたカメラ
	std::shared_ptr<EditorCamera>		m_spEditCamera;	//編集カメラ
	bool								m_editMode;		//編集中かどうか(EditCameraを使用しているかどうか)

	ChangeMode							m_changeMode;	//切り替わりモード
	CommonTimer							m_dollyTimer;	//切り替えの時間計測タイマ
	std::shared_ptr<Camera>				m_spDollyCamera;//切り替わりの際に使用するカメラ
	float								m_changeTime;	//カメラが切り替わるまでにかかる時間
	float								m_progress;		//切り替わりの進行具合
	bool								m_nowDolly;		//カメラが切り替わり中かどうか
	bool								m_dollyRow;		//切り替わりが時間のスケーリングの影響を受けるかどうか

private:

	void CheckPriority();
	void UpdateDollyCamera();

};
