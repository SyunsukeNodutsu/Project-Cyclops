//-----------------------------------------------------------------------------
// File: Matrix.h
//
// 4x4行列 DirectX::XMFLOAT4X4のラッパー
//-----------------------------------------------------------------------------
#pragma once

//4x4行列クラス
class Matrix : public DirectX::XMFLOAT4X4
{
public:

	Matrix() { *this = DirectX::XMMatrixIdentity(); }
	Matrix(const DirectX::XMMATRIX& m) { DirectX::XMStoreFloat4x4(this, m); }
	Matrix(const DirectX::XMFLOAT4X4& m) { memcpy_s(this, sizeof(float) * 16, &m, sizeof(XMFLOAT4X4)); }

	operator DirectX::XMMATRIX() const { return DirectX::XMLoadFloat4x4(this); }

	Matrix& operator*= (const Matrix& m) { *this = DirectX::XMMatrixMultiply(*this, m); return *this; }

	//作成(TODO: staticでもいい気が)

	static Matrix CreateTranslation(float x, float y, float z) { return DirectX::XMMatrixTranslation(x, y, z); }
	static Matrix CreateTranslation(const Vector3& v) { return  DirectX::XMMatrixTranslation(v.x, v.y, v.z); }

	static Matrix CreateRotationX(float angle) { return  DirectX::XMMatrixRotationX(angle); }

	static Matrix CreateScalling(float x, float y, float z) { return  DirectX::XMMatrixScaling(x, y, z); }
	static Matrix CreateScalling(const Vector3& v) { return  DirectX::XMMatrixScaling(v.x, v.y, v.z); }

	static Matrix CreateRotationAxis(const Vector3& axis, float angle) { return  DirectX::XMMatrixRotationAxis(axis, angle); }
	static Matrix CreateFromYawPitchRoll(const float yaw, const float pitch, const float roll) { return  DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll); }
	static Matrix CreateFromQuaternion(const DirectX::XMVECTOR& q) { return  DirectX::XMMatrixRotationQuaternion(q); }

	Matrix& CreateProjectionPerspectiveFov(float fovAngleY, float aspectRatio, float nearZ, float farZ)
	{
		*this = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
		return *this;
	}
	Matrix& CreateProjection_Orthographic(float viewWidth, float viewHeight, float nearZ, float farZ)
	{
		*this = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
		return *this;
	}

	//操作

	void RotateX(float angle) { *this *= DirectX::XMMatrixRotationX(angle); }
	void RotateY(float angle) { *this *= DirectX::XMMatrixRotationY(angle); }
	void RotateZ(float angle) { *this *= DirectX::XMMatrixRotationZ(angle); }

	void Scale(float x, float y, float z) { *this *= DirectX::XMMatrixScaling(x, y, z); }
	void Scale(const Vector3& v) { *this *= DirectX::XMMatrixScaling(v.x, v.y, v.z); }

	void Move(float x, float y, float z) { _41 += x; _42 += y; _43 += z; }
	void Move(const Vector3& v) { _41 += v.x; _42 += v.y; _43 += v.z; }

	void Inverse() { *this = DirectX::XMMatrixInverse(nullptr, *this); }

	//プロパティ

	Vector3 GetAxisX() const { return Vector3(_11, _12, _13); }
	Vector3 GetAxisY() const { return Vector3(_21, _22, _23); }
	Vector3 GetAxisZ() const { return Vector3(_31, _32, _33); }

	void SetAxisX(const Vector3& v) { _11 = v.x; _12 = v.y; _13 = v.z; }
	void SetAxisY(const Vector3& v) { _21 = v.x; _22 = v.y; _23 = v.z; }
	void SetAxisZ(const Vector3& v) { _31 = v.x; _32 = v.y; _33 = v.z; }

	Vector3 GetTranslation() const { return Vector3(_41, _42, _43); }
	void SetTranslation(const Vector3& v) { _41 = v.x; _42 = v.y; _43 = v.z; }

};

inline Matrix operator* (const Matrix& m1, const Matrix& m2)
{
	return DirectX::XMMatrixMultiply(m1, m2);
}
