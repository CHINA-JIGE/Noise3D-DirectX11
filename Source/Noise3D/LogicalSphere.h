
/*********************************************************

								Logical Sphere
		geometry representation that is not based on polygon.
		instead, it's based on analytic description

************************************************************/

#pragma once

namespace Noise3D
{
	class LogicalShapeManager;

	//(2019.3.27) all 'Scale' is ignored when evaluating world transform
	class /*_declspec(dllexport)*/ LogicalSphere :
		public ILogicalShape
	{
	public:

		//Vec3 ComputeVec(float theta_pitch, float phi_yaw);

		void SetRadius(float r);

		float GetRadius() const;

		//ISceneObject::
		virtual NOISE_SCENE_OBJECT_TYPE GetObjectType()const override;

		//ISceneObject::
		virtual N_AABB GetLocalAABB() override;

		//ISceneObject::
		virtual N_AABB ComputeWorldAABB_Accurate() override;

		//ILogicalShape::
		virtual float ComputeArea() override;

		//ISceneObject::
		virtual N_BoundingSphere ComputeWorldBoundingSphere_Accurate() override;

		//ILogicalShape::
		//virtual bool IsPointInside(Vec3 p) override;

	private:
		//header include's order matters
		//friend LogicalSphere* LogicalShapeManager::CreateSphere(SceneNode*, N_UID, float);
		friend LogicalShapeManager;

		friend IFactory<LogicalSphere>;

		LogicalSphere();

		~LogicalSphere();

		float mRadius;

	};

}