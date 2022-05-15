//-----------------------------------------------------------------------------
// File: Matrix.h
//
// クォータニオン DirectX::XMFLOAT4のラッパー
//-----------------------------------------------------------------------------
#pragma once

//クォータニオンクラス
class Quaternion : public DirectX::XMFLOAT4
{
public:

	Quaternion() { x = y = z = 0.0f; w = 1.0f; }
	Quaternion(const DirectX::XMVECTOR& v) { DirectX::XMStoreFloat4(this, v); }

	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat4(this); }

	Quaternion& CreateFromMatrix(const DirectX::XMMATRIX& m) { *this = DirectX::XMQuaternionRotationMatrix(m); return *this; }

};
