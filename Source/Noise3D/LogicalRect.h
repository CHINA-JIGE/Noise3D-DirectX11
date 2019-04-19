
/*********************************************************

								Logical Rect
		geometry representation that is not based on polygon.
		instead, it's based on analytic description

************************************************************/

#pragma once

namespace Noise3D
{
	class LogicalShapeManager;

	//plane that the rect lies on
	enum NOISE_RECT_ORIENTATION
	{
		RECT_XY = 0,
		RECT_XZ = 1,
		RECT_YZ = 2
	};

	//create a rect centered at origin
	class /*_declspec(dllexport)*/ LogicalRect :
		public ILogicalShape
	{
	public:

		//perhaps a ray-rect intersection should have 2 intersection points with opposite normals
		void SetOrientation(NOISE_RECT_ORIENTATION ori);

		NOISE_RECT_ORIENTATION GetOrientation()const;

		void SetSize(float width, float height);

		void SetSize(Vec2 size);

		Vec2 GetSize()const;

		//generate random sample point without world transform
		Vec3 GenLocalRandomPoint();

		//ISceneObject::
		virtual NOISE_SCENE_OBJECT_TYPE GetObjectType()const override;

		//ISceneObject::
		virtual N_AABB GetLocalAABB() override;

		//ISceneObject::
		virtual N_AABB ComputeWorldAABB_Accurate() override;

		//ISceneObject::
		virtual N_BoundingSphere ComputeWorldBoundingSphere_Accurate() override;

		//ILogicalShape::
		virtual float ComputeArea() override;

	private:

		//header include's order matters
		//friend LogicalRect* LogicalShapeManager::CreateRect(SceneNode*, N_UID, Vec2, NOISE_RECT_ORIENTATION);
		friend LogicalShapeManager;

		friend class IFactory<LogicalRect>;

		LogicalRect();

		~LogicalRect();

		Vec2 mSize;

		NOISE_RECT_ORIENTATION mOrientation;
	};
}