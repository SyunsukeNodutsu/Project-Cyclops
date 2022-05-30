//-----------------------------------------------------------------------------
// File: Camera.h
//
// カメラ
//-----------------------------------------------------------------------------
#pragma once

//カメラクラス
class Camera : public GraphicsDeviceChild
{
public:

	Camera();
	virtual ~Camera() = default;

	void SetToShader();
	void WorldToScreen(const Vector3& pos, const Matrix matrix, Vector2& screen);

	virtual void SetCameraMatrix(const Matrix& camera);

	virtual void Update() {}
	virtual void OnUseStart() {}
	virtual void OnUseEnd() {}

	void SetFovAngleY(float radians) { m_fovAngleY = radians; m_dirtyProj = true; }
	void SetAspect(float aspectRatio) { m_aspectRatio = aspectRatio; m_dirtyProj = true; }
	void SetNearZ(float nearZ) { m_nearZ = nearZ; m_dirtyProj = true; }
	void SetFarZ(float farZ) { m_farZ = farZ; m_dirtyProj = true; }
	void SetViewWidth(float viewWidth) { m_viewWidth = viewWidth; m_dirtyProj = true; }
	void SetViewHeight(float viewHeight) { m_viewHeight = viewHeight; m_dirtyProj = true; }

	inline const Matrix& GetCameraMatrix() const { return m_cameraMatrix; }
	inline const Matrix& GetViewMatrix() const { return m_viewMatrix; }
	inline const Matrix& GetProjMatrix() const { return m_projMatrix; }
	inline const Matrix& GetViewProjMatrix() const { return m_viewProjMatrix; }

	inline const Vector3& Up() const { return m_up; }
	inline const Vector3& Down() const { return m_down; }
	inline const Vector3& Forward() const { return m_forward; }
	inline const Vector3& Backward() const { return m_backward; }
	inline const Vector3& Left() const { return m_left; }
	inline const Vector3& Right() const { return m_right; }

	float GetFovAngleY() const { return m_fovAngleY; }
	float GetAspect() const { return m_aspectRatio; }
	float GetNearZ() const { return m_nearZ; }
	float GetFarZ() const { return m_farZ; }
	float GetViewWidth() const { return m_viewWidth; }
	float GetViewHeight() const { return m_viewHeight; }

public:

	std::string m_name;			//カメラの名前
	float		m_priority;		//優先度

private:

	//各行列
	Matrix	m_cameraMatrix;		//カメラ行列
	Matrix	m_viewMatrix;		//ビュー行列
	Matrix	m_projMatrix;		//射影行列
	Matrix	m_viewProjMatrix;	//ビュープロジェクション行列

	//方向
	Vector3 m_up, m_down;
	Vector3 m_forward, m_backward;
	Vector3 m_left, m_right;

	//射影関連
	float	m_fovAngleY;		//画角(ラジアン)
	float	m_aspectRatio;		//アスペクト比率
	float	m_nearZ;			//近平面
	float	m_farZ;				//遠平面

	//平行投影行列の場合
	float	m_viewWidth;		//幅
	float	m_viewHeight;		//高さ

	//ダーティーフラグ
	bool	m_dirtyCamera;		//カメラ行列が変更
	bool	m_dirtyProj;		//射影行列が変更

};
