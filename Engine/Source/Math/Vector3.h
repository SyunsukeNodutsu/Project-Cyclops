//-----------------------------------------------------------------------------
// File: Vector3.h
//
// 3Dベクトル DirectX::XMFLOAT3のラッパー
//-----------------------------------------------------------------------------
#pragma once

//3Dベクトルクラス
class Vector3 : public DirectX::XMFLOAT3
{
public:

	Vector3() { x = y = z = 0.0f; }
	Vector3(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
	Vector3(const DirectX::XMFLOAT3& v) { x = v.x; y = v.y; z = v.z; }
	Vector3(const DirectX::XMVECTOR& v) { DirectX::XMStoreFloat3(this, v); }

	Vector3& operator+= (Vector3 v) { *this = DirectX::XMVectorAdd(*this, v); return *this; }
	Vector3& operator-= (Vector3 v) { *this = DirectX::XMVectorSubtract(*this, v); return *this; }
	Vector3& operator*= (float s) { *this = DirectX::XMVectorScale(*this, s); return *this; }

	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat3(this); }

	Vector3& Normalize() { *this = DirectX::XMVector3Normalize(*this); return *this; }

	float Length() const { return DirectX::XMVector3Length(*this).m128_f32[0]; }
	float LengthSquared() const { return DirectX::XMVector3LengthSq(*this).m128_f32[0]; }

	static float Dot(const Vector3& v1, const Vector3& v2) { return DirectX::XMVector3Dot(v1, v2).m128_f32[0]; }
	static Vector3 Cross(const Vector3& v1, const Vector3& v2) { return DirectX::XMVector3Cross(v1, v2); }

};
