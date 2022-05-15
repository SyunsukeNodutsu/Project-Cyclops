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

	//TODO: 算術系追加

};