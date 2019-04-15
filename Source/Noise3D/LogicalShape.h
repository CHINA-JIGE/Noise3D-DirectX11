
/*********************************************************

								Logical Geometry
		geometry representation that is not based on polygon.
		instead, it's based on analytic representation.
		e.g., you'll need a center and radius to represent a sphere

************************************************************/

#pragma once

namespace Noise3D
{

	class /*_declspec(dllexport)*/ ILogicalShape:
		public ICollidableSceneObject,
		public GI::IAdvancedGiMaterialOwner
	{
	public:

		ILogicalShape(){};

		virtual ~ILogicalShape() { };

		//ISceneObject::
		virtual NOISE_SCENE_OBJECT_TYPE GetObjectType()const override = 0 ;

		//ISceneObject::
		virtual N_AABB GetLocalAABB() override= 0;

		//ISceneObject::
		virtual N_AABB ComputeWorldAABB_Accurate() override = 0;

		//Compute Area for the shape(might be useful for area lighting)
		virtual float ComputeArea() =0 ;

	protected:

		friend IFactory<ILogicalShape>;

	};

}