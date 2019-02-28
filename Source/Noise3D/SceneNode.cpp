/***********************************************************

								SceneNode

			A Single Node of Scene Graph/Tree.
		Each Node's transform are based on its parent node,
		so hierarchical transformation is available.

***********************************************************/

#include "Noise3D.h"

using namespace Noise3D;

void Noise3D::SceneNode::AttachSceneObject(ISceneObject * pObj)
{
	if (pObj != nullptr)
	{
		//don't invoke ISceneObject->AttachXXX, it'll cause circular call
		mSceneObjectList.push_back(pObj);
	}
}

AffineTransform& Noise3D::SceneNode::GetTransform()
{
	return mTransform;
}
