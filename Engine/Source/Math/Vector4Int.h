//-----------------------------------------------------------------------------
// File: Vector4Int.h
//
// INT型の4Dベクトル DirectX::XMINT4のラッパー
//-----------------------------------------------------------------------------
#pragma once

//INT型4Dベクトルクラス
class Vector4Int : public DirectX::XMINT4
{
public:

	Vector4Int() { x = y = z = w = 0; }
	Vector4Int(int v) { x = y = z = w = v; }
	Vector4Int(int _x, int _y, int _z, int _w) { x = _x; y = _y; z = _z; w = _w; }
	Vector4Int(const DirectX::XMINT4& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
	Vector4Int(const DirectX::XMVECTOR& v) { DirectX::XMStoreSInt4(this, v); }

	Vector4Int& operator+= (Vector4Int v) { *this = DirectX::XMVectorAdd(*this, v); return *this; }
	Vector4Int& operator-= (Vector4Int v) { *this = DirectX::XMVectorSubtract(*this, v); return *this; }
	Vector4Int& operator*= (float s) { *this = DirectX::XMVectorScale(*this, s); return *this; }

	operator DirectX::XMVECTOR() const { return DirectX::XMLoadSInt4(this); }

	int Length() const { return DirectX::XMVector4Length(*this).m128_i32[0]; }
	int LengthSquared() const { return DirectX::XMVector4LengthSq(*this).m128_i32[0]; }

	static int Dot(const Vector4Int& v1, const Vector4Int& v2) { return DirectX::XMVector4Dot(v1, v2).m128_i32[0]; }
	static Vector4Int Cross(const Vector4Int& v1, const Vector4Int& v2, const DirectX::XMVECTOR& v3) { return DirectX::XMVector4Cross(v1, v2, v3); }

};
