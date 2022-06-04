//-----------------------------------------------------------------------------
// File: Collision.h
//
// 基本的な衝突判定 処理重め
//-----------------------------------------------------------------------------
#pragma once

namespace Collision
{
	struct SphereResult
	{
		float m_distance = FLT_MAX;		//お互いの球の間の距離が入る
		bool m_hit = false;				//ヒットしたかどうか
	};

	struct RayResult
	{
		float3	m_hitPos = float3();	//衝突座標
		float m_distance = FLT_MAX;		//レイの座標からヒットした位置までの距離が入る
		bool m_hit = false;				//ヒットしたかどうか
	};

	class Mesh {};//TODO: 実装

	//球vs球
	//判定結果だけで良い場合はリザルトにnullptrを渡しておく
	bool SphereToSphere(const DirectX::XMVECTOR& rPos1, const DirectX::XMVECTOR& rPos2, float radius1, float radius2, SphereResult* pResult = nullptr);

	//レイvsメッシュ
	//判定結果だけで良い場合はリザルトにnullptrを渡しておく
	bool RayToMesh(const DirectX::XMVECTOR& rRayPos, const DirectX::XMVECTOR& rRayDir, float maxDistance, const Mesh& rMesh, const DirectX::XMMATRIX& rMatrix, RayResult* pResult = nullptr);

	//球vsメッシュ
	bool SphereToMesh(const float3& rSpherePos, float radius, const Mesh& mesh, const DirectX::XMMATRIX& matrix, float3& rPushedPos);

	//点vs三角形との最近接点を求める
	void PointToTriangle(const DirectX::XMVECTOR& p, const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b, const DirectX::XMVECTOR& c, DirectX::XMVECTOR& outPt);
}
