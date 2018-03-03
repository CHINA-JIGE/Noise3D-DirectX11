/************************************************************************

							CPP:  	Noise Camera
				
1，关于YawPitchRoll的正角定义：（其实是左手系正角的定义）
			Yaw:俯视图顺时针转
			Pitch:y往负的方向转（低头）
			Roll:逆时针转


************************************************************************/

#pragma once
#include "Noise3D.h"

using namespace Noise3D;

Noise3D::ICamera::ICamera() :
	mRotateX_Pitch(0),
	mRotateY_Yaw(0),
	mRotateZ_Roll(0),
	mViewAngleY_Radian((float)60 / 180 * MATH_PI),
	mAspectRatio(1.5f),
	mPosition(0, 0, 0),
	mLookat(1.0f, 0, 0),
	mNearPlane(1.0f),
	mFarPlane(1000.0f)
{
	D3DXMatrixPerspectiveFovLH(&mMatrixProjection,mViewAngleY_Radian,mAspectRatio,mNearPlane,mFarPlane);
	D3DXMatrixIdentity(&mMatrixView);
}

Noise3D::ICamera::~ICamera()
{
};


void Noise3D::ICamera::Destroy()
{

};

void	Noise3D::ICamera::SetLookAt(NVECTOR3 vLookat)
{
	//pos and lookat can't superpose each other
	NVECTOR3 dir = vLookat - mPosition;
	if (D3DXVec3Length(&dir) < 0.0001f)return;

	mLookat=vLookat;
	mFunction_UpdateRotation();
};

void	Noise3D::ICamera::SetLookAt(float x,float y,float z)
{
	NVECTOR3 tmpLookat(x,y,z);
	//pos and lookat can't superpose each other
	NVECTOR3 dir = tmpLookat - mPosition;
	if (D3DXVec3Length(&dir) < 0.0001f)return;

	mLookat=tmpLookat;
	mFunction_UpdateRotation();
};

NVECTOR3	Noise3D::ICamera::GetLookAt()
{
	return  mLookat;
};

NVECTOR3 	Noise3D::ICamera::GetDirection()
{
	return mLookat-mPosition;
}

void	Noise3D::ICamera::SetPosition(NVECTOR3 vPos)
{
	//pos and lookat can't superpose each other
	NVECTOR3 dir = mLookat - vPos;
	if (D3DXVec3Length(&dir) < 0.0001f)return;

	mPosition=vPos;
	mFunction_UpdateRotation();
}

void	Noise3D::ICamera::SetPosition(float x,float y,float z)
{
	NVECTOR3 tmpPos(x,y,z);
	NVECTOR3 dir = mLookat - tmpPos;
	if (D3DXVec3Length(&dir) < 0.0001f)return;

	mPosition=tmpPos;
	mFunction_UpdateRotation();
};

NVECTOR3 Noise3D::ICamera::GetPosition()
{
	return mPosition;
};

void	Noise3D::ICamera::Move(NVECTOR3 vRelativePos)
{
	D3DXVec3Add(&mPosition,&mPosition,&vRelativePos);
	D3DXVec3Add(&mLookat, &mLookat, &vRelativePos);

};

void	Noise3D::ICamera::Move(float relativeX,float relativeY,float relativeZ)
{
	mPosition += NVECTOR3(relativeX, relativeY, relativeZ);
};

void	Noise3D::ICamera::SetRotation(float RX_Pitch,float RY_Yaw,float RZ_Roll)//要更新Lookat
{
	SetRotationX_Pitch(RX_Pitch);
	SetRotationY_Yaw(RY_Yaw);
	SetRotationZ_Roll(RZ_Roll);
	mFunction_UpdateDirection();
};

void	Noise3D::ICamera::SetRotationY_Yaw(float angleY)
{
	mRotateY_Yaw = angleY;
	mFunction_UpdateDirection();
};

void	Noise3D::ICamera::SetRotationX_Pitch(float AngleX)
{
	//clamp to [-pi/2,pi/2]
	mRotateX_Pitch = Clamp(AngleX, -MATH_PI / 2.0f, MATH_PI / 2.0f);

	mFunction_UpdateDirection();
};

void	Noise3D::ICamera::SetRotationZ_Roll(float AngleZ)
{
	//roll翻滚不需要更新lookat
	mRotateZ_Roll = AngleZ;
}

float Noise3D::ICamera::GetRotationY_Yaw()
{
	return mRotateY_Yaw;
}

float Noise3D::ICamera::GetRotationX_Pitch()
{
	return mRotateX_Pitch;
}

float Noise3D::ICamera::GetRotationZ_Roll()
{
	return mRotateZ_Roll;
}

void Noise3D::ICamera::RotateY_Yaw(float angleY)
{
	SetRotationY_Yaw(mRotateY_Yaw + angleY);
	mFunction_UpdateDirection();
};

void Noise3D::ICamera::RotateX_Pitch(float angleX)
{
	float newAngle = mRotateX_Pitch + angleX;
	SetRotationX_Pitch(newAngle);
	mFunction_UpdateDirection();
};

void Noise3D::ICamera::RotateZ_Roll(float angleZ)
{
	SetRotationZ_Roll(mRotateZ_Roll + angleZ);
	mFunction_UpdateDirection();
};

void Noise3D::ICamera::fps_MoveForward(float fSignedDistance, bool enableYAxisMovement)
{
	//...Yaw Angle Starts at Z axis ( left-handed system) 
	/*		Z
			|    / A
			|_ /   
		O	|/__________ X		

			angle AOZ is the yaw angle
	
	*/

	NVECTOR3 relativePos;

	//generate a direction first (later multiply it with fDist)
	if(enableYAxisMovement)
	{ 
		relativePos.x = sinf(mRotateY_Yaw) *cosf(mRotateX_Pitch);
		relativePos.z = cosf(mRotateY_Yaw) *cosf(mRotateX_Pitch);
		relativePos.y = sinf(mRotateX_Pitch);
	}
	else
	{
		relativePos.x = sinf(mRotateY_Yaw);
		relativePos.z = cosf(mRotateY_Yaw);
		relativePos.y = 0;
	}

	// length * unit_dir
	relativePos *= fSignedDistance;

	//relative movement
	Move(relativePos);

}

void Noise3D::ICamera::fps_MoveRight(float fSignedDistance, bool enableYAxisMovement)
{
	//...Yaw Angle Starts at Z axis ( left-handed system) 
	/*		
		Z
		|    / A
		|_ /
	O	|/__________ X

	angle AOZ is the yaw angle

	*/

		NVECTOR3 relativePos;

		//generate a direction first (later multiply it with fDist)
		if (enableYAxisMovement)
		{
			relativePos.x = cosf(mRotateY_Yaw) *cosf(mRotateX_Pitch);
			relativePos.z = -sinf(mRotateY_Yaw) *cosf(mRotateX_Pitch);
			relativePos.y = sinf(mRotateX_Pitch);
		}
		else
		{
			relativePos.x = cosf(mRotateY_Yaw);
			relativePos.z = -sinf(mRotateY_Yaw);
			relativePos.y = 0;
		}

		// length * unit_dir
		relativePos *= fSignedDistance;

		//relative movement
		Move(relativePos);
}

void Noise3D::ICamera::fps_MoveUp(float fSignedDistance)
{
	mPosition += NVECTOR3(0, fSignedDistance, 0);
	mLookat += NVECTOR3(0, fSignedDistance, 0);
}

void Noise3D::ICamera::GetViewMatrix(NMATRIX & outMat)
{
	mFunction_UpdateViewMatrix();
	outMat = mMatrixView;
}

void Noise3D::ICamera::GetProjMatrix(NMATRIX & outMat)
{
	mFunction_UpdateProjMatrix();
	outMat = mMatrixProjection;
}

void Noise3D::ICamera::GetInvViewMatrix(NMATRIX & outMat)
{
	mFunction_UpdateViewMatrix();
	auto invPtr = D3DXMatrixInverse(&outMat, nullptr, &mMatrixView);
	if(invPtr==nullptr)ERROR_MSG("Camera : Inverse of View Matrix not exist!")
}

void Noise3D::ICamera::OptimizeForQwertyPass1(const IMesh * pScreenDescriptor)
{
	//NOTE: the position of camera won't be changed.
	//compute and adjust intrinsic parameter(fov, aspectRatio....) and posture(rotation/lookat)
	//to maximize the projection area (in pixel) of Qwerty 3D's virtual screen while the whole
	//screen can be rendered to render target without being partially occluded.
	//This function can increase the resolution of the down-sampling source for 
	//Qwerty3D's Pass 2 (Qwerty Distortion)


	//(2018.3.3)but optimal solution is too design...i'll go with a non-optimal solution now..
	//Lookat = the gravitational center of screen descriptor vertices
	//eulerAngleZ (roll) = 0;
	//AspectRatio & fov = just big enough to hold the whole perspective projection of screen descriptor
	const std::vector<N_DefaultVertex>& vb = *pScreenDescriptor->GetVertexBuffer();

	//1. Adjust view matrix (lookat, roll angle). the center of virtual screen
	NVECTOR3 scrCenterPos = { 0,0,0 };
	for (auto v : vb)
	{
		scrCenterPos += v.Pos;
	}
	scrCenterPos /= float(vb.size());
	ICamera::SetLookAt(scrCenterPos);
	ICamera::SetRotationZ_Roll(0);

	//2. Adjust projection matrix (fov & aspectRatio)
	float halfBoundingRectWidth=0.0f;
	float halfBoundingRectHeight = 0.0f;

	//traverse vertices of scr descriptor to calculate a bounding rectangle, then fov/aspectRatio
	for (auto v : vb)
	{
		NMATRIX viewMat;
		ICamera::GetViewMatrix(viewMat);
		NVECTOR3 posV = {
			v.Pos.x *  viewMat.m[0][0] + v.Pos.y *  viewMat.m[1][0] + v.Pos.z *  viewMat.m[2][0] + 1.0f * viewMat.m[3][0],
			v.Pos.x *  viewMat.m[0][1] + v.Pos.y *  viewMat.m[1][1] + v.Pos.z *  viewMat.m[2][1] + 1.0f * viewMat.m[3][1],
			v.Pos.x *  viewMat.m[0][2] + v.Pos.y *  viewMat.m[1][2] + v.Pos.z *  viewMat.m[2][2] + 1.0f * viewMat.m[3][2]
		};

		//project to plane z=1
		NVECTOR2 posH = { posV.x / posV.z,posV.y / posV.z };
		if (halfBoundingRectWidth < abs(posH.x))halfBoundingRectWidth = abs(posH.x);
		if (halfBoundingRectHeight < abs(posH.y))halfBoundingRectHeight = abs(posH.y);

		//fovY = arctan(y/z), aspectRatio=width/height
		ICamera::SetViewAngle_Radian(2.0f * atan(halfBoundingRectHeight / 1.0f), halfBoundingRectWidth / halfBoundingRectHeight);
	}
}

void	Noise3D::ICamera::SetViewFrustumPlane(float iNearPlaneZ,float iFarPlaneZ)
{
	if ( (iNearPlaneZ >0) && (iFarPlaneZ>iNearPlaneZ))
	{
		mNearPlane	= iNearPlaneZ;
		mFarPlane	=	iFarPlaneZ;
	}
};

void Noise3D::ICamera::SetViewAngle_Degree(float fovY_Degree,float fAspectRatio)
{
	if(fovY_Degree>0 && (mViewAngleY_Radian <180.0f)){ mViewAngleY_Radian	= fovY_Degree * MATH_PI / 180.0f;	}
	if(fAspectRatio>0){mAspectRatio	= fAspectRatio;}
}

void Noise3D::ICamera::SetViewAngle_Radian(float fovY_Radian, float fAspectRatio)
{
	if (fovY_Radian>0 && (mViewAngleY_Radian <(MATH_PI))) { mViewAngleY_Radian = fovY_Radian; }
	if (fAspectRatio>0) { mAspectRatio = fAspectRatio; }
};



/************************************************************************
											PRIVATE	
************************************************************************/

void	Noise3D::ICamera::mFunction_UpdateProjMatrix()
{
	D3DXMatrixPerspectiveFovLH(
		&mMatrixProjection,
		mViewAngleY_Radian,
		mAspectRatio,
		mNearPlane,
		mFarPlane);

	//(2018.x.xx)Id3dx11EffectMatrix.setMatrix is used, transpose can be neglected
	//D3DXMatrixTranspose(&mMatrixProjection,&mMatrixProjection);
};

void	Noise3D::ICamera::mFunction_UpdateViewMatrix()
{
	NMATRIX	tmpMatrixTranslation;
	NMATRIX	tmpMatrixRotation;
	//先对齐原点
	D3DXMatrixTranslation(&tmpMatrixTranslation, -mPosition.x, -mPosition.y, -mPosition.z);
	//然后用yaw-pitch-roll的逆阵转到view空间
	D3DXMatrixRotationYawPitchRoll(&tmpMatrixRotation, mRotateY_Yaw, mRotateX_Pitch, mRotateZ_Roll);
	//for ortho matrix, transpose=inverse
	D3DXMatrixTranspose(&tmpMatrixRotation,&tmpMatrixRotation);
	//先平移，再旋转
	D3DXMatrixMultiply(&mMatrixView,&tmpMatrixTranslation,&tmpMatrixRotation);

	//D3DXMatrixTranspose(&mMatrixView,&mMatrixView);
};

void	Noise3D::ICamera::mFunction_UpdateRotation()
{
	//main function: change Euler Angle after the direction changes

	//update Direction
	D3DXVec3Subtract(&mDirection,&mLookat,&mPosition);

	//pitch angle： tan = y/sqr(x^2+z^2))
	/*	CAUTION：	atan ranged [-pi/2,pi/2]  
					atan2 ranged [-pi,pi] 		*/

	NVECTOR3 xzProjDir(mDirection.x, 0, mDirection.z);
	//always positive
	float radiusLength = D3DXVec3Length(&xzProjDir);
	//atan2(y,x) , radiusLength is constantly positive, pitch angle will range [-pi/2,pi/2] 
	//pitch : rotate downside is positive (the definition of rotation in left-handed frame)
	mRotateX_Pitch = atan2(-mDirection.y,radiusLength );

	//YAW： tan = -x/z
	mRotateY_Yaw = atan2(mDirection.x,mDirection.z);//俯视图yaw是顺时针正角

	//roll angle: direction update doesn't change ROLL angle
};

void	Noise3D::ICamera::mFunction_UpdateDirection()
{
	//update lookat according to new posture
	float tmpDirectionLength = D3DXVec3Length(&mDirection);
	//z+ axis is the original posture.
	//mDirection.x =- tmpDirectionLength* sin(mRotateY_Yaw)* cos(mRotateX_Pitch);
	mDirection.x =tmpDirectionLength* sin(mRotateY_Yaw)* cos(mRotateX_Pitch);
	mDirection.z =tmpDirectionLength* cos(mRotateY_Yaw)*cos(mRotateX_Pitch);
	mDirection.y =tmpDirectionLength* sin(mRotateX_Pitch);
	D3DXVec3Add(&mLookat,&mPosition,&mDirection);
};
