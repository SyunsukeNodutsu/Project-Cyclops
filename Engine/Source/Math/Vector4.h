//-----------------------------------------------------------------------------
// File: Vector4.h
//
// 4Dベクトル DirectX::XMFLOAT4のラッパー
//-----------------------------------------------------------------------------
#pragma once

//4Dベクトルクラス
class Vector4 : public DirectX::XMFLOAT4
{
public:

	Vector4() { x = y = z = w = 0.0f; }
	Vector4(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
	Vector4(const DirectX::XMFLOAT4& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
	Vector4(const DirectX::XMVECTOR& v) { DirectX::XMStoreFloat4(this, v); }

	Vector4& operator+= (Vector4 v) { *this = DirectX::XMVectorAdd(*this, v); return *this; }
	Vector4& operator-= (Vector4 v) { *this = DirectX::XMVectorSubtract(*this, v); return *this; }
	Vector4& operator*= (float s) { *this = DirectX::XMVectorScale(*this, s); return *this; }

	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat4(this); }

	Vector4& Normalize() { *this = DirectX::XMVector4Normalize(*this); return *this; }

	float Length() const { return DirectX::XMVector4Length(*this).m128_f32[0]; }
	float LengthSquared() const { return DirectX::XMVector4LengthSq(*this).m128_f32[0]; }

	static float Dot(const DirectX::XMVECTOR& v1, const DirectX::XMVECTOR& v2) { return DirectX::XMVector4Dot(v1, v2).m128_f32[0]; }
	static Vector4 Cross(const DirectX::XMVECTOR& v1, const DirectX::XMVECTOR& v2, const DirectX::XMVECTOR& v3) { return DirectX::XMVector4Cross(v1, v2, v3); }

	static const Vector4 Zero;
	static const Vector4 One;

};
