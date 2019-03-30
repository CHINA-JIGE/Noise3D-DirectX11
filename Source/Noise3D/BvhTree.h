
/***********************************************************************

								 h£ºBVH Tree
		desc: BVH(Bounding Volumn Hierarchy) node and tree
		commonly used in acceleration of collision test and render culling

************************************************************************/

#pragma once

namespace Noise3D
{
	class BvhTree;

	//a single node in BVH tree
	class BvhNode:
		public TreeNodeTemplate<BvhNode, BvhTree>
	{
	public:

		BvhNode();

		~BvhNode();

		void SetSceneObject(ISceneObject* pObj);
		
		ISceneObject* GetSceneObject();

		void SetAABB(const N_AABB& aabb);

		N_AABB GetAABB();

	private:

		ISceneObject* m_pSceneObject;//for leaf node

		N_AABB mAabb;

	};

	//BVH tree, might be used in collision testor as an acceleration data structure
	class BvhTree:
		public TreeTemplate<BvhNode, BvhTree>
	{
	public:

		BvhTree();

		~BvhTree();

		bool Construct(const SceneGraph& pSG);

	private:

		//SceneObject ptr and its aabb cache
		struct ObjectAabbPair
		{
			ObjectAabbPair() : pObj(nullptr) {}
			ObjectAabbPair(const ObjectAabbPair& rhs):pObj(rhs.pObj ), aabb(rhs.aabb){}

			ISceneObject* pObj;
			N_AABB aabb;//cached result
		};

		//deprecated
		bool mFunction_SplitMidPointViaCentroid(BvhNode* pNode,const std::vector<ObjectAabbPair>& infoList);

		bool mFunction_SplitMidPointViaAabbSlabs(BvhNode* pNode, const std::vector<ObjectAabbPair>& infoList);

		float mFunction_GetVecComponent(NVECTOR3 vec, uint32_t id);
	};

}
