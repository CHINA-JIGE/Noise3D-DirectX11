
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


	//class inherited from this base interface can be attach to scene node.
	class ISceneObject :
		public SceneNode
	{
	public:

		ISceneObject(bool isRoot);

		virtual ~ISceneObject();

		//customized RTTI info to avoid dynamic_cast's overhead?
		virtual NOISE_SCENE_OBJECT_TYPE GetObjectType() = 0;

		//require concrete geometry data.
		virtual N_AABB GetLocalAABB() = 0;

		//require concrete geometry data, won't impl here
		virtual N_AABB ComputeWorldAABB_Accurate() = 0;

		//bounding box of transformed bounding box
		N_AABB ComputeWorldAABB_Fast();

		//object name/uid (initialized in IFactory<> creation)
		std::string GetName();

	protected:

		std::string mUid;//object name, initialized once in IFactory<> using dynamic_cast

	};

};
