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
const matrix4x4& Transform::GetWorldMatrix() const
{
	if (m_needUpdateMatrix)
	{
		m_needUpdateMatrix = false;

		matrix4x4 scale;
		scale.CreateScale(m_worldScale);

		matrix4x4 trans;
		trans.CreateTranslation(m_worldPosition);

		matrix4x4 rotation;
		rotation.CreateFromYawPitchRoll(DegToRad(m_worldAngle.y), DegToRad(m_worldAngle.x), DegToRad(m_worldAngle.z));

		m_worldMatrix = scale * rotation * trans;
	}

	return m_worldMatrix;
}

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
void Transform::SetWorldMatrix(const matrix4x4& matrix)
{
	m_worldMatrix = matrix;

	m_needUpdateMatrix = false;

	m_worldScale.x = m_worldMatrix.Left().Length();
	m_worldScale.y = m_worldMatrix.Up().Length();
	m_worldScale.z = m_worldMatrix.Forward().Length();

	m_worldPosition = m_worldMatrix.Translation();

	matrix4x4 mRota;
	mRota.CreateFromYawPitchRoll(DegToRad(m_worldAngle.y), DegToRad(m_worldAngle.x), DegToRad(m_worldAngle.z));

	m_worldAngle.x = atan2f(mRota.m[1][2], mRota.m[2][2]);
	m_worldAngle.y = atan2f(-mRota.m[0][2], sqrtf(mRota.m[1][2] * mRota.m[1][2] + mRota.m[2][2] * mRota.m[2][2]));
	m_worldAngle.z = atan2f(mRota.m[0][1], mRota.m[0][0]);
}
