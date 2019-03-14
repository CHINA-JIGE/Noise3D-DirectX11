/***********************************************************

					 BVH Tree and Node

		desc: BVH(Bounding Volumn Hierarchy) node and tree
		commonly used in acceleration of collision test 
		and render culling. Leaf node of BVH should have a 
		concrete scene object. Non-leaf node have just AABB
		to accelerate collision test
		(pruning branches ASAP in a fast manner)

***********************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Noise3D::BvhNode::BvhNode():
	m_pSceneObject(nullptr)
{
}

Noise3D::BvhNode::~BvhNode()
{
}

void Noise3D::BvhNode::SetSceneObject(ISceneObject * pObj)
{
	 if(pObj!=nullptr) m_pSceneObject = pObj;
}

ISceneObject * Noise3D::BvhNode::GetSceneObject()
{
	return m_pSceneObject;
}

void Noise3D::BvhNode::SetAABB(const N_AABB & aabb)
{
	mAabb = aabb;
}

N_AABB Noise3D::BvhNode::GetAABB()
{
	return mAabb;
}
