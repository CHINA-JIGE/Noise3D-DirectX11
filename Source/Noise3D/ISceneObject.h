
/***********************************************************************

								 h£ºISceneObject
			scene object's interface; can be attached to SceneNode

************************************************************************/
#pragma once

namespace Noise3D
{
	class SceneNode;

	//class inherited from this base interface can be attach to scene node.
	class ISceneObject
	{
	public:

		ISceneObject():m_pParentSceneNode(nullptr){}

		SceneNode* GetParentSceneNode();

		void	AttachToSceneNode(SceneNode* pNode);

		bool IsAttachedToSceneNode();

		virtual N_AABB GetLocalAABB() = 0;//require concrete geometry data, won't impl here

		//bounding box of transformed bounding box
		virtual N_AABB ComputeWorldAABB_Fast();

		virtual N_AABB ComputeWorldAABB_Accurate() = 0;//require concrete geometry data, won't impl here

	private:

		SceneNode* m_pParentSceneNode;

	};

};
