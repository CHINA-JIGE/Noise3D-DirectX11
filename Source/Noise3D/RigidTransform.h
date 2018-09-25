
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
	//RigidTransform implements translation & rotation, and avaible rotation representations are:
	//***Quaternion
	//***Rotation Matrix
	//***Euler Angle(for human's convenient)
	//***AxisAngle(not stored/sync, only used to construct rotations in other form)
	//plz refer to https://github.com/CHINA-JIGE/JiGe-NotesOfVisualComputing/tree/master/Rotation
	//for my notes about 3D rotation forms' conversions.
	//NOTE: when one representation changes, other representation will also change
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
		void		SetEulerY_Yaw(float angleY_radian);//radian angle

		void		SetEulerX_Pitch(float angleX_radian);//radian angle

		void		SetEulerZ_Roll(float angleZ_radian);//radian angle

		void		SetRotation(NVECTOR3 axis, float angle);

		void		SetRotation(NQUATERNION q);

		void		SetRotation(NVECTOR3 eulerAngle);

		bool		SetRotation(const NMATRIX& mat);//top left 3x3 sub - matrix must be a orthonormal

		//there is a "lookat" param that will affect rotation, and an affine matrix doesn't include a "lookat" param,  so this function brings confusion 
		//bool		SetTransformMatrix(const NMATRIX& rigidTrans);//top left 3x3 sub-matrix must be a orthogonal, rotation+translation

		NMATRIX		GetTransformMatrix();

		//output transformed vector/point
		void ApplyTransformToVec(NVECTOR4 vec);

	private:

		/*	
					----------------------(direct construct)--------------
	   			    |																		      \|/
				Euler----(direct construct)---->Matrix--------->Quaternion-------->AxisAngle

							
				Euler<----(decompose using mat elements)----Matrix<--------Quaternion
		*/
		bool mFunc_CheckTopLeft3x3Orthonomal(const NMATRIX& mat);

		void mFunc_RotationMatrixToEuler(const NMATRIX& mat);

		void mFunc_QuaternionToEuler(const NQUATERNION& q);

		//1.Rotation: Euler Angles (yaw-pitch-roll, tait-bryan angle)
		float	mEulerX_Pitch;
		float	mEulerY_Yaw;
		float	mEulerZ_Roll;

		//2.Rotation: Quaternion (2018.9.14, like Unity, quaternion could be the main representation of rotation on CPU-end)
		NQUATERNION mQuaternion;

		//3.Rotation+translation: Matrix (2018.9.14, this is the Only representation on GPU-end)
		NMATRIX mRotationMatrix;

		//4.Affine transform info 
		NVECTOR3	mPosition;
		NVECTOR3	mLookat;
		NVECTOR3	mDirection;
	};

}