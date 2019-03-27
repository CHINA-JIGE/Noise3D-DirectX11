
/*********************************************************

								Logical Sphere
		geometry representation that is not based on polygon.
		instead, it's based on analytic description

************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ LogicalSphere :
		public ILogicalShape,
		public Collidable
	{
	public:

		//NVECTOR3 ComputeVec(float theta_pitch, float phi_yaw);

		void SetRadius(float r);

		float GetRadius();

		//ISceneObject::
		virtual NOISE_SCENE_OBJECT_TYPE GetObjectType()const override;

		//ISceneObject::
		virtual N_AABB GetLocalAABB() override;

		//ISceneObject::
		virtual N_AABB ComputeWorldAABB_Accurate() override;

		//ILogicalShape::
		virtual float ComputeArea() override;

		//ILogicalShape::
		virtual bool IsPointInside(NVECTOR3 p) override;

	private:

		LogicalSphere();

		~LogicalSphere();

		float mRadius;

	};

}