
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
	struct N_EULER_ANGLE_ZYZ
	{
		N_EULER_ANGLE_ZYZ():angleZ1(0.0f), angleY2(0.0f), angleZ3(0.0f){}
		float angleZ1;
		float angleY2;
		float angleZ3;
	};


	//RigidTransform implements translation & rotation, and avaible rotation representations are:
	//***Quaternion(Main representation in CPU, others
	//***Rotation Matrix
	//***Euler Angle(for human's convenient)
	//***AxisAngle(not stored/sync, only used to construct rotations in other form)
	//plz refer to https://github.com/CHINA-JIGE/JiGe-NotesOfVisualComputing/tree/master/Rotation
	//for my notes about 3D rotation forms' conversions.
	//NOTE: When it comes to rotation, only Quaternion is stored. Rotation via other method will cause
	//				construction of other rotate form. (e.g. if i want a delta rotation via rotation matrix, then a 
	//				current posture rotation matrix will be constructed from the quaternion first, apply, then 
	//				convert the transformed matrix back to quaternion.
	class /*_declspec(dllexport)*/ RigidTransform
	{
	public:

		RigidTransform();

		RigidTransform(const RigidTransform& t);

		void		SetPosition(NVECTOR3 vPos);

		void		SetPosition(float x, float y, float z);

		void		Move(NVECTOR3 deltaPos);

		void		Move(float dx, float dy, float dz);

		//get value
		NVECTOR3	GetPosition() const;

		//get euler angle (decomposed by ZXY convention, but the vec3 x/y/z corresponds to their own axis's rotation.
		NVECTOR3	GetEulerAngleZXY() const;

		//get euler angle (decomposed by ZYZ convention, but the vec3 x/y/z corresponds to their own axis's rotation.
		N_EULER_ANGLE_ZYZ GetEulerAngleZYZ() const;

		NQUATERNION GetQuaternion() const;

		NMATRIX GetRotationMatrix() const;

		//delta rotation
		void		Rotate(NVECTOR3 axis, float angle);//left-handed rotate

		bool		Rotate(NQUATERNION q);

		//use ZXY euler angle to construct one delta rotation(not simply adds euler angles)
		void		Rotate(float pitch_x, float yaw_y, float roll_z);

		bool		Rotate(const NMATRIX& deltaRotMat);

		//set absolute rotation
		void		SetRotation(NVECTOR3 axis, float angle);

		bool		SetRotation(NQUATERNION q);

		void		SetRotation(float pitch_x, float yaw_y, float roll_z);

		void		SetRotation(NVECTOR3 eulerAngles);

		bool		SetRotation(const NMATRIX& mat);//top left 3x3 sub - matrix must be a orthonormal

		void		InvertRotation();

		//apply rigid transformation to vector/point and output
		NVECTOR3 TransformVector_Rigid(NVECTOR3 vec) const;

		void		SetRigidTransform(const RigidTransform& t);

		void		SetRigidTransformMatrix(const NMATRIX& mat);

		NMATRIX		GetRigidTransformMatrix() const;

	private:

		bool mFunc_CheckTopLeft3x3Orthonomal(const NMATRIX& mat);

		NVECTOR3 mFunc_RotationMatrixToEulerZXY(const NMATRIX& mat) const;

		N_EULER_ANGLE_ZYZ mFunc_RotationMatrixToEulerZYZ(const NMATRIX& mat) const;

		//Rotation is stored as Quaternion (2018.9.14, like Unity, quaternion could be the main representation of rotation on CPU-end)
		NQUATERNION mQuaternion;
		NVECTOR3	mPosition;
	};

}