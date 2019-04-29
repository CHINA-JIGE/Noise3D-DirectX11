/***********************************************************

					 BVH Tree and Node for triangular mesh

		desc:  similar to BVH tree in collision testor.
		this bvh tree hold triangle id as value at the leaf nodes

***********************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Noise3D::BvhNodeForTriangularMesh::BvhNodeForTriangularMesh()
{
}

Noise3D::BvhNodeForTriangularMesh::~BvhNodeForTriangularMesh()
{
}

std::vector<uint32_t>& Noise3D::BvhNodeForTriangularMesh::GetTriangleIndexList()
{
	return mTriangleIndexList;
}

void Noise3D::BvhNodeForTriangularMesh::SetAABB(const N_AABB & aabb)
{
	mAabb = aabb;
}

N_AABB Noise3D::BvhNodeForTriangularMesh::GetAABB() const
{
	return mAabb;
}


/**********************************************

							BVH Tree

***********************************************/


Noise3D::BvhTreeForTriangularMesh::BvhTreeForTriangularMesh()
{
}

Noise3D::BvhTreeForTriangularMesh::~BvhTreeForTriangularMesh()
{
}

bool Noise3D::BvhTreeForTriangularMesh::Construct(Mesh* pMesh)
{
	//could be invoked by BvhTreeForScene::Construct()
	//if leaf node BvhTreeForScene is mesh, then mesh's internal bvh tree will be rebuilt

	if (pMesh == nullptr)
	{
		ERROR_MSG("BvhTreeForTriangularMesh: construction failed. mesh is nullptr.");
		return false;
	}

	//store computed AABB of triangle in pair
	uint32_t triangleCount = pMesh->GetTriangleCount();
	std::vector<TriIdListAabbPair> infoList;
	infoList.reserve(triangleCount);
	m_pVB = pMesh->GetVertexBuffer();
	m_pIB = pMesh->GetIndexBuffer();

	//compute AABB of each triangles
	for (uint32_t i = 0; i < triangleCount; ++i)
	{
		Vec3 v0 = m_pVB->at(m_pIB->at(i * 3 + 0)).Pos;
		Vec3 v1 = m_pVB->at(m_pIB->at(i * 3 + 1)).Pos;
		Vec3 v2 = m_pVB->at(m_pIB->at(i * 3 + 2)).Pos;

		TriIdListAabbPair pair;
		pair.aabb = mFunction_ComputeAabb(v0, v1, v2);
		pair.triangleIndexList.push_back(i);		//in the end, a triangle cluster(several triangles) might be attached to BVH leaf node
		infoList.push_back(pair);
	}

	//1.calculate the largest aabb that bound the whole mesh (AABB of all small AABBs)
	N_AABB rootAabb = pMesh->GetLocalAABB();
	if (!rootAabb.IsValid())
	{
		ERROR_MSG("BvhTreeForTriangularMesh: AABB of root(the whole mesh) should have a positive volume.");
		return false;
	}

	//2. set aabb to root bvh node, and start to recursive split
	BvhNodeForTriangularMesh* pRootNode = BvhTreeForTriangularMesh::GetRoot();
	pRootNode->SetAABB(rootAabb);

	//start to recursive splitting(info list might be splitted and copied many times)
	//(2019.3.25)could be optimized with std::partition
	if (!mFunction_SplitMidPoint(pRootNode, infoList))
	{
		ERROR_MSG("BvhTreeForTriangularMesh: failed to split the root BVH node.");
		return false;
	}

	return true;
}

/******************************************

							PRIVATE

******************************************/

bool Noise3D::BvhTreeForTriangularMesh::mFunction_SplitMidPoint(BvhNodeForTriangularMesh * pNode, std::vector<TriIdListAabbPair>& infoList)
{
	//one could refer to BVH for scene object in Collision Testor

	//0.******* leaf node return condition*********
	if (infoList.empty())return false;

	N_AABB bigAabb = pNode->GetAABB();
	if (!bigAabb.IsValid())return false;
	Vec3 bigAabbCenterPos = bigAabb.Centroid();

	//one leaf node could have several triangles / triangle cluster
	const uint32_t c_maxTriangleCountPerCluster = 3;
	if (infoList.size() <= c_maxTriangleCountPerCluster)
	{
		TriIdListAabbPair leafNodeInfo;
		for (auto& info : infoList)
		{
			leafNodeInfo.aabb.Union(info.aabb);
			//until deep down to leaf node, each TriIdlistAabbInfo will only be 1 to 1 Triangle-AABB pair
			leafNodeInfo.triangleIndexList.push_back(info.triangleIndexList.front());
		}
		pNode->SetAABB(leafNodeInfo.aabb);
		pNode->GetTriangleIndexList().assign(leafNodeInfo.triangleIndexList.begin(), leafNodeInfo.triangleIndexList.end());
		return true;
	}

	//(>= max triangle count per clusters)
	//2. find the AXIS where aabb has maximum width
	Vec3 width = bigAabb.max - bigAabb.min;
	int splitAxisId = 0;	//0 for x, 1 for y, 2 for z
	if (width.x > width.y && width.x > width.z)splitAxisId = 0;
	else if (width.y > width.x && width.y > width.z)splitAxisId = 1;
	else if (width.z > width.x && width.z > width.y)splitAxisId = 2;

	//3. partition the objects into two piles in terms of centroid position, 
	//compute Big AABB for each of the object cluster
	N_AABB leftAabb, middleAabb, rightAabb;
	std::vector<TriIdListAabbPair> leftInfoList, middleInfoList, rightInfoList;//1 to 1 Triangle-AABB pair
	leftInfoList.reserve(infoList.size() / 3);
	middleInfoList.reserve(infoList.size() / 3);
	rightInfoList.reserve(infoList.size() / 3);
	for (auto& pair : infoList)
	{
		//get vec component via the splitting axis
		float centerPos = mFunction_GetVecComponent(bigAabbCenterPos, splitAxisId);
		for (auto& triId : pair.triangleIndexList)
		{
			Vec3 v0 = m_pVB->at(m_pIB->at(triId * 3 + 0)).Pos;
			Vec3 v1 = m_pVB->at(m_pIB->at(triId * 3 + 1)).Pos;
			Vec3 v2 = m_pVB->at(m_pIB->at(triId * 3 + 2)).Pos;
			float pos_0 = mFunction_GetVecComponent(v0, splitAxisId);
			float pos_1 = mFunction_GetVecComponent(v1, splitAxisId);
			float pos_2 = mFunction_GetVecComponent(v2, splitAxisId);
			uint32_t leftVertexCount = 0;
			if (pos_0 < centerPos)leftVertexCount++;
			if (pos_1 < centerPos)leftVertexCount++;
			if (pos_2 < centerPos)leftVertexCount++;

			//***classify objects***
			//(2019.4.27)if there are many triangles coincide, then the recursion might never stop.
			//solution: refer to bvh tree in collision testor
			/*if (leftVertexCount==3)
			{
				leftInfoList.push_back(pair);
				leftAabb.Union(pair.aabb);
			}
			else if (leftVertexCount == 2 || leftVertexCount == 1)
			{
				middleInfoList.push_back(pair);
				middleAabb.Union(pair.aabb);
			}
			else
			{
				rightInfoList.push_back(pair);
				rightAabb.Union(pair.aabb);
			}*/
			if (leftVertexCount==3)
			{
				leftInfoList.push_back(pair);
				leftAabb.Union(pair.aabb);
			}
			else if (leftVertexCount == 0)
			{
				rightInfoList.push_back(pair);
				rightAabb.Union(pair.aabb);
			}
			else //==1 or ==2
			{
				// absolute distance from min/max to midpoint
				float dist_min2mid = abs(mFunction_GetVecComponent(pair.aabb.min - bigAabbCenterPos, splitAxisId));
				float dist_max2mid = abs(mFunction_GetVecComponent(pair.aabb.max - bigAabbCenterPos, splitAxisId));
				float aabbWidth = mFunction_GetVecComponent(bigAabb.max - bigAabb.min, splitAxisId);
				if (dist_min2mid > 0.01f *aabbWidth && dist_max2mid > 0.01f * aabbWidth)
				{
					middleInfoList.push_back(pair);
					middleAabb.Union(pair.aabb);
				}
				else if (dist_min2mid < dist_max2mid)
				{
					//--min--MIDPOINT------------max---,  more 'right' than 'left'
					rightInfoList.push_back(pair);
					rightAabb.Union(pair.aabb);
				}
				else if (dist_min2mid >= dist_max2mid)
				{
					//--min--------------MIDPOINT--max---,  more 'left' than 'right'
					leftInfoList.push_back(pair);
					leftAabb.Union(pair.aabb);
				}
			}

		}//for each triangle
	}//for each info in current big AABB

	//up-level info list is no longer useful
	infoList.clear();

	 //4. create new BVH child node for current node and start recursion
	 //(but haha, for N-ary tree, actually there is no such a thing as 'left' or 'right')
	 //----- left -----
	if (leftAabb.IsValid() && !leftInfoList.empty())
	{
		BvhNodeForTriangularMesh* pLeftChild = pNode->CreateChildNode();
		pLeftChild->SetAABB(leftAabb);
		mFunction_SplitMidPoint(pLeftChild, leftInfoList);
	}

	//--middle--
	if (middleAabb.IsValid() && !middleInfoList.empty())
	{
		/*BvhNodeForTriangularMesh* pMidChild = pNode->CreateChildNode();
		pMidChild->SetAABB(middleAabb);
		mFunction_SplitMidPoint(pMidChild, middleInfoList);*/
		BvhNodeForTriangularMesh* pMidChild = pNode->CreateChildNode();
		pMidChild->SetAABB(middleAabb);
		for (auto& pair : middleInfoList)
		{
			pMidChild->GetTriangleIndexList().push_back(pair.triangleIndexList.front());//until leaf node, the pair is 1 to 1 index-AABB
		}
	}

	//----- right -----
	if (rightAabb.IsValid() && !rightInfoList.empty())
	{
		BvhNodeForTriangularMesh* pRightChild = pNode->CreateChildNode();
		pRightChild->SetAABB(rightAabb);
		mFunction_SplitMidPoint(pRightChild, rightInfoList);
	}
	return true;
}

inline N_AABB Noise3D::BvhTreeForTriangularMesh::mFunction_ComputeAabb(Vec3 v0, Vec3 v1, Vec3 v2)
{
	N_AABB aabb;
	float min[3], max[3];
	for (int i = 0; i < 3; ++i)
	{
		min[i] = std::min<float>({
			mFunction_GetVecComponent(v0,i),
			mFunction_GetVecComponent(v1,i),
			mFunction_GetVecComponent(v2,i) });
		max[i] = std::max<float>({
			mFunction_GetVecComponent(v0,i),
			mFunction_GetVecComponent(v1,i),
			mFunction_GetVecComponent(v2,i) });
	}

	aabb.min = Vec3(min[0], min[1], min[2]);
	aabb.max = Vec3(max[0], max[1], max[2]);
	return aabb;
}

inline float Noise3D::BvhTreeForTriangularMesh::mFunction_GetVecComponent(Vec3 vec, uint32_t id)
{
	switch (id)
	{
	case 0:return vec.x;
	case 1: return vec.y;
	case 2:return vec.z;
	default:return std::numeric_limits<float>::quiet_NaN();
	}
}
