//-----------------------------------------------------------------------------
// File: Vector2Int.h
//
// INT型の2Dベクトル DirectX::XMINT2のラッパー
//-----------------------------------------------------------------------------
#pragma once

//INT型2Dベクトルクラス
class Vector2Int : public DirectX::XMINT2
{
public:

	Vector2Int() { x = y = 0; }
	Vector2Int(int v) { x = y = v; }
	Vector2Int(int _x, int _y) { x = _x; y = _y; }
	Vector2Int(const DirectX::XMINT2& v) { x = v.x; y = v.y; }
	Vector2Int(const DirectX::XMVECTOR& v) { DirectX::XMStoreSInt2(this, v); }

	Vector2Int& operator+= (Vector2Int v) { *this = DirectX::XMVectorAdd(*this, v); return *this; }
	Vector2Int& operator-= (Vector2Int v) { *this = DirectX::XMVectorSubtract(*this, v); return *this; }
	Vector2Int& operator*= (float s) { *this = DirectX::XMVectorScale(*this, s); return *this; }

	operator DirectX::XMVECTOR() const { return DirectX::XMLoadSInt2(this); }

	int Length() const { return DirectX::XMVector2Length(*this).m128_i32[0]; }
	int LengthSquared() const { return DirectX::XMVector2LengthSq(*this).m128_i32[0]; }

	static int Dot(const Vector2Int& v1, const Vector2Int& v2) { return DirectX::XMVector2Dot(v1, v2).m128_i32[0]; }
	static Vector2Int Cross(const Vector2Int& v1, const Vector2Int& v2) { return DirectX::XMVector2Cross(v1, v2); }

};
