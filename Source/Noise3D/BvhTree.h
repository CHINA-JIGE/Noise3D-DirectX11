
/***********************************************************************

								 h£ºBVH Tree
		desc: BVH(Bounding Volumn Hierarchy) node and tree
		commonly used in acceleration of collision test and render culling

************************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		class IGiRenderable;
	}

	class BvhTreeForGI;

	//a single node in BVH tree
	class BvhNodeForGI:
		public TreeNodeTemplate<BvhNodeForGI, BvhTreeForGI>
	{
	public:

		BvhNodeForGI();

		~BvhNodeForGI();

		void SetGiRenderable(GI::IGiRenderable* pObj);
		
		GI::IGiRenderable* GetGiRenderable();

		void SetAABB(const N_AABB& aabb);

		N_AABB GetAABB();

	private:

		GI::IGiRenderable* m_pSceneObject;//for leaf node

		N_AABB mAabb;

	};

	//BVH tree, might be used in collision testor as an acceleration data structure
	//only scene objects attached to BvhNodeForGI will support ray-object intersection
	class BvhTreeForGI:
		public TreeTemplate<BvhNodeForGI, BvhTreeForGI>
	{
	public:

		BvhTreeForGI();

		~BvhTreeForGI();

		bool Construct(const SceneGraph& pSG);

		bool Construct(SceneNode* pNode);

		void TraverseSceneObjects(NOISE_TREE_TRAVERSE_ORDER order, std::vector<GI::IGiRenderable*>& outResult) const;

	private:

		//SceneObject ptr and its aabb cache
		struct ObjectAabbPair
		{
			ObjectAabbPair() : pObj(nullptr) {}
			ObjectAabbPair(const ObjectAabbPair& rhs):pObj(rhs.pObj ), aabb(rhs.aabb){}

			GI::IGiRenderable* pObj;
			N_AABB aabb;//cached result
		};

		//deprecated
		bool mFunction_SplitMidPointViaCentroid(BvhNodeForGI* pNode,const std::vector<ObjectAabbPair>& infoList);

		bool mFunction_SplitMidPointViaAabbSlabs(BvhNodeForGI* pNode, const std::vector<ObjectAabbPair>& infoList);

		float mFunction_GetVecComponent(Vec3 vec, uint32_t id);
	};

}
