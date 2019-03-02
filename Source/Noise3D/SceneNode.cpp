/***********************************************************

								SceneNode

			A Single Node of Scene Graph/Tree.
		Each Node's transform are based on its parent node,
		so hierarchical transformation is available.

***********************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Noise3D::SceneNode::SceneNode(bool isRoot):
	mIsRoot(isRoot),
	m_pParentSceneNode(nullptr)
{
}

SceneNode * Noise3D::SceneNode::GetParentSceneNode()
{
	return m_pParentSceneNode;
}

void Noise3D::SceneNode::AttachToParentSceneNode(SceneNode * pNode)
{
	if(pNode!=nullptr)pNode->at
}

AffineTransform& Noise3D::SceneNode::GetLocalTransform()
{
	return mTransform;
}

bool Noise3D::SceneNode::IsRoot()
{
	return mIsRoot;
}
