
/***********************************************************************

                           h：NoiseCamera

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ ICamera
	{
	public:


		void				Destroy();

		void				SetLookAt(NVECTOR3 vLookat);//要更新旋转角

		void				SetLookAt(float x, float y, float z);//要更新旋转角

		NVECTOR3	GetLookAt();

		void				SetPosition(NVECTOR3 vPos);

		void				SetPosition(float x, float y, float z);

		NVECTOR3	GetPosition();
		
		NVECTOR3	GetDirection();

		void				Move(NVECTOR3 vRelativePos);//pos and lookat

		void				Move(float relativeX, float relativeY, float relativeZ);

		void				SetViewFrustumPlane(float fNearPlaneZ, float fFarPlaneZ);

		void				SetViewAngle_Degree(float fovY_Degree, float fAspectRatio);

		void				SetViewAngle_Radian(float fovY_Radian, float fAspectRatio);

		void				SetRotation(float RX_Pitch, float RY_Yaw, float RZ_Roll);//Lookat vector will be updated

		void				SetRotationY_Yaw(float AngleX);//radian angle

		void				SetRotationX_Pitch(float AngleY);//radian angle

		void				SetRotationZ_Roll(float AngleZ);//radian angle

		float				GetRotationY_Yaw();

		float				GetRotationX_Pitch();

		float				GetRotationZ_Roll();

		void				RotateY_Yaw(float angleY);

		void				RotateX_Pitch(float angleX);

		void				RotateZ_Roll(float angleZ);

		void				fps_MoveForward(float fSignedDistance, bool enableYAxisMovement = false);

		void				fps_MoveRight(float fSignedDistance, bool enableYAxisMovement = false);

		void				fps_MoveUp(float fSignedDistance);

		void				GetViewMatrix(NMATRIX& outMat);

		void				GetProjMatrix(NMATRIX& outMat);

		void				GetInvViewMatrix(NMATRIX& outMat);

		void				OptimizeForQwertyPass1(const IMesh* pScreenDescriptor);

	private:

		friend  class IRenderer;

		friend IFactory<ICamera>;

		ICamera();

		~ICamera();

		void		NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_UpdateProjMatrix();//invoked before update to shader
		void		NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_UpdateViewMatrix();
		void		mFunction_UpdateRotation();
		void		mFunction_UpdateDirection();
		float		mViewAngleY_Radian;//radian
		float		mAspectRatio;
		float		mNearPlane;
		float		mFarPlane;

		NVECTOR3		mPosition;
		NVECTOR3		mLookat;
		NVECTOR3		mDirection;

		NMATRIX			mMatrixView;
		NMATRIX			mMatrixProjection;

		float			mRotateX_Pitch;
		float			mRotateY_Yaw;
		float			mRotateZ_Roll;

	};

};


