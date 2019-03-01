
/***********************************************************************

								 h£ºScene Node
				desc: a single node in a scene graph, storing 
				transformation and some geometry, object binding infos
				(similar to OGRE's idea)

************************************************************************/

#pragma once

namespace Noise3D
{
	//Scene Node is a base class that contains Tree operation and transformation info
	class SceneNode
	{
	public:

		//void AttachChildSceneObject(ISceneObject* pObj);

		SceneNode* GetParentSceneNode();

		void	AttachToParentSceneNode(SceneNode* pNode);

		bool IsAttachedToSceneNode();

		AffineTransform& GetLocalTransform();//relative to its father node

		AffineTransform EvalWorldTransform();//relative to its scene graph root node

		SceneNode* CreateChildSceneNode();

		bool IsRoot();

	protected:

		SceneNode(bool isRoot);

		//std::vector<ISceneObject*> mSceneObjectList;

		std::vector<SceneNode*> mChildNodeList;

		SceneNode* m_pParentSceneNode;

		AffineTransform mTransform;

		bool mIsRoot;//scene graph's root

	};

};