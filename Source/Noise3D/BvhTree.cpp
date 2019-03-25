/***********************************************************

					 BVH Tree and Node

		desc: BVH(Bounding Volume Hierarchy) node and tree
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

/**********************************************

							Scene Graph

***********************************************/

bool Noise3D::BvhTree::Construct(SceneGraph * pTree)
{
	if (pTree == nullptr) 
	{
		ERROR_MSG("BvhTree: scene graph ptr invalid. failed to construct.");
		return false; 
	}

	//i arbitrarily choose an traverse order to get all the scene nodes
	std::vector<ISceneObject*> tmpSceneObjectList;
	pTree->TraverseSceneObjects(SceneGraph::NOISE_TREE_TRAVERSE_ORDER::PRE_ORDER, tmpSceneObjectList);

	//store computed AABB of object in pair
	std::vector<ObjectAabbPair> infoList;
	infoList.resize(tmpSceneObjectList.size());
	for (uint32_t i = 0; i < infoList.size(); ++i)
	{
		infoList.at(i).pObj = tmpSceneObjectList.at(i);
		infoList.at(i).aabb = tmpSceneObjectList.at(i)->ComputeWorldAABB_Accurate();
	}

	//AABB of Bvh Node that include the whole scene
	N_AABB rootAabb;	

	//1.calculate the largest aabb that bound the whole scene (AABB of all small AABBs)
	for (auto& info : infoList) rootAabb.Union(info.aabb);
	if (!rootAabb.IsValid())
	{
		ERROR_MSG("BvhTree: AABB of root(the whole scene) should have a positive volume.");
		return false;
	}

	//2. set aabb to root bvh node, and start to recursive split
	BvhNode* pRootNode = BvhTree::GetRoot();
	pRootNode->SetAABB(rootAabb);

	//start to recursive splitting(info list might be splitted and copied many times)
	//(2019.3.25)could be optimized with std::partition
	if (!mFunction_Split_MidPoint(pRootNode, infoList))
	{
		ERROR_MSG("BvhTree: failed to split the root BVH node.");
		return false;
	}

	return true;
}

bool Noise3D::BvhTree::mFunction_Split_MidPoint(BvhNode* pNode,const std::vector<ObjectAabbPair>& infoList)
{
	//there r many ways to implement a top-down construction's BVH, 
	//i.e., many ways to split a BVH build node/scene objects cluster.
	//this method splits objects into 2 piles according to the mid point in the axis 
	//where the aabb has the maximum width among 3 axes.

	//0.******* leaf node return condition*********
	//in some cases of splitting, some node 
	if (infoList.empty())return false;

	N_AABB bigAabb = pNode->GetAABB();
	if (!bigAabb.IsValid())return false;

	//1. if only 1 object is in current cluster, leaf node, bound SCENE OBJECT and return
	if (infoList.size() == 1)
	{
		const ObjectAabbPair& info = infoList.front();
		pNode->SetAABB(info.aabb);
		pNode->SetSceneObject(info.pObj);
		return true;
	}

	//(>= 2 objects)
	//2. find the axis where aabb has maximum width
	NVECTOR3 width = bigAabb.max - bigAabb.min;
	int splitAxisId = 0;	//0 for x, 1 for y, 2 for z
	if (width.x > width.y && width.x > width.z)splitAxisId = 0;
	else if (width.y > width.x && width.y > width.z)splitAxisId = 1;
	else if (width.z > width.x && width.z > width.y)splitAxisId = 2;

	//3. partition the objects into two piles in terms of centroid position, 
	//compute Big AABB for each of the object cluster
	N_AABB leftAabb, rightAabb;
	std::vector<ObjectAabbPair> leftInfoList, rightInfoList;
	leftInfoList.reserve(infoList.size() / 2);
	rightInfoList.reserve(infoList.size() / 2);
	for (auto& pair : infoList)
	{
		SceneNode* pNode = pair.pObj->GetAttachedSceneNode();
		NVECTOR3 objectCentroidPos = pNode->EvalWorldTransform().GetPosition();
		NVECTOR3 aabbCenterPos = bigAabb.Centroid();

		//flatten positions into array
		float flattenedObjectPos[3] = { objectCentroidPos.x, objectCentroidPos.y, objectCentroidPos.z };
		float flattenedAabbCenterPos[3] = { aabbCenterPos.x, aabbCenterPos.y, aabbCenterPos.z };

		//classify object, using one vector component of their centroid position
		if (flattenedObjectPos[splitAxisId] < flattenedAabbCenterPos[splitAxisId])
		{ 
			leftInfoList.push_back(pair);
			leftAabb.Union(pair.aabb);
		}
		else
		{
			rightInfoList.push_back(pair);
			rightAabb.Union(pair.aabb);
		}
	}

	//4. create new BVH child node for current node and start recursion
	//(but haha, for N-ary tree, actually there is no such a thing as 'left' or 'right')
	if (leftAabb.IsValid() && !leftInfoList.empty())
	{
		BvhNode* pLeftChild = pNode->CreateChildNode();
		pLeftChild->SetAABB(leftAabb);
		mFunction_Split_MidPoint(pLeftChild, infoList);
	}

	if (rightAabb.IsValid() && !rightInfoList.empty())
	{
		BvhNode* pRightChild = pNode->CreateChildNode();
		pRightChild->SetAABB(rightAabb);
		mFunction_Split_MidPoint(pRightChild, infoList);
	}
	
	return true;
}
