
/***********************************************************************

                           h£ºCamera

************************************************************************/

#pragma once


namespace Noise3D
{
	//(2019.3.8) WARNING: currently Camera doesn't support hierarchical transform.
	//its transform is directly the World transform

	class /*_declspec(dllexport)*/ Camera:
		public ISceneObject
	{
	public:

		void		LookAt(Vec3 vLookat);

		void		LookAt(float x, float y, float z);

		void		SetDirection(Vec3 viewDir);

		void		SetDirection(float x, float y, float z);

		Vec3		GetLookAtPos();

		Vec3		GetDirection();

		void		SetProjectionType(bool isPerspective = true);//true for perspective, false for orthographic

		void		GetViewMatrix(Matrix& outMat);

		void		GetProjMatrix(Matrix& outMat);

		void		GetViewInvMatrix(Matrix& outMat);

		void		SetViewFrustumPlane(float fNearPlaneZ, float fFarPlaneZ);//for perspective

		void		SetViewAngle_Degree(float fovY_Degree, float fAspectRatio);//for perspective

		void		SetViewAngle_Radian(float fovY_Radian, float fAspectRatio);//for perspective

		void		SetOrthoViewSize(float width, float height);//for orhtographic

		//fire a ray from cam pos(0,0,0),return ray dir, used for picking or path tracing
		Vec3	FireRay_ViewSpace(PixelCoord2 pixelCoord, size_t backBuffPxWidth, size_t backBuffPxHeight);

		//fire a ray from cam pos(0,0,0), return ray dir, used for picking or path tracing
		Vec3	FireRay_ViewSpace(Vec2 uv);

		//fire a ray from cam pos using pixel coord, return an world space ray. used for picking or path tracing
		N_Ray	FireRay_WorldSpace(PixelCoord2 pixelCoord, size_t backBuffPxWidth, size_t backBuffPxHeight);

		//fire a ray from cam pos using NDC [-1,1]x[-1,1], return an world space ray. used for picking or path tracing
		N_Ray	FireRay_WorldSpace(Vec2 uv);

		void		fps_MoveForward(float fSignedDistance, bool enableYAxisMovement = false);

		void		fps_MoveRight(float fSignedDistance, bool enableYAxisMovement = false);

		void		fps_MoveUp(float fSignedDistance);

		void		OptimizeForQwertyPass1(const Mesh* pScreenDescriptor);

		//ISceneObject::
		virtual	N_AABB GetLocalAABB() override;

		//ISceneObject::
		virtual	N_AABB ComputeWorldAABB_Accurate() override;

		//ISceneObject::
		virtual	NOISE_SCENE_OBJECT_TYPE GetObjectType() const override;

		//(2019.3.8) sorry, currently Camera doesn't support hierarchical transform.
		//its world transform will simply be the local transform. because lookat
		RigidTransform& GetWorldTransform();

		//SceneNode::
		Matrix EvalWorldAffineTransformMatrix() = delete;

		//SceneNode::
		void EvalWorldAffineTransformMatrix(Matrix& outWorldMat, Matrix& outWorldInvTranspose) = delete;
		
		//SceneNode::
		AffineTransform& GetLocalTransform() =delete;


	private:

		friend  class Renderer;

		friend IFactory<Camera>;

		friend class SceneManager;//for ISceneObject init

		Camera();

		~Camera();

		//update view direction/lookat after rotation
		void		mFunc_UpdateViewDir();

		//update rotation after 'lookat'/SetDirection
		void		mFunc_UpdateRotation();

		Vec3	mLookat;
		bool		mIsPerspective;
		float		mViewAngleY_Radian;//radian
		float		mAspectRatio;// horizontal/vertical
		float		mNearPlane;
		float		mFarPlane;
		float		mOrthoViewWidth;//used in orthographic projection
		float		mOrthoViewHeight;//used in orthographic projection

	};

};


