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
	Vector3& TransformNormal(const DirectX::XMMATRIX& m) { return *this = XMVector3TransformNormal(*this, m); }//指定(回転)行列でVectorを変換する

	float Length() const { return DirectX::XMVector3Length(*this).m128_f32[0]; }
	float LengthSquared() const { return DirectX::XMVector3LengthSq(*this).m128_f32[0]; }

	static float Dot(const Vector3& v1, const Vector3& v2) { return DirectX::XMVector3Dot(v1, v2).m128_f32[0]; }
	static Vector3 Cross(const Vector3& v1, const Vector3& v2) { return DirectX::XMVector3Cross(v1, v2); }
	

	//自身をaxis方向にrad分向ける
	void Complement(const Vector3& axis, float rad)
	{
		//この軸で回転する
		Vector3 rot_axis = Vector3::Cross(*this, axis);

		//ベクトルの内積
		float dot = Vector3::Dot(*this, axis);
		dot = std::clamp(dot, -1.0f, 1.0f);

		//ベクトル間の角度(必ず正の値)
		float radian = acos(dot);
		dot = std::clamp(radian, 0.0f, rad);

		DirectX::XMMATRIX matrix{};
		matrix = DirectX::XMMatrixRotationAxis(rot_axis, radian);

		//軸回転行列をベクトルに変換
		TransformNormal(matrix);
	}

	static const Vector3 Zero;
	static const Vector3 One;
	static const Vector3 UnitX;
	static const Vector3 UnitY;
	static const Vector3 UnitZ;
	static const Vector3 Up;
	static const Vector3 Down;
	static const Vector3 Right;
	static const Vector3 Left;
	static const Vector3 Forward;
	static const Vector3 Backward;

};
