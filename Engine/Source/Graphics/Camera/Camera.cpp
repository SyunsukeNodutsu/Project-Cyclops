#include "Camera.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Camera::Camera()
	: m_name("Camera Main")
	, m_priority(FLT_MIN)
	, m_enable(true)
	, m_cameraMatrix()
	, m_viewMatrix()
	, m_projMatrix()
	, m_viewProjMatrix()
	, m_position()
	, m_fovAngleY(DegToRad(60.0f))
	, m_aspectRatio(16.0f / 9.0f)
	, m_nearZ(0.01f)
	, m_farZ(5000.0f)
	, m_viewWidth(1280.0f)
	, m_viewHeight(780.0f)
	, m_dirtyCamera(true)
	, m_dirtyProj(true)
{
	m_projMatrix = DirectX::XMMatrixPerspectiveFovLH(m_fovAngleY, m_aspectRatio, m_nearZ, m_farZ);
	SetCameraMatrix(Matrix());
}

//-----------------------------------------------------------------------------
// 定数バッファセット
//-----------------------------------------------------------------------------
void Camera::SetToShader()
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpContext == nullptr) return;
	if (m_graphicsDevice->m_spRendererStatus == nullptr) return;

	if (m_dirtyCamera)
	{
		m_position = m_cameraMatrix.GetTranslation();

		m_viewProjMatrix = m_viewMatrix * m_projMatrix;

		m_graphicsDevice->m_spRendererStatus->m_cb5Camera.Work().m_cameraMatrix = m_cameraMatrix;
		m_graphicsDevice->m_spRendererStatus->m_cb5Camera.Work().m_viewMatrix = m_viewMatrix;
		m_graphicsDevice->m_spRendererStatus->m_cb5Camera.Write();
	}

	if (m_dirtyProj)
	{
		m_projMatrix = DirectX::XMMatrixPerspectiveFovLH(m_fovAngleY, m_aspectRatio, m_nearZ, m_farZ);
		m_viewProjMatrix = m_viewMatrix * m_projMatrix;

		m_graphicsDevice->m_spRendererStatus->m_cb5Camera.Work().m_projMatrix = m_projMatrix;
		m_graphicsDevice->m_spRendererStatus->m_cb5Camera.Write();
	}
}

//-----------------------------------------------------------------------------
// ワールド座標 ->スクリーン座標
//-----------------------------------------------------------------------------
void Camera::WorldToScreen(const Vector3& pos, const Matrix matrix, Vector2& screen)
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpContext == nullptr) return;

	// ビューポートを取得する
	const auto& vp = m_graphicsDevice->m_viewport;

	const float HalfViewportWidth = vp.Width * 0.5f;
	const float HalfViewportHeight = vp.Height * 0.5f;

	Vector3 Scale = DirectX::XMVectorSet(HalfViewportWidth, -HalfViewportHeight, vp.MaxDepth - vp.MinDepth, 0.0f);
	Vector3 Offset = DirectX::XMVectorSet(vp.TopLeftX + HalfViewportWidth, vp.TopLeftY + HalfViewportHeight, vp.MinDepth, 0.0f);

	Matrix Transform = XMMatrixMultiply(matrix, GetViewMatrix());
	Transform = XMMatrixMultiply(Transform, GetProjMatrix());

	Vector3 Pos = { GetCameraMatrix()._41,GetCameraMatrix()._42 ,GetCameraMatrix()._43 };
	Vector3 Result = XMVector3TransformCoord(pos, Transform);

	Result = XMVectorMultiplyAdd(Result, Scale, Offset);
	screen = Vector2(Result.x, Result.y);
}

//-----------------------------------------------------------------------------
// カメラ/ビュー行列の設定
//-----------------------------------------------------------------------------
void Camera::SetCameraMatrix(const Matrix& camera)
{
	m_cameraMatrix = camera;
	m_viewMatrix = m_cameraMatrix;
	m_viewMatrix.Inverse();
	m_dirtyCamera = true;

	//試錐台作成
	/*Quaternion quaternion = XMQuaternionRotationMatrix(m_cameraMatrix);
	BoundingFrustum::CreateFromMatrix(m_frustum, m_projMatrix);
	m_frustum.Origin = m_cameraMatrix.Translation();
	m_frustum.Orientation = quaternion;*/
}
