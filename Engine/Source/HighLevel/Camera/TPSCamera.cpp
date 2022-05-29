#include "TPSCamera.h"

//-----------------------------------------------------------------------------
// カメラ行列を設定する
//-----------------------------------------------------------------------------
void TPSCamera::SetCameraMatrix(const Matrix& worldMatrix)
{
	//視線調整用の移動行列
	Matrix view_trans = Matrix::CreateTranslation(m_viewPoint);

	Matrix local_pos = Matrix::CreateTranslation(m_localPos);
	Matrix rotation = GetRotationMatrix();

	Camera::SetCameraMatrix(local_pos * rotation * view_trans * worldMatrix);
}
