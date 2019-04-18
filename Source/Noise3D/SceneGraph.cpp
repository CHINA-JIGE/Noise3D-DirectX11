/***********************************************************

					SceneNode & SceneGraph

		Each Node's transform are based on its parent node,
		so hierarchical transformation is available.
		World transform evaluation might need to traverse a
		path in scene graph from leaf to root, and concatenate
		the local transforms into one.

***********************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Noise3D::SceneNode::SceneNode():
	mIsWorldMatrixCached(false)
{
}

Noise3D::SceneNode::~SceneNode()
{
}

AffineTransform& Noise3D::SceneNode::GetLocalTransform()
{
	return mLocalTransform;
}

AffineTransform Noise3D::SceneNode::EvalWorldTransform(bool cacheResult)
{
	//previous evaluated result's fast retrival
	if (mIsWorldMatrixCached)return mWorldTransformCache;

	SceneGraph* pSG = this->GetHostTree();
	std::vector<SceneNode*> path;//path to root
	pSG->TraversePathToRoot(this, path);

	//concatenate leaf node's transform first, then level-by-level to root
	//traversePathToRoot's result have the leaf node at the beginning, and root at the end
	Matrix world_mat = XMMatrixIdentity();

	//(2019.3.22)ignore root node's transform
	for (uint32_t i = 0; i<path.size() - 1; ++i)
	{
		//WARNING: plz be careful about ROW/COLUMN major 
		//(2019.3.7)Noise3D uses ROW major like DXMath do. refer to AffineTransform for related info
		SceneNode* pNode = path.at(i);
		Matrix tmpMat = XMMatrixIdentity();
		AffineTransform& localTrans = pNode->GetLocalTransform();
		tmpMat = localTrans.GetAffineTransformMatrix();

		// world_vec = local_vec *  Mat_n * Mat_(n-1) * .... Mat_1 * Mat_root
		world_mat = world_mat * tmpMat;
	}

	//
	AffineTransform t;
	t.SetAffineMatrix(world_mat);
	if (cacheResult)
	{
		mIsWorldMatrixCached = true;
		mWorldTransformCache = t;
	}

	return t;
}

AffineTransform Noise3D::SceneNode::EvalWorldTransform_Rigid(bool cacheResult)
{
	//similar to evalWorldTransform, except that it only count T & R, ignore S
	//previous evaluated result's fast retrival
	if (mIsWorldMatrixCached)return mWorldTransformCache;

	SceneGraph* pSG = this->GetHostTree();
	std::vector<SceneNode*> path;//path to root
	pSG->TraversePathToRoot(this, path);

	Matrix world_mat = XMMatrixIdentity();
	//(2019.3.22)ignore root node's transform
	for (uint32_t i = 0; i<path.size() - 1; ++i)
	{
		SceneNode* pNode = path.at(i);
		Matrix tmpMat = XMMatrixIdentity();
		AffineTransform& localTrans = pNode->GetLocalTransform();
		tmpMat = localTrans.GetRigidTransformMatrix();

		// world_vec = local_vec *  Mat_n * Mat_(n-1) * .... Mat_1 * Mat_root
		world_mat = world_mat * tmpMat;
	}

	AffineTransform t;
	t.SetAffineMatrix(world_mat);
	if (cacheResult)
	{
		mIsWorldMatrixCached = true;
		mWorldTransformCache = t;
	}

	return t;
}

void Noise3D::SceneNode::ClearWorldTransformCache()
{
	//clear world matrix cache mark set by 'EvalWorldTransform' function (with parameter 'cacheResult'==true)
	mIsWorldMatrixCached = false;
}

bool Noise3D::SceneNode::IsWorldTransformCached()
{
	return mIsWorldMatrixCached;
}

void Noise3D::SceneNode::AttachSceneObject(ISceneObject * pObj)
{
	//should be compatible with ISceneObject::AttachToSceneNode(SceneNode * pNode)

	//similar to the impl of TreeNodeTemplate's AttachChildNode()
	if (pObj != nullptr)
	{
		SceneNode* pOriginalNode = pObj->GetAttachedSceneNode();
		//if attach to different node, we must detach its original connection first
		if (pOriginalNode != nullptr && pOriginalNode != this)
		{
			//delete pObj's original father's ref to this scene object
			std::vector<ISceneObject*>& originalNodeSOList = pOriginalNode->SceneNode::mAttachedSceneObjectList;
			auto iter_ChildObjRef = std::find(originalNodeSOList.begin(),	originalNodeSOList.end(),pObj);

			if (iter_ChildObjRef != originalNodeSOList.end())
			{
				originalNodeSOList.erase(iter_ChildObjRef);
			}
		}
		pObj->m_pAttachedSceneNode = this;
		mAttachedSceneObjectList.push_back(pObj);
	}
}

void Noise3D::SceneNode::DetachSceneObject(ISceneObject * pObj)
{
	if (pObj != nullptr)
	{
		//delete pObj's original father's ref to current node
		auto iter_ChildObjRef = std::find(
				mAttachedSceneObjectList.begin(),
				mAttachedSceneObjectList.end(),pObj);

		if (iter_ChildObjRef != mAttachedSceneObjectList.end())
		{
			mAttachedSceneObjectList.erase(iter_ChildObjRef);
		}
	}
	pObj->m_pAttachedSceneNode = nullptr;
}

bool Noise3D::SceneNode::IsAttachedSceneObject()
{
	return (mAttachedSceneObjectList.size()!=0);
}

uint32_t Noise3D::SceneNode::GetSceneObjectCount()
{
	return mAttachedSceneObjectList.size() ;
}

ISceneObject * Noise3D::SceneNode::GetSceneObject(uint32_t index)
{
	if (index < mAttachedSceneObjectList.size())
	{
		return mAttachedSceneObjectList[index];
	}
	return nullptr;
}


/******************************************
					
						Scene Graph

*******************************************/

Noise3D::SceneGraph::SceneGraph()
{
}

Noise3D::SceneGraph::~SceneGraph()
{
}


void Noise3D::SceneGraph::TraverseSceneObjects(NOISE_TREE_TRAVERSE_ORDER order, std::vector<ISceneObject*>& outResult) const
{
	std::vector<SceneNode*> nodeList;

	//(2019.3.24)actually getting scene nodes first has extra cost, because all scene node ptr s
	//are copied, but nodes with no scene object bound to it are useless here.
	switch (order)
	{
	case NOISE_TREE_TRAVERSE_ORDER::PRE_ORDER:
		SceneGraph::Traverse_PreOrder(nodeList); break;

	case NOISE_TREE_TRAVERSE_ORDER::POST_ORDER:
		SceneGraph::Traverse_PostOrder(nodeList); break;

	case NOISE_TREE_TRAVERSE_ORDER::LAYER_ORDER:
		SceneGraph::Traverse_LayerOrder(nodeList); break;

	default:
		break;
	}

	//output scene objects bound to nodes
	for (auto pn:nodeList)
	{
		for (uint32_t i = 0; i < pn->GetSceneObjectCount();++i)
		{
			outResult.push_back(pn->GetSceneObject(i));
		}
	}
}

void Noise3D::SceneGraph::TraverseSceneObjects(NOISE_TREE_TRAVERSE_ORDER order, SceneNode * pNode, std::vector<ISceneObject*>& outResult) const
{
	if (pNode == nullptr)return;
	std::vector<SceneNode*> nodeList;

	//(2019.3.24)actually getting scene nodes first has extra cost, because all scene node ptr s
	//are copied, but nodes with no scene object bound to it are useless here.
	switch (order)
	{
	case NOISE_TREE_TRAVERSE_ORDER::PRE_ORDER:
		SceneGraph::Traverse_PreOrder(pNode,nodeList); break;

	case NOISE_TREE_TRAVERSE_ORDER::POST_ORDER:
		SceneGraph::Traverse_PostOrder(pNode, nodeList); break;

	case NOISE_TREE_TRAVERSE_ORDER::LAYER_ORDER:
		SceneGraph::Traverse_LayerOrder(pNode, nodeList); break;

	default:
		break;
	}

	//output scene objects bound to nodes
	for (auto pn : nodeList)
	{
		for (uint32_t i = 0; i < pn->GetSceneObjectCount(); ++i)
		{
			outResult.push_back(pn->GetSceneObject(i));
		}
	}
}

