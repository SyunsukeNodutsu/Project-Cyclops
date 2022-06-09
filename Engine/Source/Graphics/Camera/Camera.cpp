#include "Camera.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Camera::Camera()
	: m_name("Camera Main")
	, m_priority(FLT_MIN)
	, m_cameraMatrix()
	, m_viewMatrix()
	, m_projMatrix()
	, m_viewProjMatrix()
	, m_frustum()
	, m_up(float3::Zero)
	, m_down(float3::Zero)
	, m_forward(float3::Zero)
	, m_backward(float3::Zero)
	, m_left(float3::Zero)
	, m_right(float3::Zero)
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
	SetCameraMatrix(matrix4x4::CreateTranslation(float3::Zero));
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
void Camera::WorldToScreen(const float3& pos, const matrix4x4 matrix, float2& screen)
{
	if (m_graphicsDevice == nullptr) return;
	if (m_graphicsDevice->m_cpContext == nullptr) return;

	const auto& vp = m_graphicsDevice->m_viewport;
	const float vpWidthHalf = vp.Width * 0.5f;
	const float vpHeightHalf = vp.Height * 0.5f;

	float3 scale = DirectX::XMVectorSet(vpWidthHalf, -vpHeightHalf, vp.MaxDepth - vp.MinDepth, 0.0f);
	float3 offset = DirectX::XMVectorSet(vp.TopLeftX + vpWidthHalf, vp.TopLeftY + vpHeightHalf, vp.MinDepth, 0.0f);

	matrix4x4 transform = XMMatrixMultiply(matrix, GetViewMatrix());
	transform = XMMatrixMultiply(transform, GetProjMatrix());

	float3 cameraPos = { GetCameraMatrix()._41,GetCameraMatrix()._42 ,GetCameraMatrix()._43 };
	float3 result = DirectX::XMVector3TransformCoord(cameraPos, transform);

	result = DirectX::XMVectorMultiplyAdd(result, scale, offset);
	screen = float2(result.x, result.y);
}

//-----------------------------------------------------------------------------
// カメラ/ビュー行列の設定
//-----------------------------------------------------------------------------
void Camera::SetCameraMatrix(const matrix4x4& camera)
{
	m_dirtyCamera = true;

	m_cameraMatrix = camera;
	m_viewMatrix = m_cameraMatrix.Invert();

	//方向
	m_up		= m_cameraMatrix.Up();
	m_down		= m_cameraMatrix.Down();
	m_forward	= m_cameraMatrix.Forward();
	m_backward	= m_cameraMatrix.Backward();
	m_left		= m_cameraMatrix.Left();
	m_right		= m_cameraMatrix.Right();

	//試錐台作成
	/*DirectX::BoundingFrustum::CreateFromMatrix(m_frustum, m_projMatrix);
	m_frustum.Origin = m_cameraMatrix.Translation();
	m_frustum.Orientation = quaternion::CreateFromRotationMatrix(m_cameraMatrix);*/

	//quaternion q = XMQuaternionRotationMatrix(m_cameraMatrix);
	//m_frustum.Orientation	= q;
}
