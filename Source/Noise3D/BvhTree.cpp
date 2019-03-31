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

Noise3D::BvhTree::BvhTree()
{
}

Noise3D::BvhTree::~BvhTree()
{
}

bool Noise3D::BvhTree::Construct(const SceneGraph& tree)
{
	return BvhTree::Construct(tree.GetRoot());
}

bool Noise3D::BvhTree::Construct(SceneNode * pNode)
{
	if (pNode == nullptr)
	{
		ERROR_MSG("BvhTree: construction failed. scene node is nullptr.");
		return false;
	}
	//i arbitrarily choose an traverse order to get all the scene nodes
	std::vector<ISceneObject*> tmpSceneObjectList;
	SceneGraph* pGraph = pNode->GetHostTree();
	pGraph->TraverseSceneObjects(SceneGraph::NOISE_TREE_TRAVERSE_ORDER::PRE_ORDER,pNode, tmpSceneObjectList);

	//store computed AABB of object in pair
	std::vector<ObjectAabbPair> infoList;
	infoList.reserve(tmpSceneObjectList.size());
	for (uint32_t i = 0; i < tmpSceneObjectList.size(); ++i)
	{
		//it should be an collidable object first
		if (ICollidableSceneObject* pSO = dynamic_cast<ICollidableSceneObject*>(tmpSceneObjectList.at(i)))
		{
			//not collidable, then no need to add it to BVH
			//after all, BVH is built for acceleration of ray-XXX intersection
			if (pSO->IsCollidable())
			{
				ObjectAabbPair info;
				info.pObj = pSO;
				info.aabb = pSO->ComputeWorldAABB_Accurate();
				infoList.push_back(info);
			}
		}
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
	if (!mFunction_SplitMidPointViaAabbSlabs(pRootNode, infoList))
	{
		ERROR_MSG("BvhTree: failed to split the root BVH node.");
		return false;
	}

	return true;
}

//deprecated
bool Noise3D::BvhTree::mFunction_SplitMidPointViaCentroid(BvhNode* pNode,const std::vector<ObjectAabbPair>& infoList)
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
	Vec3 width = bigAabb.max - bigAabb.min;
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
		Vec3 objectCentroidPos = pNode->EvalWorldTransform().GetPosition();
		Vec3 aabbCenterPos = bigAabb.Centroid();

		//flatten positions into array
		float flattenedObjectPos[3] = { objectCentroidPos.x, objectCentroidPos.y, objectCentroidPos.z };
		float flattenedAabbCenterPos[3] = { aabbCenterPos.x, aabbCenterPos.y, aabbCenterPos.z };

		//classify object, using one vector component of their centroid position
		if ((Ut::TolerantEqual(flattenedObjectPos[splitAxisId], flattenedAabbCenterPos[splitAxisId], 0.0001f)))
		{
			//special case, object centroid lies right on the middle.
			//***(somethings a big object includes another small things inside, and the big AABB of that pile is the AABB of the largest object
			//And also imagine that 2 objects lies on the midpoint at the same time,
			//if nothing is done, both of them will be categorized into left or right side
			//and the recursion won't ever stop
			//possible solution 1: make a leaf node for the midpoint object
			//possible solution 2: randomly throw the object into left side or right side
			static int counter = 0;
			++counter;
			if (counter % 2 == 0)
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
		else if (flattenedObjectPos[splitAxisId] < flattenedAabbCenterPos[splitAxisId])
		{ 
			leftInfoList.push_back(pair);
			leftAabb.Union(pair.aabb);
		}
		else if(flattenedObjectPos[splitAxisId] > flattenedAabbCenterPos[splitAxisId])
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
		mFunction_SplitMidPointViaCentroid(pLeftChild, leftInfoList);
	}

	if (rightAabb.IsValid() && !rightInfoList.empty())
	{
		BvhNode* pRightChild = pNode->CreateChildNode();
		pRightChild->SetAABB(rightAabb);
		mFunction_SplitMidPointViaCentroid(pRightChild, rightInfoList);
	}
	
	return true;
}

bool Noise3D::BvhTree::mFunction_SplitMidPointViaAabbSlabs(BvhNode * pNode, const std::vector<ObjectAabbPair>& infoList)
{
	//there r many ways to implement a top-down construction's BVH, 
	//i.e., many ways to split a BVH build node/scene objects cluster.
	//this method splits objects into 2 piles according to the mid point in the axis 
	//where the aabb has the maximum width among 3 axes.

	//(2019.3.30)this method use aabb to determine the split strategy instead of centroid
	// so there will be several circumstances depend on the aabb slab's distance to mid point.

	//0.******* leaf node return condition*********
	if (infoList.empty())return false;

	N_AABB bigAabb = pNode->GetAABB();
	if (!bigAabb.IsValid())return false;
	Vec3 bigAabbCenterPos = bigAabb.Centroid();

	//if only 1 object is in current cluster, leaf node, bound SCENE OBJECT and return
	if (infoList.size() == 1)
	{
		const ObjectAabbPair& info = infoList.front();
		pNode->SetAABB(info.aabb);
		pNode->SetSceneObject(info.pObj);
		return true;
	}

	//(>= 2 objects)
	//2. find the axis where aabb has maximum width
	Vec3 width = bigAabb.max - bigAabb.min;
	int splitAxisId = 0;	//0 for x, 1 for y, 2 for z
	if (width.x > width.y && width.x > width.z)splitAxisId = 0;
	else if (width.y > width.x && width.y > width.z)splitAxisId = 1;
	else if (width.z > width.x && width.z > width.y)splitAxisId = 2;

	//3. partition the objects into two piles in terms of centroid position, 
	//compute Big AABB for each of the object cluster
	N_AABB leftAabb, rightAabb;
	std::vector<ObjectAabbPair> leftInfoList, middileInfoList, rightInfoList;
	leftInfoList.reserve(infoList.size() / 2);
	rightInfoList.reserve(infoList.size() / 2);
	for (auto& pair : infoList)
	{
		SceneNode* pNode = pair.pObj->GetAttachedSceneNode();
		const N_AABB& objectAabb = pair.aabb;

		//get vec component via the splitting axis
		float centerPos = mFunction_GetVecComponent(bigAabbCenterPos, splitAxisId);
		float minPos = mFunction_GetVecComponent(objectAabb.min, splitAxisId);
		float maxPos = mFunction_GetVecComponent(objectAabb.max, splitAxisId);

		//***classify objects***
		//---min---max---MIDPOINT------------
		if(minPos < centerPos && maxPos <= centerPos)
		{
			leftInfoList.push_back(pair);
			leftAabb.Union(pair.aabb);
		}//-----MIDPOINT---min---max-----
		else if (minPos >= centerPos && maxPos > centerPos)
		{
			rightInfoList.push_back(pair);
			rightAabb.Union(pair.aabb);
		}//---min---MIDPOINT---max---, different treatment depending on the slab's distance to middle
		else if (minPos< centerPos && maxPos> centerPos)
		{
			// absolute distance from min/max to midpoint
			float dist_min2mid = abs(mFunction_GetVecComponent(objectAabb.min - bigAabbCenterPos, splitAxisId));
			float dist_max2mid = abs(mFunction_GetVecComponent(objectAabb.max - bigAabbCenterPos, splitAxisId));
			float aabbWidth = mFunction_GetVecComponent(objectAabb.max - objectAabb.min, splitAxisId);

			//object aabb occupy too much space near MIDPOINT, spare them one BVH node for each object
			if (dist_min2mid > 0.2f *aabbWidth && dist_max2mid > 0.2f * aabbWidth)
			{
				middileInfoList.push_back(pair);
			}
			else if (dist_min2mid < dist_max2mid)
			{
				//--min--MIDPOINT------------max---,  more 'right' than 'left'
				//(and at least one side is close enough (dist<=0.2w) to MIDPOINT)
				rightInfoList.push_back(pair);
				rightAabb.Union(pair.aabb);
			}
			else if (dist_min2mid >= dist_max2mid)
			{
				//--min--------------MIDPOINT--max---,  more 'left' than 'right'
				//(and at least one side is close enough (dist<=0.2w) to MIDPOINT)
				leftInfoList.push_back(pair);
				leftAabb.Union(pair.aabb);
			}
		}//minPos< centerPos && maxPos> centerPos)
	}//pos comparison with MIDPOINT

	//4. create new BVH child node for current node and start recursion
	//(but haha, for N-ary tree, actually there is no such a thing as 'left' or 'right')
	//----- left -----
	if (leftAabb.IsValid() && !leftInfoList.empty())
	{
		BvhNode* pLeftChild = pNode->CreateChildNode();
		pLeftChild->SetAABB(leftAabb);
		mFunction_SplitMidPointViaAabbSlabs(pLeftChild, leftInfoList);
	}
	//----- right -----
	if (rightAabb.IsValid() && !rightInfoList.empty())
	{
		BvhNode* pRightChild = pNode->CreateChildNode();
		pRightChild->SetAABB(rightAabb);
		mFunction_SplitMidPointViaAabbSlabs(pRightChild, rightInfoList);
	}

	//----- middle -----
	//one BVH node for each object that occupyies too many space near MIDPOINT
	//end recursion
	for (auto& info : middileInfoList)
	{
		BvhNode* pMidChild = pNode->CreateChildNode();
		pMidChild->SetAABB(info.aabb);
		pMidChild->SetSceneObject(info.pObj);
	}

	return true;
}

inline float Noise3D::BvhTree::mFunction_GetVecComponent(Vec3 vec, uint32_t id)
{
	switch (id)
	{
	case 0:return vec.x;
	case 1: return vec.y;
	case 2:return vec.z;
	default:return std::numeric_limits<float>::quiet_NaN();
	}
}
