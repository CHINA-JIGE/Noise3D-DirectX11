/***********************************************************

								SceneNode

			A Single Node of Scene Graph/Tree.
		Each Node's transform are based on its parent node,
		so hierarchical transformation is available.

***********************************************************/

#include "Noise3D.h"

using namespace Noise3D;

AffineTransform& Noise3D::SceneNode::GetLocalTransform()
{
	return mLocalTransform;
}

NMATRIX Noise3D::SceneNode::EvalWorldTransformMatrix()
{
	SceneGraph* pSG = this->GetHostTree();
	std::vector<SceneNode*> path;//path to root
	pSG->TraversePathToRoot(this, path);

	//concatenate leaf node's transform first, then level-by-level to root
	//traversePathToRoot's result have the leaf node at the beginning, and root at the end
	NMATRIX result = XMMatrixIdentity();
	for (auto pNode: path)
	{
		//WARNING: plz be careful about ROW/COLUMN major 
		//(2019.3.7)Noise3D uses ROW major like DXMath do. refer to AffineTransform for related info
		NMATRIX tmpMat = XMMatrixIdentity();
		AffineTransform& localTrans = pNode->GetLocalTransform();
		localTrans.GetTransformMatrix(tmpMat);

		// world_vec = local_vec *  Mat_n * Mat_(n-1) * .... Mat_1 * Mat_root
		result = result * tmpMat;
	}

	return result;
}

void Noise3D::SceneNode::EvalWorldTransformMatrix(NMATRIX & outWorldMat, NMATRIX & outWorldInvTranspose)
{
	outWorldMat = SceneNode::EvalWorldTransformMatrix();

	//world inv transpose for normal's transformation
	NMATRIX worldInvMat = XMMatrixInverse(nullptr, outWorldMat);
	if (XMMatrixIsInfinite(worldInvMat))
	{
		ERROR_MSG("world matrix Inv not exist! determinant == 0 ! ");
		outWorldInvTranspose= XMMatrixIdentity();
		return;
	}
	else
	{
		outWorldInvTranspose = worldInvMat.Transpose();
	}
}

bool Noise3D::SceneNode::IsBoundWithSceneObject()
{
	return mIsBoundWithSceneObject;
}

/*************************************


*************************************/
Noise3D::SceneNode::SceneNode(bool isBoundWidthObject) :
	mIsBoundWithSceneObject(isBoundWidthObject)
{
}

Noise3D::SceneNode::~SceneNode()
{
}
