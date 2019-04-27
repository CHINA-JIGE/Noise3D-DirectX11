
/***********************************************************************

								 h£ºBVH Tree for triangular mesh
		desc: BVH(Bounding Volumn Hierarchy) node and tree
		used for triangle

************************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		class IGiRenderable;
	}

	class BvhTreeForTriangularMesh;

	//a single node in BVH tree
	class BvhNodeForTriangularMesh:
		public TreeNodeTemplate<BvhNodeForTriangularMesh, BvhTreeForTriangularMesh>
	{
	public:

		BvhNodeForTriangularMesh();

		~BvhNodeForTriangularMesh();

		std::vector<uint32_t>& GetTriangleIndexList();

		void SetAABB(const N_AABB& aabb);

		N_AABB GetAABB();

	private:

		std::vector<uint32_t> mTriangleIndexList;//holds triangle's index in triangle list of a mesh

		N_AABB mAabb;

	};

	//BVH tree, might be used in collision testor as an acceleration data structure
	//only scene objects attached to BvhNodeForScene will support ray-object intersection
	class BvhTreeForTriangularMesh:
		public TreeTemplate<BvhNodeForTriangularMesh, BvhTreeForTriangularMesh>
	{
	public:

		BvhTreeForTriangularMesh();

		~BvhTreeForTriangularMesh();

		bool Construct(Mesh* pMesh);

	private:

		//SceneObject ptr and its aabb cache
		struct TriIdListAabbPair
		{
			TriIdListAabbPair(){}
			TriIdListAabbPair(const TriIdListAabbPair& rhs):triangleIndexList(rhs.triangleIndexList), aabb(rhs.aabb){}

			std::vector<uint32_t> triangleIndexList;
			N_AABB aabb;//cached result
		};

		bool mFunction_SplitMidPoint(BvhNodeForTriangularMesh* pNode,const std::vector<TriIdListAabbPair>& infoList);

		N_AABB mFunction_ComputeAabb(Vec3 v0, Vec3 v1, Vec3 v2);

		float mFunction_GetVecComponent(Vec3 vec, uint32_t id);

		const std::vector<N_DefaultVertex>* m_pVB;
		const std::vector<uint32_t>* m_pIB;
	};

}
