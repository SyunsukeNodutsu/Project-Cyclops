//-----------------------------------------------------------------------------
// File: Vector2.h
//
// 2D�x�N�g�� DirectX::XMFLOAT2�̃��b�p�[
//-----------------------------------------------------------------------------
#pragma once

//2D�x�N�g���N���X
class Vector2 : public DirectX::XMFLOAT2
{
public:

	Vector2() { x = y = 0.0f; }
	Vector2(float _x, float _y) { x = _x; y = _y; }
	Vector2(const DirectX::XMFLOAT2& v) { x = v.x; y = v.y; }
	Vector2(const DirectX::XMVECTOR& v) { DirectX::XMStoreFloat2(this, v); }

	Vector2& operator+= (Vector2 v) { *this = DirectX::XMVectorAdd(*this, v); return *this; }
	Vector2& operator-= (Vector2 v) { *this = DirectX::XMVectorSubtract(*this, v); return *this; }
	Vector2& operator*= (float s) { *this = DirectX::XMVectorScale(*this, s); return *this; }

	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat2(this); }

	Vector2& Normalize() { *this = DirectX::XMVector2Normalize(*this); return *this; }

	float Length() const { return DirectX::XMVector2Length(*this).m128_f32[0]; }
	float LengthSquared() const { return DirectX::XMVector2LengthSq(*this).m128_f32[0]; }

	static float Dot(const Vector2& v1, const Vector2& v2) { return DirectX::XMVector2Dot(v1, v2).m128_f32[0]; }
	static Vector2 Cross(const Vector2& v1, const Vector2& v2) { return DirectX::XMVector2Cross(v1, v2); }

};
