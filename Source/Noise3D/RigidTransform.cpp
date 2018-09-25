
/*******************************************************************

								cpp£ºITransform
			Desc: an interface for different kinds of transform 
			forms and their conversion

*******************************************************************/

#include <Noise3D.h>

using namespace Noise3D;

void Noise3D::RigidTransform::SetPosition(NVECTOR3 vPos)
{
	mPosition = vPos;
}

void Noise3D::RigidTransform::SetPosition(float x, float y, float z)
{
	mPosition = NVECTOR3(x, y, z);
}

bool Noise3D::RigidTransform::SetRotation(const NMATRIX & mat)
{
	//top left 3x3 must be orthonormal
	bool isOrtho = mFunc_CheckTopLeft3x3Orthonomal(mat);
	if (!isOrtho)
	{
		ERROR_MSG("RigidTransform: failed to set rotation with matrix because top left 3x3 sub-matrix isn't orthonormal.");
		return false;
	}

	//1.Matrix
	mRotationMatrix = mat;
	mRotationMatrix.m[3][3] = 1.0f;//homogeneous coordinate

	//2.Quaternion
	XMQuaternionRotationMatrix(mat);

	//3.Euler angle
	NVECTOR3 euler = mFunc_RotationMatrixToEuler(mRotationMatrix);
	mEulerX_Pitch = euler.x;
	mEulerY_Yaw = euler.y;
	mEulerZ_Roll = euler.z;
}

NMATRIX Noise3D::RigidTransform::GetTransformMatrix()
{ 
	NMATRIX out;
	out = mRotationMatrix;
	out.m[3][0] = 0.0f;
	out.m[3][1] = 0.0f;
	out.m[3][2] = 0.0f;
	out.m[3][3] = 1.0f;
	out.m[0][3] = mPosition.x;
	out.m[1][3] = mPosition.y;
	out.m[2][3] = mPosition.z;

	return out;
}

void Noise3D::RigidTransform::SetEulerX_Pitch(float angleX)
{
	//1.Euler angle
	mEulerX_Pitch = angleX;

	//2.Rotation matrix
	//pitch-x, yaw-y, roll-z, left-handed, zxy convention, COLUMN vector
	//https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.matrix.xmmatrixrotationrollpitchyaw%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
	/*c1,s1~Yaw_Y ; c2,s2~Pitch_X ; c3,s3~Roll_Z
	R(x,y,z) = Ry * Rx * Rz
		[c1c3+s1s2s3		c3s1s2-c1s3			c2s1	]
	=	[c2s3					c2c3						-s2	]
		[c1s2s3-s1c3		s1s3+c1c3s2		c1c2	]
	*/
	XMMatrixRotationRollPitchYaw(mEulerX_Pitch, mEulerY_Yaw, mEulerZ_Roll);

	//3.Quaternion
	//pitch-x, yaw-y, roll-z, left-handed, zxy convention, COLUMN vector
	//https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.quaternion.xmquaternionrotationrollpitchyaw%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
	/*c1,s1~Yaw_Y ; c2,s2~Pitch_X ; c3,s3~Roll_Z
	q(x,y,z) = qy * qx * qz
		[0				]	[sin(x/2)	]	[0				]
	=	[sin(y/2)	] *	[0				] *	[0				]
		[0				]	[0				]	[sin(z/2)	]
		[cos(y/2)	]	[cos(x/2	]	[cos(z/2)	]

		[cos(y/2)sin(x/2)cos(z/2)+sin(y/2)cos(x/2)sin(z/2)	]
	=	[sin(y/2)cos(x/2)cos(z/2)-cos(y/2)sin(x/2)sin(z/2)	]
		[-sin(y/2)sin(x/2)cos(z/2)+cos(y/2)cos(x/2)sin(z/2)	]
		[cos(y/2)cos(x/2)cos(z/2)+sin(y/2)sin(x/2)sin(z/2)	]
	*/
	XMQuaternionRotationRollPitchYaw(mEulerX_Pitch, mEulerY_Yaw, mEulerZ_Roll);
}

void Noise3D::RigidTransform::SetEulerY_Yaw(float angleY)
{
	//1.Euler angle
	mEulerY_Yaw = angleY;

	//2.Rotation matrix
	//pitch-x, yaw-y, roll-z, left-handed, zxy convention, COLUMN vector
	//https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.matrix.xmmatrixrotationrollpitchyaw%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
	/*c1,s1~Yaw_Y ; c2,s2~Pitch_X ; c3,s3~Roll_Z
	R(x,y,z) = Ry * Rx * Rz
		[c1c3+s1s2s3		c3s1s2-c1s3			c2s1	]
	=	[c2s3					c2c3						-s2	]
		[c1s2s3-s1c3		s1s3+c1c3s2		c1c2	]
	*/
	XMMatrixRotationRollPitchYaw(mEulerX_Pitch, mEulerY_Yaw, mEulerZ_Roll);

	//3.Quaternion
	//pitch-x, yaw-y, roll-z, left-handed, zxy convention, COLUMN vector
	//https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.quaternion.xmquaternionrotationrollpitchyaw%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
	/*c1,s1~Yaw_Y ; c2,s2~Pitch_X ; c3,s3~Roll_Z
	q(x,y,z) = qy * qx * qz
		[0				]	[sin(x/2)	]	[0				]
	=	[sin(y/2)	] *	[0				] *	[0				]
		[0				]	[0				]	[sin(z/2)	]
		[cos(y/2)	]	[cos(x/2	]	[cos(z/2)	]

		[cos(y/2)sin(x/2)cos(z/2)+sin(y/2)cos(x/2)sin(z/2)	]
	=	[sin(y/2)cos(x/2)cos(z/2)-cos(y/2)sin(x/2)sin(z/2)	]
		[-sin(y/2)sin(x/2)cos(z/2)+cos(y/2)cos(x/2)sin(z/2)	]
		[cos(y/2)cos(x/2)cos(z/2)+sin(y/2)sin(x/2)sin(z/2)	]
	*/
	XMQuaternionRotationRollPitchYaw(mEulerX_Pitch, mEulerY_Yaw, mEulerZ_Roll);

}

void Noise3D::RigidTransform::SetEulerZ_Roll(float angleZ)
{
	//1.Euler angle
	mEulerZ_Roll = angleZ;

	//2.Rotation matrix
	//pitch-x, yaw-y, roll-z, left-handed, zxy convention, COLUMN vector
	//https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.matrix.xmmatrixrotationrollpitchyaw%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
	/*c1,s1~Yaw_Y ; c2,s2~Pitch_X ; c3,s3~Roll_Z
		R(x,y,z) = Ry * Rx * Rz
		[c1c3+s1s2s3		c3s1s2-c1s3			c2s1	]
	=	[c2s3					c2c3						-s2	]
		[c1s2s3-s1c3		s1s3+c1c3s2		c1c2	]
	*/
	XMMatrixRotationRollPitchYaw(mEulerX_Pitch, mEulerY_Yaw, mEulerZ_Roll);

	//3.Quaternion
	//pitch-x, yaw-y, roll-z, left-handed, zxy convention, COLUMN vector
	//https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.quaternion.xmquaternionrotationrollpitchyaw%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
	/*c1,s1~Yaw_Y ; c2,s2~Pitch_X ; c3,s3~Roll_Z
		q(x,y,z) = qy * qx * qz
		[0				]	[sin(x/2)	]	[0				]
	=	[sin(y/2)	] *	[0				] *	[0				]
		[0				]	[0				]	[sin(z/2)	]
		[cos(y/2)	]	[cos(x/2	]	[cos(z/2)	]

		[cos(y/2)sin(x/2)cos(z/2)+sin(y/2)cos(x/2)sin(z/2)	]
	=	[sin(y/2)cos(x/2)cos(z/2)-cos(y/2)sin(x/2)sin(z/2)	]
		[-sin(y/2)sin(x/2)cos(z/2)+cos(y/2)cos(x/2)sin(z/2)	]
		[cos(y/2)cos(x/2)cos(z/2)+sin(y/2)sin(x/2)sin(z/2)	]
	*/
	XMQuaternionRotationRollPitchYaw(mEulerX_Pitch, mEulerY_Yaw, mEulerZ_Roll);
}

/****************************************************

									PRIVATE

****************************************************/
bool Noise3D::RigidTransform::mFunc_CheckTopLeft3x3Orthonomal(const NMATRIX & mat)
{
	//the determination of orthogonal matrix:
	//M * M^T = M^T * M = I
	//which is equivalent to :
	//		1.every column/row axis vector is orthogonal to other axis
	//		2.every column/row axis vector is unit length
	/*
	[ A ]							[1	0 0]
	[ B ]	* [ A | B | C ] = [0 1 0]
	[ C ]							[0 0 1]
	*/

	auto func_tolerantEqual = [](float lhs, float rhs, float errorLimit = 0.001f)->bool
	{
	//a little numerical error can be tolerated
	return (abs(lhs-rhs) < errorLimit);
	};

	NMATRIX T = mat.Transpose();
	NMATRIX mat1 = mat * T;
	NMATRIX mat2 = T * mat;
	bool isOrthonormal = true;

	//check multiplied result's top left 3x3 sub-matrix whether it's identity
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			//diagonal elements
			if (i == j)
			{
				if (!func_tolerantEqual(mat1.m[i][j], 1.0f, 0.001f))
				{
					isOrthonormal = false;
					break;
				}
				if (!func_tolerantEqual(mat2.m[i][j], 1.0f, 0.001f))
				{
					isOrthonormal = false;
					break;
				}
			}
			else//non-diagonal
			{
				if (!func_tolerantEqual(mat1.m[i][j], 0.0f, 0.001f))
				{
					isOrthonormal = false;
					break;
				}
				if (!func_tolerantEqual(mat2.m[i][j], 0.0f, 0.001f))
				{
					isOrthonormal = false;
					break;
				}
			}
		}
	}

	return isOrthonormal;
}

void Noise3D::RigidTransform::mFunc_RotationMatrixToEuler(const NMATRIX & mat)
{
	/*c1,s1~Yaw_Y ; c2,s2~Pitch_X ; c3,s3~Roll_Z
	R(x,y,z) = Ry * Rx * Rz
		[c1c3+s1s2s3		c3s1s2-c1s3			c2s1	]
	=	[c2s3					c2c3						-s2	]
		[c1s2s3-s1c3		s1s3+c1c3s2		c1c2	]
	*/

	/*
	yaw_Y = atan2(s1c2,c1c3)=atan2(m02,m22)
	pitch_X = arcsin(s2)=arcsin(-m12)
	roll_Z = atan2(c2s3, c2c3)=atan2(m10,m11)
	
	WARNING : gimbal lock situation (c2==0) should be carefully dealt with
	*/
	if (mat.m[1][2] != 1.0f && mat.m[1][2] != -1.0f)
	{
		mEulerX_Pitch = asinf(-mat.m[1][2]);
		mEulerY_Yaw = atan2f(mat.m[0][2], mat.m[2][2]);
		mEulerZ_Roll = atan2f(mat.m[1][0], mat.m[1][1]);
	}
	else
	{
		//gimbal lock case, the matrix degenerate into:
		/*
		1. when pitch==-pi/2
				[ cos(y+z)			-sin(y+z)		0]
		R=	[0						0					1]
				[-sin(y+z)			-cos(y+z)		0]

		2. when pitch==pi/2
				[ cos(y-z)			sin(y-z)			0]
		R=	[0						0					1]
				[-sin(y-z)			cos(y-z)		0]
		*/
		if (mat.m[1][2] == -1.0f)
		{
			mEulerX_Pitch = MATH_PI / 2.0f;
			mEulerZ_Roll = mEulerY_Yaw - atan2f(mat.m[0][1], mat.m[0][0]);	//yaw_y - roll_z = atan2(m01,m00) , 2 DOF left
		}
		else//-sin(pitch) = 1.0f
		{
			mEulerX_Pitch = -MATH_PI / 2.0f;
			mEulerZ_Roll = -mEulerY_Yaw + atan2f(-mat.m[0][1], mat.m[0][0]);//yaw_y + roll_z = atan2(-m01,m00), 2 DOF
		}
	}
}
