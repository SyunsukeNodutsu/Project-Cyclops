//-----------------------------------------------------------------------------
// File: Vector3Int.h
//
// INT型の3Dベクトル DirectX::XMINT3のラッパー
//-----------------------------------------------------------------------------
#pragma once

//INT型3Dベクトルクラス
class Vector3Int : public DirectX::XMINT3
{
public:

	Vector3Int() { x = y = z = 0; }
	Vector3Int(int v) { x = y = z = v; }
	Vector3Int(int _x, int _y, int _z) { x = _x; y = _y; z = _z; }
	Vector3Int(const DirectX::XMINT3& v) { x = v.x; y = v.y; z = v.z; }
	Vector3Int(const DirectX::XMVECTOR& v) { DirectX::XMStoreSInt3(this, v); }

	Vector3Int& operator+= (Vector3Int v) { *this = DirectX::XMVectorAdd(*this, v); return *this; }
	Vector3Int& operator-= (Vector3Int v) { *this = DirectX::XMVectorSubtract(*this, v); return *this; }
	Vector3Int& operator*= (float s) { *this = DirectX::XMVectorScale(*this, s); return *this; }

	operator DirectX::XMVECTOR() const { return DirectX::XMLoadSInt3(this); }

	int Length() const { return DirectX::XMVector3Length(*this).m128_i32[0]; }
	int LengthSquared() const { return DirectX::XMVector3LengthSq(*this).m128_i32[0]; }

	static int Dot(const Vector3Int& v1, const Vector3Int& v2) { return DirectX::XMVector3Dot(v1, v2).m128_i32[0]; }
	static Vector3Int Cross(const Vector3Int& v1, const Vector3Int& v2) { return DirectX::XMVector3Cross(v1, v2); }

};
