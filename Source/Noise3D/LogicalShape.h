
/*********************************************************

								Logical Geometry
		geometry representation that is not based on polygon.
		instead, it's based on analytic representation.
		e.g., you'll need a center and radius to represent a sphere

************************************************************/

#pragma once

namespace Noise3D
{
	class LambertMaterial;

	class /*_declspec(dllexport)*/ ILogicalShape:
		public ICollidableSceneObject
	{
	public:

		ILogicalShape(): m_pMaterial(nullptr) {};

		virtual ~ILogicalShape() { m_pMaterial = nullptr; };

		//ISceneObject::
		virtual NOISE_SCENE_OBJECT_TYPE GetObjectType()const override = 0 ;

		//ISceneObject::
		virtual N_AABB GetLocalAABB() override= 0;

		//ISceneObject::
		virtual N_AABB ComputeWorldAABB_Accurate() override = 0;

		//Compute Area for the shape(might be useful for area lighting)
		virtual float ComputeArea() =0 ;

		//determine if point is inside this shape
		//virtual bool IsPointInside(Vec3 p) = 0;

		//..
		void SetMaterial(LambertMaterial* pMat) 
		{
			m_pMaterial = pMat;
		};

	protected:

		friend IFactory<ILogicalShape>;

		LambertMaterial* m_pMaterial;

	};

}