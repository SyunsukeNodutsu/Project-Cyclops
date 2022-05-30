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

	static Quaternion CreateFromMatrix(const DirectX::XMMATRIX& m) { return DirectX::XMQuaternionRotationMatrix(m); }

	static Quaternion Slerp(const Quaternion& s, const Quaternion& e, float t) { return DirectX::XMQuaternionSlerp(s, e, t); }

};
