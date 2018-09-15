
/*******************************************************************

								h£ºRigidTransform
			Desc: an interface for different kinds of transform 
			forms and their conversion

		1.Euler Angle
		2.Quaternion
		3.Rotation Matrix
		4.Lookat and other rigid transform info

		WARNING: if you modified one kind of transform info, it might 
		have impact on other posture representation. For example,
		modification to Quaternion will cause changes in rotation matrix
		and euler angles, direction, etc.

*******************************************************************/
#pragma once

namespace Noise3D
{
	//translation & rotation
	class /*_declspec(dllexport)*/ RigidTransform
	{
	public:

		//Lookat & Position
		void		LookAt(NVECTOR3 vLookat);

		void		LookAt(float x, float y, float z);

		void		SetPosition(NVECTOR3 vPos);

		void		SetPosition(float x, float y, float z);

		//get value
		NVECTOR3	GetPosition();

		NVECTOR3	GetLookAtPos();

		NVECTOR3	GetDirection();

		NVECTOR3	GetEulerAngle();

		NQUATERNION GetQuaternion();

		void GetRotationMatrix(NMATRIX& outMat);

		//delta rotation
		void		Rotate(NVECTOR3 axis, float angle);//left-handed rotate

		void		Rotate(NQUATERNION q);

		void		Rotate(NVECTOR3 deltaEulerAngle);

		//set absolute rotation
		void		SetEulerY_Yaw(float AngleX);//radian angle

		void		SetEulerX_Pitch(float AngleY);//radian angle

		void		SetEulerZ_Roll(float AngleZ);//radian angle

		void		SetRotation(NVECTOR3 axis, float angle);

		void		SetRotation(NQUATERNION q);

		void		SetRotation(NVECTOR3 eulerAngle);


	private:

		//1.Rotation: Euler Angles (yaw-pitch-roll, tait-bryan angle)
		float	mRotateX_Pitch;
		float	mRotateY_Yaw;
		float	mRotateZ_Roll;

		//2.Rotation: Matrix (2018.9.14, this is the Only representation on GPU-end)
		NMATRIX mRotationMatrix;

		//3.Rotation: Quaternion (2018.9.14, like Unity, quaternion could be the main representation of rotation on CPU-end)
		NQUATERNION mQuaternion;

		//4.Affine transform info 
		NVECTOR3	mPosition;
		NVECTOR3	mLookat;
		NVECTOR3	mDirection;
	};

}