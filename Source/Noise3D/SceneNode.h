
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
	class SceneNode :
		protected GeneralTreeNode<>
	{
	public:

		SceneNode* GetParentNode();

		void	AttachToParentNode(SceneNode* pNode);//attach current node to parent

		void	AttachChildNode(SceneNode* pNode);//attach to child node

		bool IsAttachedToParentNode();

		SceneNode* CreateChildNode();//directly create a new scene node and attach to current node

		SceneNode* GetChildNode(int index);

		uint32_t GetChildNodeCount();

		AffineTransform& GetLocalTransform();//relative to its father node (if current node is attached to root node, then local=world)

		AffineTransform EvalWorldTransform();//relative to its scene graph root node


		//bool IsRoot();

	protected:

		SceneNode();

		//SceneNode(bool isRoot);

		std::vector<ISceneObject*> mSceneObjectList;

		std::vector<SceneNode*> mChildNodeList;

		SceneNode* m_pParentSceneNode;

		AffineTransform mTransform;

		//bool mIsRoot;//scene graph's root

	};

};