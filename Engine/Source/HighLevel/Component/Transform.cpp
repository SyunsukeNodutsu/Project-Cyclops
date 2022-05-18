#include "Transform.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Transform::Transform()
{
}

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
const Matrix& Transform::GetWorldMatrix() const
{
	if (m_needUpdateMatrix)
	{
		m_needUpdateMatrix = false;

		Matrix scale;
		scale.CreateScalling(m_worldScale);

		Matrix trans;
		trans.CreateTranslation(m_worldPosition);

		Matrix rotation;
		rotation.CreateFromYawPitchRoll(DegToRad(m_worldAngle.y), DegToRad(m_worldAngle.x), DegToRad(m_worldAngle.z));

		m_worldMatrix = scale * rotation * trans;
	}

	return m_worldMatrix;
}

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
void Transform::SetWorldMatrix(const Matrix& matrix)
{
	m_worldMatrix = matrix;

	m_needUpdateMatrix = false;

	m_worldScale.x = m_worldMatrix.GetAxisX().Length();
	m_worldScale.y = m_worldMatrix.GetAxisY().Length();
	m_worldScale.z = m_worldMatrix.GetAxisZ().Length();

	m_worldPosition = m_worldMatrix.GetTranslation();

	Matrix mRota;
	mRota.CreateFromYawPitchRoll(DegToRad(m_worldAngle.y), DegToRad(m_worldAngle.x), DegToRad(m_worldAngle.z));

	m_worldAngle.x = atan2f(mRota.m[1][2], mRota.m[2][2]);
	m_worldAngle.y = atan2f(-mRota.m[0][2], sqrtf(mRota.m[1][2] * mRota.m[1][2] + mRota.m[2][2] * mRota.m[2][2]));
	m_worldAngle.z = atan2f(mRota.m[0][1], mRota.m[0][0]);
}
