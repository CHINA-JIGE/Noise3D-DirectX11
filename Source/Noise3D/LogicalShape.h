
/*********************************************************

								Logical Geometry
		geometry representation that is not based on polygon.
		instead, it's based on analytic representation.
		e.g., you'll need a center and radius to represent a sphere

************************************************************/

#pragma once

namespace Noise3D
{
	class Material;

	class /*_declspec(dllexport)*/ ILogicalShape:
		public ISceneObject
	{
	public:

		//ISceneObject::
		virtual NOISE_SCENE_OBJECT_TYPE GetObjectType()const override = 0 ;

		//ISceneObject::
		virtual N_AABB GetLocalAABB() override= 0;

		//ISceneObject::
		virtual N_AABB ComputeWorldAABB_Accurate() override = 0;

		//Compute Area for the shape(might be useful for area lighting)
		virtual float ComputeArea() =0 ;

		//determine if point is inside this shape
		virtual bool IsPointInside(NVECTOR3 p) = 0;

		//..
		void SetMaterial(Material* pMat) 
		{
			m_pMaterial = pMat;
		};

	protected:

		ILogicalShape() {};

		~ILogicalShape() {};

		Material* m_pMaterial;

	};

}