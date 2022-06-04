#include "Collision.h"

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Collision::SphereToSphere(const DirectX::XMVECTOR& rPos1, const DirectX::XMVECTOR& rPos2, float radius1, float radius2, SphereResult* pResult)
{
	return false;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Collision::RayToMesh(const DirectX::XMVECTOR& rRayPos, const DirectX::XMVECTOR& rRayDir, float maxDistance, const Mesh& rMesh, const DirectX::XMMATRIX& rMatrix, RayResult* pResult)
{
	return false;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Collision::SphereToMesh(const float3& rSpherePos, float radius, const Mesh& mesh, const DirectX::XMMATRIX& matrix, float3& rPushedPos)
{
	return false;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void Collision::PointToTriangle(const DirectX::XMVECTOR& p, const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b, const DirectX::XMVECTOR& c, DirectX::XMVECTOR& outPt)
{

}
