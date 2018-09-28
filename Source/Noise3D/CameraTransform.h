
/***********************************************************************

                           h£ºCamera

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ CameraTransform: 
		public RigidTransform
	{
	public:

		CameraTransform();

		//Lookat & Position
		void		LookAt(NVECTOR3 vLookat);

		void		LookAt(float x, float y, float z);
		
		void		SetDirection(NVECTOR3 viewDir);

		void		SetDirection(float x, float y, float z);

		NVECTOR3	GetLookAtPos();

		NVECTOR3	GetDirection();

		void				GetViewMatrix(NMATRIX& outMat);

		void				GetProjMatrix(NMATRIX& outMat);

		void				GetInvViewMatrix(NMATRIX& outMat);

		void				SetViewFrustumPlane(float fNearPlaneZ, float fFarPlaneZ);

		void				SetViewAngle_Degree(float fovY_Degree, float fAspectRatio);

		void				SetViewAngle_Radian(float fovY_Radian, float fAspectRatio);

	private:

		//update view direction/lookat after rotation
		void				mFunc_UpdateViewDir();

		//update rotation after 'lookat'/SetDirection
		void				mFunc_UpdateRotation();

		NVECTOR3	mLookat;
		float		mViewAngleY_Radian;//radian
		float		mAspectRatio;
		float		mNearPlane;
		float		mFarPlane;
	};
}