
/*******************************************************************

								cpp£ºRigidTransform
			Desc: an interface for different kinds of transform 
			forms and their conversion

*******************************************************************/

#include <Noise3D.h>
#include "RigidTransform.h"

using namespace Noise3D;

//WARNING: 
//1.DirectXMath generate ROW major matrix
//2. XM-generated matrix elements are accessed with m[ROW][COLOMN]
//3. Effects11::SetMatrix will also re-arrange the memory layout (transpose again, not identical to memcpy to constant buffer)

Noise3D::RigidTransform::RigidTransform():
	mPosition(0,0,0),
	mQuaternion(XMQuaternionIdentity())
{
}

Noise3D::RigidTransform::RigidTransform(const RigidTransform & t):
	mPosition(t.mPosition),
	mQuaternion(t.mQuaternion)
{
}

void Noise3D::RigidTransform::SetPosition(Vec3 vPos)
{
	mPosition = vPos;
}

void Noise3D::RigidTransform::SetPosition(float x, float y, float z)
{
	mPosition = Vec3(x, y, z);
}

void Noise3D::RigidTransform::Move(Vec3 deltaPos)
{
	mPosition += deltaPos;
}

void Noise3D::RigidTransform::Move(float dx, float dy, float dz)
{
	mPosition += Vec3(dx, dy, dz);
}

Vec3 Noise3D::RigidTransform::GetPosition() const 
{
	return mPosition;
}

Vec3 Noise3D::RigidTransform::GetEulerAngleZXY() const
{
	//Quaternion--->Matrix
	Matrix rotMat=RigidTransform::GetRotationMatrix();

	//Matrix---->EulerAngle (Gimbal lock is dealt with inside the conversion function)
	Vec3 euler = mFunc_RotationMatrixToEulerZXY(rotMat);

	return euler;
}

N_EULER_ANGLE_ZYZ Noise3D::RigidTransform::GetEulerAngleZYZ() const
{
	//Quaternion--->Matrix
	Matrix rotMat = RigidTransform::GetRotationMatrix();

	//Matrix---->EulerAngle (Gimbal lock is dealt with inside the conversion function)
	N_EULER_ANGLE_ZYZ euler = mFunc_RotationMatrixToEulerZYZ(rotMat);

	return euler;
}

Quaternion Noise3D::RigidTransform::GetQuaternion() const 
{
	return mQuaternion;
}

Matrix Noise3D::RigidTransform::GetRotationMatrix() const
{
	//Quaternion---->Matrix
	//ref.1 : https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Conversion_to_and_from_the_matrix_representation
	//ref.2 : Tomas K.M. , Eric H., Naty H.. Real Time Rendering 3rd Edition , p76-p77, 2008.
	//here we use COLUMN vector
	/*
				[1-2y^2-2z^2		2xy-2zw				2xz+2yw			]
	R(q) =	[2xy+2zw				1-2x^2-2z^2		2yz-2xw			]
				[2xz-2yw				2yz+2xw				1-2x^2-2y^2	]
	*/
	return XMMatrixRotationQuaternion(mQuaternion);
}


void Noise3D::RigidTransform::Rotate(Vec3 axis, float angle)
{
	//AxisAngle---->Quaternion
	// COLUMN vector
	//https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.quaternion.xmquaternionrotationrollpitchyaw%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
	/*Axis(x,y,z), angle(a)
					[xsin(a/2)	]
	q(x,y,z=	[ysin(a/2)	]
					[zsin(a/2)	]
					[cos(y/2)	]
	*/
	Quaternion deltaRotQ= XMQuaternionRotationAxis(axis, angle);
	RigidTransform::Rotate(deltaRotQ);
}

bool Noise3D::RigidTransform::Rotate(Quaternion q)
{
	if (!Ut::TolerantEqual(q.Length(),1.0f, 0.0001f))
	{
		ERROR_MSG("SetRotation: Error! input is not a unit quaternion!");
		return false;
	}

	//to combine two quaternion rotation, just use it like rotation matrix
	//(left-multiply a quaternion)
	//https://math.stackexchange.com/questions/331539/combining-rotation-quaternions
	//https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
	mQuaternion = q * mQuaternion;
	return true;
}

void Noise3D::RigidTransform::Rotate(float pitch_x, float yaw_y, float roll_z)
{
	Vec3 euler = RigidTransform::GetEulerAngleZXY();

	//rotate with delta euler angle
	euler += Vec3(pitch_x, yaw_y, roll_z);

	//Euler---->Quaternion
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
	mQuaternion = XMQuaternionRotationRollPitchYaw(euler.x, euler.y, euler.z);
}

bool Noise3D::RigidTransform::Rotate(const Matrix & deltaRotMat)
{
	//top left 3x3 must be orthonormal
	bool isOrtho = mFunc_CheckTopLeft3x3Orthonomal(deltaRotMat);
	if (!isOrtho)
	{
		ERROR_MSG("RigidTransform: failed to (delta) Rotate with matrix because top left 3x3 sub-matrix isn't orthonormal.");
		return false;
	}

	//Quaternion---->Matrix
	Matrix currentMat=RigidTransform::GetRotationMatrix();

	//left-multiply/concatenate a delta rotation matrix
	currentMat = deltaRotMat * currentMat;

	//Matrix---->Quaternion
	mQuaternion =  XMQuaternionRotationMatrix(currentMat);

	return true;
}

void Noise3D::RigidTransform::SetRotation(Vec3 axis, float angle)
{
	//AxisAngle---->Quaternion
	// COLUMN vector
	//https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.quaternion.xmquaternionrotationrollpitchyaw%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
	/*Axis(x,y,z), angle(a)
					[xsin(a/2)	]
	q(x,y,z=	[ysin(a/2)	]
					[zsin(a/2)	]
					[cos(y/2)	]
	*/
	mQuaternion =  XMQuaternionRotationAxis(axis,angle);
}

bool Noise3D::RigidTransform::SetRotation(Quaternion q)
{
	if (!Ut::TolerantEqual(q.Length() ,1.0f, 0.0001f))
	{
		ERROR_MSG("SetRotation: Error! input is not a unit quaternion!");
		return false;
	}
	mQuaternion = q;
	return true;
}

void Noise3D::RigidTransform::SetRotation(float pitch_x, float yaw_y, float roll_z)
{
	//Euler---->Quaternion
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
	mQuaternion = XMQuaternionRotationRollPitchYaw(pitch_x, yaw_y, roll_z);
}

void Noise3D::RigidTransform::SetRotation(Vec3 eulerAngles)
{
	RigidTransform::SetRotation(eulerAngles.x, eulerAngles.y, eulerAngles.z);
}

bool Noise3D::RigidTransform::SetRotation(const Matrix & mat)
{
	//top left 3x3 must be orthonormal
	bool isOrtho = mFunc_CheckTopLeft3x3Orthonomal(mat);
	if (!isOrtho)
	{
		ERROR_MSG("RigidTransform: failed to set rotation with matrix because top left 3x3 sub-matrix isn't orthonormal.");
		return false;
	}

	//Matrix---->Quaternion
	mQuaternion = XMQuaternionRotationMatrix(mat);
	return true;
}

void Noise3D::RigidTransform::InvertRotation()
{
	//q^(-1)=q*/|q|
	mQuaternion = XMQuaternionInverse(mQuaternion);
}

Vec3 Noise3D::RigidTransform::TransformVector_Rigid(Vec3 vec)const 
{
	Vec3 outVec = Vec3(0, 0, 0);
	if (vec != Vec3(0, 0, 0))
	{
		//Quaternion Rotation: R(q) = qpq^(-1), with p=(vec3, 0), a pure quaternion
		//https://zh.wikipedia.org/wiki/%E5%9B%9B%E5%85%83%E6%95%B0%E4%B8%8E%E7%A9%BA%E9%97%B4%E6%97%8B%E8%BD%AC
		Quaternion v = Quaternion(vec.x, vec.y, vec.z, 0);
		Quaternion q = mQuaternion;
		Quaternion q_inv = XMQuaternionInverse(mQuaternion);
		Quaternion p_rotated = q * v *q_inv;
		//extract rotated vector
		outVec = Vec3(p_rotated.x, p_rotated.y, p_rotated.z);
	}

	//plus translation
	outVec += mPosition;
	return outVec;
}

void Noise3D::RigidTransform::SetRigidTransform(const RigidTransform & t)
{
	SetRotation(t.GetQuaternion());
	SetPosition(t.GetPosition());
}

void Noise3D::RigidTransform::SetRigidTransformMatrix(const Matrix & mat)
{
	mPosition = Vec3(mat.m[3][0], mat.m[3][1], mat.m[3][2]);
	RigidTransform::SetRotation(mat);
}

Matrix Noise3D::RigidTransform::GetRigidTransformMatrix() const
{
	Matrix outMat = RigidTransform::GetRotationMatrix();

	//this generate an COLUMN matrix
	/*out.m[3][0] = 0.0f;
	out.m[3][1] = 0.0f;
	out.m[3][2] = 0.0f;
	out.m[3][3] = 1.0f;
	out.m[0][3] = mPosition.x;
	out.m[1][3] = mPosition.y;
	out.m[2][3] = mPosition.z;*/

	//this generate an ROW matrix
	outMat.m[0][3] = 0.0f;
	outMat.m[1][3] = 0.0f;
	outMat.m[2][3] = 0.0f;
	outMat.m[3][3] = 1.0f;
	outMat.m[3][0] = mPosition.x;
	outMat.m[3][1] = mPosition.y;
	outMat.m[3][2] = mPosition.z;
	return outMat;
}


/****************************************************

									PRIVATE

****************************************************/
bool Noise3D::RigidTransform::mFunc_CheckTopLeft3x3Orthonomal(const Matrix & mat)
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

	Matrix T = mat.Transpose();
	Matrix mat1 = mat * T;
	Matrix mat2 = T * mat;
	bool isOrthonormal = true;

	//check multiplied result's top left 3x3 sub-matrix whether it's identity
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			//diagonal elements
			if (i == j)
			{
				if (!Ut::TolerantEqual(mat1.m[i][j], 1.0f, 0.001f))
				{
					isOrthonormal = false;
					break;
				}
				if (!Ut::TolerantEqual(mat2.m[i][j], 1.0f, 0.001f))
				{
					isOrthonormal = false;
					break;
				}
			}
			else//non-diagonal elements
			{
				if (!Ut::TolerantEqual(mat1.m[i][j], 0.0f, 0.001f))
				{
					isOrthonormal = false;
					break;
				}
				if (!Ut::TolerantEqual(mat2.m[i][j], 0.0f, 0.001f))
				{
					isOrthonormal = false;
					break;
				}
			}
		}
	}

	return isOrthonormal;
}

Vec3 Noise3D::RigidTransform::mFunc_RotationMatrixToEulerZXY(const Matrix & mat) const
{
	/*c1,s1~Yaw_Y ; c2,s2~Pitch_X ; c3,s3~Roll_Z
	COLUMN MAJOR
	R(x,y,z) = Ry * Rx * Rz
		[c1c3+s1s2s3		c3s1s2-c1s3			c2s1	]
	=	[c2s3					c2c3						-s2	]
		[c1s2s3-s1c3		s1s3+c1c3s2		c1c2	]
	/*
	yaw_Y = atan2(s1c2,c1c3)=atan2(m02,m22)
	pitch_X = arcsin(s2)=arcsin(-m12)
	roll_Z = atan2(c2s3, c2c3)=atan2(m10,m11)
	
	WARNING : gimbal lock situation (c2==0) should be carefully dealt with
	*/

	/*
	DirectXMath ROW Major :
	R(x,y,z) = Ry * Rx * Rz
		[c1c3+s1s2s3		c2s3		c1s2s3-s1c3		]
	=	[c3s1s2-c1s3		c2c3		s1s3+c1c3s2	]
		[c2s1					-s2		c1c2					]

		yaw_Y = atan2(s1c2,c1c3)=atan2(m20,m22)
		pitch_X = arcsin(s2)=arcsin(-m21)
		roll_Z = atan2(c2s3, c2c3)=atan2(m01,m11)
	/*

	*/
	Vec3 outEuler = Vec3(0,0,0);

	//XM-generated row major matrix
	if (mat.m[2][1] != 1.0f && mat.m[2][1] != -1.0f)
	{
		outEuler.x = asinf(-mat.m[2][1]);//pitch
		outEuler.y = atan2f(mat.m[2][0], mat.m[2][2]);//yaw
		outEuler.z = atan2f(mat.m[0][1], mat.m[1][1]);//roll
	}
	else
	{
		//gimbal lock case, the matrix degenerate into:
		/*
		1. when pitch== - pi/2 
		(column major)
				[ cos(y+z)			-sin(y+z)		0]
		R=	[0						0					1]
				[-sin(y+z)			-cos(y+z)		0]

		(row major)
				[ cos(y+z)		0		-sin(y+z)	]
		R=	[-sin(y+z)		0		-cos(y+z)	]
				[0					1		0				]

		2. when pitch==pi/2
		(column major)
				[ cos(y-z)			-sin(y-z)		0]
		R=	[0						0					-1]
				[sin(y-z)			cos(y-z)		0]

		(row major)
				[cos(y-z)		0		sin(y-z)	]
		R=	[-sin(y-z)		0		cos(y-z)	]
				[0					-1		0				]
		*/
		if (mat.m[2][1] == -1.0f)//-sin(pitch)=-1.0f, pitch=pi/2
		{
			outEuler.x = Ut::PI / 2.0f;
			outEuler.z = 0.0f;//arbitrarily set a value
			outEuler.y = outEuler.z + atan2f(mat.m[0][2], mat.m[0][0]);	//yaw_y - roll_z = atan2(m02,m00) , 2 DOF left (row major)
		}
		else//-sin(pitch) = 1.0f, pitch = -pi/2
		{
			outEuler.x = -Ut::PI / 2.0f;
			outEuler.z = 0.0f;
			outEuler.y = -outEuler.z + atan2f(-mat.m[1][0], mat.m[0][0]);//yaw_y + roll_z = atan2(-m10,m00), 2 DOF (row major)
		}
	}
	return outEuler;
}

N_EULER_ANGLE_ZYZ Noise3D::RigidTransform::mFunc_RotationMatrixToEulerZYZ(const Matrix & mat) const
{
	/*c1,s1~Yaw_Y ; c2,s2~Pitch_X ; c3,s3~Roll_Z
	COLUMN MAJOR
	R = Rz1 * Ry2 * Rz3
		[c1c2c3-s1s3		-c3s1-c1c2s3		c1s2	]
	=	[c1s3+c2c3s1		c1c3-c2s1s3			s1s2	]
		[-c3s2					s2s3						c2		]
	
	but the order the angle should be converted 
	z3 is the first applied to vector, y2 the second, z1 the last, completely reverse of the order of matrix mul
	z1' = z3 = atan2(s2s3, c3s2) = atan2(m21, -m20)
	y2' = y2 = acos(m22)
	z3' = z1 = atan2(s1s2, c1s2) = atan2(m12, m02)

	WARNING : gimbal lock situation (s2==0) should be carefully dealt with
	*/

	/*
	DirectXMath ROW Major :
		R =
		[c1c2c3-s1s3		c1s3+c2c3s1		-c3s2	]
	=	[-c3s1-c1c2s3		c1c3-c2s1s3			s2s3		]
		[c1s2					s1s2						c2			]

		z1' = z3 = atan2(s2s3, c3s2) = atan2(m12, -m02)
		y2' = y2 = acos(m22)
		z3' = z1 = atan2(s1s2, c1s2) = atan2(m21, m20)
	/*

	*/
	N_EULER_ANGLE_ZYZ outEuler;

	//XM-generated row major matrix
	if (mat.m[2][2] !=1.0f && mat.m[2][2]!=-1.0f)
	{
		outEuler.angleZ1 = atan2(mat.m[1][2], -mat.m[0][2]);
		outEuler.angleY2 = acos(mat.m[2][2]);
		outEuler.angleZ3 = atan2(mat.m[2][1], mat.m[2][0]);
	}
	else
	{
		//gimbal lock case, the matrix degenerate into:
		/*
		1. when y2==-pi, ==>s2=0, c2=-1
		(row major)
				[-c1c3-s1s3		c1s3-c3s1		0]
		R=	[-c3s1+c1s3		c1c3+s1s3	0]
				[0						0					-1]

				[-c3'c1'-s3's1'		c3s1'-c1's3'		0	]
			=	[-c1's3'+c3's1'		c3'c1'+s3's1'	0	]
				[0							0						-1	]

				[-cos(z1-z3)			sin(z1-z3)		0	]
			=	[sin(z1-z3)			cos(z1-z3)	0	]
				[0							0					-1	]


		2. when y2==pi, ==>s2=0, c2=1
		(row major)
				[c1c3-s1s3		c1s3+c3s1	0]
		R=	[-c3s1-c1s3		c1c3-s1s3		0]
				[0						0					1]

				[c3'c1'-s3's1'		c3s1'+c1's3'		0	]
			=	[-c1's3'-c3's1'		c3'c1'-s3's1'		0	]
				[0							0						1	]

				[cos(z1+z3)			sin(z1+z3)	0	]
			=	[-sin(z1+z3)			cos(z1+z3)	0	]
				[0							0					1	]
		*/
		if (mat.m[2][2] == -1.0f)//y2=-pi, cos(y2)=-1.0f
		{
			outEuler.angleY2 = -Ut::PI;
			outEuler.angleZ1= 0.0f;//arbitrarily set a value
			outEuler.angleZ3 = outEuler.angleZ1 - atan2(mat.m[0][1], mat.m[1][1]);//z1-z3=atan2
		}
		else//y2=0, cos(y2)=1.0f
		{
			outEuler.angleY2 = 0.0f;
			outEuler.angleZ1 = 0.0f;//arbitrarily set a value
			outEuler.angleZ3 = -outEuler.angleZ1 + atan2(mat.m[0][1], mat.m[1][1]);//z1+z3=atan2
		}
	}
	return outEuler;
}
