
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
		public TreeNodeTemplate<SceneNode, SceneGraph>
	{
	public:

		AffineTransform& GetLocalTransform();//relative to its father node (if current node is attached to root node, then local=world)

		//traverse through scene graph(from given node to root) and concatenate local transforms.
		//any computed accumulated world matrix will be CACHED in ScenNode;
		NMATRIX EvalWorldTransformMatrix();

		bool IsBoundWithSceneObject();

	protected:

		SceneNode(bool isBoundWidthObject);

		~SceneNode();

		bool mIsBoundWithSceneObject;

		AffineTransform mTransform;

		//accumulated world transform matrix CACHE(from current to path)
		//(2019.3.6) i decide not to do this optimization
		//NMATRIX mEvaluatedWorldTransform;

	};

	class SceneGraph :
		public TreeTemplate<SceneNode>
	{
	public:


	private:

	};

};