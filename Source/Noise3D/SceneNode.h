
/***********************************************************************

								 h£ºScene Node
				desc: a single node in a scene graph, storing 
				transformation and some geometry, object binding infos
				(similar to OGRE's idea)

************************************************************************/

#pragma once

namespace Noise3D
{
	class ISceneObject;

	//Scene Node is a base class that contains Tree operation and transformation info
	class SceneNode :
		public TreeNodeTemplate<SceneNode>
	{
	public:

		AffineTransform& GetLocalTransform();//relative to its father node (if current node is attached to root node, then local=world)

		AffineTransform EvalWorldTransform();//relative to its scene graph root node


		//bool IsRoot();

	protected:

		SceneNode();

		~SceneNode();

		std::vector<ISceneObject*> mSceneObjectList;

		std::vector<SceneNode*> mChildNodeList;

		SceneNode* m_pParentSceneNode;

		AffineTransform mTransform;

	};

};