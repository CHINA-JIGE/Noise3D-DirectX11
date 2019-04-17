
/***********************************************************************

								 h£ºScene Node
				desc: a single node in a scene graph, storing 
				transformation and some geometry, object binding infos
				(similar to OGRE's idea)

************************************************************************/

#pragma once

namespace Noise3D
{

	class SceneGraph;
	class ISceneObject;

	//Scene Node is a base class that contains Tree operation and transformation info
	class SceneNode :
		public TreeNodeTemplate<SceneNode, SceneGraph>
	{
	public:

		SceneNode();

		~SceneNode();

		//relative to its father node (if current node is attached to root node, then local=world)
		AffineTransform& GetLocalTransform();

		//traverse through scene graph(from given node to root) and concatenate local transforms.
		//evaluated world transform (relative to root) won't cache in ScenNode by default
		//set 'cacheResult' to true in order to cache the evaluated world transform 
		//(which can be directly retrived until the cache is cleared)
		AffineTransform EvalWorldTransform(bool cacheResult = false);//normally the cacheResult should be set true by Noise3D(?)

		//similar to evalWorldTransform, except that it only count T & R
		AffineTransform EvalWorldTransform_Rigid(bool cacheResult = false);//normally the cacheResult should be set true by Noise3D(?)

		//clear world transform cache, disable direct retrival of cache in 'EvalWorldTransform'
		void ClearWorldTransformCache();

		//determine if world transform has been stored.
		bool IsWorldTransformCached();

		//attach scene object to this node
		void AttachSceneObject(ISceneObject* pObj);

		//detach scene object from this node, clear reference to pObj
		void DetachSceneObject(ISceneObject* pObj);

		bool IsAttachedSceneObject();

		uint32_t GetSceneObjectCount();

		ISceneObject* GetGiRenderable(uint32_t index);

	protected:

		AffineTransform mLocalTransform;

		std::vector<ISceneObject*> mAttachedSceneObjectList;

		//(2019.3.23)to avoid tremendous re-calculation by setting 'cacheResult'
		//when eval world transform matrix
		bool mIsWorldMatrixCached;
		//Matrix mWorldMatrixCache;
		AffineTransform mWorldTransformCache;

	};

	class SceneGraph :
		public TreeTemplate<SceneNode,SceneGraph>
	{
	public:


		void TraverseSceneObjects(NOISE_TREE_TRAVERSE_ORDER order, std::vector<ISceneObject*>& outResult) const;

		void TraverseSceneObjects(NOISE_TREE_TRAVERSE_ORDER order, SceneNode* pNode, std::vector<ISceneObject*>& outResult) const;

	private:

		friend class SceneManager;

		SceneGraph();

		~SceneGraph();

	};

};