/***********************************************************

								SceneNode

			A Single Node of Scene Graph/Tree.
		Each Node's transform are based on its parent node,
		so hierarchical transformation is available.

***********************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Noise3D::SceneNode::SceneNode(bool isRoot):
	mIsRoot(isRoot)
{
}

AffineTransform& Noise3D::SceneNode::GetLocalTransform()
{
	return mTransform;
}

bool Noise3D::SceneNode::IsRoot()
{
	return mIsRoot;
}
