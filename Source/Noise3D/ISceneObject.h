
/***********************************************************************

								 h£ºISceneObject
			scene object's interface; can be attached to SceneNode

************************************************************************/
#pragma once

namespace Noise3D
{
	class SceneNode;

	enum NOISE_SCENE_OBJECT_TYPE
	{
		_INVALID,//not implemented(?) if some scene object is tagged 'INVALID', then there might be a bug
		MESH,
		GRAPHIC_OBJECT,
		CAMERA,
		LIGHT,
		SWEEPING_TRAIL
	};


	//class that inherits from this base interface can be attach to scene node.
	class ISceneObject
	{
	public:

		ISceneObject();

		virtual ~ISceneObject();

		//customized RTTI info to avoid dynamic_cast's overhead?
		virtual NOISE_SCENE_OBJECT_TYPE GetObjectType() const = 0;

		//require concrete geometry data.
		virtual N_AABB GetLocalAABB() = 0;

		//require concrete geometry data, won't impl here
		virtual N_AABB ComputeWorldAABB_Accurate() = 0;

		//bounding box of transformed bounding box
		virtual N_AABB ComputeWorldAABB_Fast();

		//object name/uid (initialized in IFactory<> creation)
		std::string GetName();

		//attach to scene node, update double-way connection (node--object)
		void AttachToSceneNode(SceneNode* pNode);

		//detach from scene node, delete double-way connection(node--object)
		void DetachFromSceneNode();

		//..
		SceneNode* GetAttachedSceneNode() const;

	protected:

		//friend SceneNode to let it directly set 'AttachedSceneNode'
		//and avoid loop invoke(SceneNode and SceneObject needs double-way connection)
		friend void SceneNode::AttachSceneObject(ISceneObject *);
		friend void SceneNode::DetachSceneObject(ISceneObject*);
		SceneNode* m_pAttachedSceneNode;

		void mFunc_InitSceneObject(const std::string& name, SceneNode* pAttachedNode);

		std::string mUid;//object name, initialized once in IFactory<XXX>


	};

};
