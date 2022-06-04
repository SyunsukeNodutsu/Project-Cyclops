#include "TPSCamera.h"

//-----------------------------------------------------------------------------
// カメラ行列を設定する
//-----------------------------------------------------------------------------
void TPSCamera::SetCameraMatrix(const matrix4x4& worldMatrix)
{
	//視線調整用の移動行列
	matrix4x4 view_trans = matrix4x4::CreateTranslation(m_viewPoint);

	matrix4x4 local_pos = matrix4x4::CreateTranslation(m_localPos);
	matrix4x4 rotation = GetRotationMatrix();

	Camera::SetCameraMatrix(local_pos * rotation * view_trans * worldMatrix);
}
