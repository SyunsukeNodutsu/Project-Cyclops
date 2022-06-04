//-----------------------------------------------------------------------------
// File: Transform.h
//
// オブジェクトの位置,回転.スケールを扱う Objectに最初から付与
//-----------------------------------------------------------------------------
#pragma once

//位置,回転.スケールを扱うクラス
class Transform : public Component
{
public:

	Transform();
	~Transform() = default;

	const matrix4x4& GetWorldMatrix() const;
	float3 GetPosition() const { return m_worldPosition; }
	float3 GetAngle() const { return m_worldAngle; }
	float3 GetScale() const { return m_worldScale; }

	void SetWorldMatrix(const matrix4x4& matrix);

	void SetPosition(const float3& position) { m_worldPosition = position; m_needUpdateMatrix = true; }
	void SetPosition(const float x, const float y, const float z) { m_worldPosition = float3(x, y, z); m_needUpdateMatrix = true; }

	void SetAngle(const float3& angle) { m_worldAngle = angle; m_needUpdateMatrix = true; }

	void SetScale(const float3& scale) { m_worldScale = scale; m_needUpdateMatrix = true; }
	void SetScale(const float scale) { m_worldScale = float3(scale, scale, scale); m_needUpdateMatrix = true; }

private:

	float3 m_worldPosition;	//ワールド行列を構成する座標ベクトル
	float3 m_worldAngle;		//ワールド行列を構成する回転ベクトル
	float3 m_worldScale;		//ワールド行列を構成する拡大

	mutable matrix4x4 m_worldMatrix;
	mutable bool m_needUpdateMatrix;//行列を更新する必要があるか？

};
