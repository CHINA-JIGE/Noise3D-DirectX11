
/***********************************************************************

								 h£ºISceneObject
			scene object's interface; can be attached to SceneNode

************************************************************************/
#pragma once

namespace Noise3D
{
	class SceneNode;
	enum NOISE_SCENE_OBJECT_TYPE;

	//class inherited from this base interface can be attach to scene node.
	class ISceneObject
	{
	public:

		ISceneObject():m_pParentSceneNode(nullptr){}

		virtual ~ISceneObject() {};

		SceneNode* GetParentSceneNode();

		void	AttachToSceneNode(SceneNode* pNode);

		bool IsAttachedToSceneNode();

		virtual N_AABB GetLocalAABB() = 0;//require concrete geometry data, won't impl here

		//bounding box of transformed bounding box
		N_AABB ComputeWorldAABB_Fast();

		virtual N_AABB ComputeWorldAABB_Accurate() = 0;//require concrete geometry data, won't impl here

		std::string GetName();//object name/uid

		virtual NOISE_SCENE_OBJECT_TYPE GetObjectType() = 0;//customized RTTI info to avoid dynamic_cast's overhead?

	private:

		SceneNode* m_pParentSceneNode;

		std::string mUid;//object name, initialized once

	};

};
