
/*********************************************************

								Logical Box
		geometry representation that is not based on polygon.
		instead, it's based on analytic description

************************************************************/

#pragma once

namespace Noise3D
{
	class LogicalShapeManager;

	//side's of AABB (or box in local space)
	enum NOISE_BOX_FACET
	{
		POS_X=0, 
		NEG_X=1, 
		POS_Y=2, 
		NEG_Y=3, 
		POS_Z=4, 
		NEG_Z=5,
		_INVALID_FACET=0xffffffff
	};

	class /*_declspec(dllexport)*/ LogicalBox :
		public ILogicalShape
	{
	public:

		//compute normal on given surface position
		//('facet' is used to explicit decide  face, avoid the rounding error on box's corner, might round to adjacent facet)
		static Vec3 ComputeNormal(NOISE_BOX_FACET facet);

		static Vec2 ComputeUV(const N_AABB& aabb, NOISE_BOX_FACET facet, Vec3 pos);

		Vec2 ComputeUV(NOISE_BOX_FACET facet, Vec3 pos);

		//box is AABB in local space
		//void SetLocalBox(N_AABB aabb);
		void SetSizeXYZ(Vec3 size);

		//box is AABB in local space
		N_AABB GetLocalBox() const;

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

		//ILogicalShape::
		//virtual bool IsPointInside(Vec3 p) override;

	private:
		//header include's order matters
		//friend LogicalBox* LogicalShapeManager::CreateBox(SceneNode*, N_UID, Vec3);
		friend LogicalShapeManager;
		
		friend class IFactory<LogicalBox>;

		LogicalBox();

		~LogicalBox();

		//N_AABB mLocalBox;
		Vec3 mSize;

	};

}