//-----------------------------------------------------------------------------
// File: Transform.h
//
// オブジェクトの位置,回転.スケールを扱う Objectに最初から付与
//-----------------------------------------------------------------------------
#pragma once

//位置,回転.スケールを扱うクラス
class Transform
{
public:

	Transform();
	~Transform() = default;

	const Matrix& GetWorldMatrix() const;
	Vector3 GetPosition() const { return m_worldPosition; }
	Vector3 GetAngle() const { return m_worldAngle; }
	Vector3 GetScale() const { return m_worldScale; }

	void SetWorldMatrix(const Matrix& matrix);

	void SetPosition(const Vector3& position) { m_worldPosition = position; m_needUpdateMatrix = true; }
	void SetPosition(const float x, const float y, const float z) { m_worldPosition = Vector3(x, y, z); m_needUpdateMatrix = true; }

	void SetAngle(const Vector3& angle) { m_worldAngle = angle; m_needUpdateMatrix = true; }

	void SetScale(const Vector3& scale) { m_worldScale = scale; m_needUpdateMatrix = true; }
	void SetScale(const float scale) { m_worldScale = Vector3(scale, scale, scale); m_needUpdateMatrix = true; }

private:

	Vector3 m_worldPosition;	//ワールド行列を構成する座標ベクトル
	Vector3 m_worldAngle;		//ワールド行列を構成する回転ベクトル
	Vector3 m_worldScale;		//ワールド行列を構成する拡大

	mutable Matrix m_worldMatrix;
	mutable bool m_needUpdateMatrix;//行列を更新する必要があるか？

};
