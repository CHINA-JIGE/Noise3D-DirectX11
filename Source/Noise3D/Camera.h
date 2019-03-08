
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

		//Lookat & Position
		void		LookAt(NVECTOR3 vLookat);

		void		LookAt(float x, float y, float z);

		void		SetDirection(NVECTOR3 viewDir);

		void		SetDirection(float x, float y, float z);

		NVECTOR3	GetLookAtPos();

		NVECTOR3	GetDirection();

		void		SetProjectionType(bool isPerspective = true);//true for perspective, false for orthographic

		void		GetViewMatrix(NMATRIX& outMat);

		void		GetProjMatrix(NMATRIX& outMat);

		void		GetInvViewMatrix(NMATRIX& outMat);

		void		SetViewFrustumPlane(float fNearPlaneZ, float fFarPlaneZ);//for perspective

		void		SetViewAngle_Degree(float fovY_Degree, float fAspectRatio);//for perspective

		void		SetViewAngle_Radian(float fovY_Radian, float fAspectRatio);//for perspective

		void		SetOrthoViewSize(float width, float height);//for orhtographic


		void		fps_MoveForward(float fSignedDistance, bool enableYAxisMovement = false);

		void		fps_MoveRight(float fSignedDistance, bool enableYAxisMovement = false);

		void		fps_MoveUp(float fSignedDistance);

		void		OptimizeForQwertyPass1(const Mesh* pScreenDescriptor);

		//ISceneObject::
		virtual	N_AABB GetLocalAABB() override;

		//ISceneObject::
		virtual	N_AABB ComputeWorldAABB_Accurate() override;

		//ISceneObject::
		virtual	NOISE_SCENE_OBJECT_TYPE GetObjectType() override;

		//(2019.3.8) sorry, currently Camera doesn't support hierarchical transform.
		//its transform is directly the World transform. because lookat
		AffineTransform& GetWorldTransform();

		//SceneNode::
		NMATRIX EvalWorldAffineTransformMatrix() = delete;

		//SceneNode::
		void EvalWorldAffineTransformMatrix(NMATRIX& outWorldMat, NMATRIX& outWorldInvTranspose) = delete;
		
		//SceneNode::
		AffineTransform& GetLocalTransform() =delete;


	private:

		friend  class Renderer;

		friend IFactory<Camera>;

		Camera();

		~Camera();

		//update view direction/lookat after rotation
		void		mFunc_UpdateViewDir();

		//update rotation after 'lookat'/SetDirection
		void		mFunc_UpdateRotation();

		NVECTOR3	mLookat;
		bool		mIsPerspective;
		float		mViewAngleY_Radian;//radian
		float		mAspectRatio;
		float		mNearPlane;
		float		mFarPlane;
		float		mOrthoViewWidth;
		float		mOrthoViewHeight;

	};

};


