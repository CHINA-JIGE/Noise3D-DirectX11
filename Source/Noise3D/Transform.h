
/*******************************************************************

								h£ºITransform
			Desc: an interface for different kinds of transform 
			forms and their conversion

		1.Euler Angle
		2.Quaternion
		3.Rotation Matrix
		4.Pos/Scale/Lookat and other affine transform info
*******************************************************************/
#pragma once

namespace Noise3D
{

	class /*_declspec(dllexport)*/ Transform
	{
	public:

		void				LookAt(NVECTOR3 vLookat);

		void				LookAt(float x, float y, float z);

		NVECTOR3	GetLookAtPos();

		void				SetPosition(NVECTOR3 vPos);

		void				SetPosition(float x, float y, float z);

		NVECTOR3	GetPosition();

		NVECTOR3	GetDirection();

		void				SetEulerY_Yaw(float AngleX);//radian angle

		void				SetEulerX_Pitch(float AngleY);//radian angle

		void				SetEulerZ_Roll(float AngleZ);//radian angle

		void				SetEulerAngle(NVECTOR3 euler);

		NVECTOR3	GetEulerAngle();

		void				RotateAxisAngle(NVECTOR3 axis, float angle);//left-handed rotate

		void				RotateAxisAngle(const NMATRIX& mat);//left-handed rotate

		void				Rotate(NQUATERNION q);

	private:

		//1.Rotation: Euler Angles (yaw-pitch-roll, tait-bryan angle)
		float	mRotateX_Pitch;
		float	mRotateY_Yaw;
		float	mRotateZ_Roll;

		//2.Rotation: Quaternion (2018.9.14, like Unity, quaternion could be the main representation of rotation on CPU-end)
		NQUATERNION mQuaternion;

		//3.Rotation: Matrix (2018.9.14, this is the Only representation on GPU-end)
		NMATRIX mRotationMatrix;

		//4.Affine transform info 
		float mScale;
		NVECTOR3	mPosition;
		NVECTOR3	mLookat;
		NVECTOR3	mDirection;
	};

}