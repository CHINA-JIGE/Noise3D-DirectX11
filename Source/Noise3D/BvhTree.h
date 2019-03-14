
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

		void Construct(SceneGraph* pSG);

	private:
	};

}
