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

ICamera::ICamera() :
	mRotateX_Pitch(0),
	mRotateY_Yaw(0),
	mRotateZ_Roll(0),
	mViewAngleY((float)60 / 180 * MATH_PI),
	mAspectRatio(1.5f),
	mPosition(0, 0, 0),
	mLookat(1.0f, 0, 0),
	mNearPlane(1.0f),
	mFarPlane(1000.0f)
{
	D3DXMatrixPerspectiveFovLH(&mMatrixProjection,mViewAngleY,mAspectRatio,mNearPlane,mFarPlane);
	D3DXMatrixIdentity(&mMatrixView);
}

ICamera::~ICamera()
{
};


void ICamera::Destroy()
{

};

void	ICamera::SetLookAt(NVECTOR3 vLookat)
{
	//pos and lookat can't superpose each other
	NVECTOR3 dir = vLookat - mPosition;
	if (D3DXVec3Length(&dir) < 0.0001f)return;

	mLookat=vLookat;
	mFunction_UpdateRotation();
};

void	ICamera::SetLookAt(float x,float y,float z)
{
	NVECTOR3 tmpLookat(x,y,z);
	//pos and lookat can't superpose each other
	NVECTOR3 dir = tmpLookat - mPosition;
	if (D3DXVec3Length(&dir) < 0.0001f)return;

	mLookat=tmpLookat;
	mFunction_UpdateRotation();
};

NVECTOR3	ICamera::GetLookAt()
{
	return  mLookat;
};

NVECTOR3 	ICamera::GetDirection()
{
	return mLookat-mPosition;
}

void	ICamera::SetPosition(NVECTOR3 vPos)
{
	//pos and lookat can't superpose each other
	NVECTOR3 dir = mLookat - vPos;
	if (D3DXVec3Length(&dir) < 0.0001f)return;

	mPosition=vPos;
	mFunction_UpdateRotation();
}

void	ICamera::SetPosition(float x,float y,float z)
{
	NVECTOR3 tmpPos(x,y,z);
	NVECTOR3 dir = mLookat - tmpPos;
	if (D3DXVec3Length(&dir) < 0.0001f)return;

	mPosition=tmpPos;
	mFunction_UpdateRotation();
};

NVECTOR3 ICamera::GetPosition()
{
	return mPosition;
};

void	ICamera::Move(NVECTOR3 vRelativePos)
{
	D3DXVec3Add(&mPosition,&mPosition,&vRelativePos);
	D3DXVec3Add(&mLookat, &mLookat, &vRelativePos);

};

void	ICamera::Move(float relativeX,float relativeY,float relativeZ)
{
	mPosition += NVECTOR3(relativeX, relativeY, relativeZ);
};

void	ICamera::SetRotation(float RX_Pitch,float RY_Yaw,float RZ_Roll)//要更新Lookat
{
	SetRotationX_Pitch(RX_Pitch);
	SetRotationY_Yaw(RY_Yaw);
	SetRotationZ_Roll(RZ_Roll);
	mFunction_UpdateDirection();
};

void	ICamera::SetRotationY_Yaw(float angleY)
{
	mRotateY_Yaw = angleY;
	mFunction_UpdateDirection();
};

void	ICamera::SetRotationX_Pitch(float AngleX)
{
	//clamp to [-pi/2,pi/2]
	mRotateX_Pitch = Clamp(AngleX, -MATH_PI / 2.0f, MATH_PI / 2.0f);

	mFunction_UpdateDirection();
};

void	ICamera::SetRotationZ_Roll(float AngleZ)
{
	//roll翻滚不需要更新lookat
	mRotateZ_Roll = AngleZ;
}

float ICamera::GetRotationY_Yaw()
{
	return mRotateY_Yaw;
}

float ICamera::GetRotationX_Pitch()
{
	return mRotateX_Pitch;
}

float ICamera::GetRotationZ_Roll()
{
	return mRotateZ_Roll;
}

void ICamera::RotateY_Yaw(float angleY)
{
	SetRotationY_Yaw(mRotateY_Yaw + angleY);
	mFunction_UpdateDirection();
};

void ICamera::RotateX_Pitch(float angleX)
{
	float newAngle = mRotateX_Pitch + angleX;
	SetRotationX_Pitch(newAngle);
	mFunction_UpdateDirection();
};

void ICamera::RotateZ_Roll(float angleZ)
{
	SetRotationZ_Roll(mRotateZ_Roll + angleZ);
	mFunction_UpdateDirection();
};

void ICamera::fps_MoveForward(float fSignedDistance, BOOL enableYAxisMovement)
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

void ICamera::fps_MoveRight(float fSignedDistance, BOOL enableYAxisMovement)
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

void ICamera::fps_MoveUp(float fSignedDistance)
{
	mPosition += NVECTOR3(0, fSignedDistance, 0);
	mLookat += NVECTOR3(0, fSignedDistance, 0);
}

void ICamera::GetViewMatrix(NMATRIX & outMat)
{
	mFunction_UpdateViewMatrix();
	outMat = mMatrixView;
}

void ICamera::GetProjMatrix(NMATRIX & outMat)
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

void Noise3D::ICamera::GetInvProjMatrix(NMATRIX & outMat)
{
	mFunction_UpdateProjMatrix();
	auto invPtr = D3DXMatrixInverse(&outMat, nullptr, &mMatrixProjection);
	if (invPtr == nullptr)ERROR_MSG("Camera : Inverse of Proj Matrix not exist!")
};

void	ICamera::SetViewFrustumPlane(float iNearPlaneZ,float iFarPlaneZ)
{
	if ( (iNearPlaneZ >0) && (iFarPlaneZ>iNearPlaneZ))
	{
		mNearPlane	= iNearPlaneZ;
		mFarPlane	=	iFarPlaneZ;
	}

};

void ICamera::SetViewAngle(float iViewAngleY,float iAspectRatio)
{
	if(iViewAngleY>0 && (mViewAngleY <(MATH_PI/2))){mViewAngleY	=	iViewAngleY;	}
	if(iAspectRatio>0){mAspectRatio	= iAspectRatio;}
};


/************************************************************************
											PRIVATE	
************************************************************************/

void	ICamera::mFunction_UpdateProjMatrix()
{
	D3DXMatrixPerspectiveFovLH(
		&mMatrixProjection,
		mViewAngleY,
		mAspectRatio,
		mNearPlane,
		mFarPlane);

	//要更新到GPU，TM居然要先转置
	D3DXMatrixTranspose(&mMatrixProjection,&mMatrixProjection);
};

void	ICamera::mFunction_UpdateViewMatrix()
{

	NMATRIX	tmpMatrixTranslation;
	NMATRIX	tmpMatrixRotation;
	//先对齐原点
	D3DXMatrixTranslation(&tmpMatrixTranslation, -mPosition.x, -mPosition.y, -mPosition.z);
	//然后用yawpitchroll的逆阵转到view空间
	D3DXMatrixRotationYawPitchRoll(&tmpMatrixRotation, mRotateY_Yaw, mRotateX_Pitch, mRotateZ_Roll);
	//正交矩阵的转置是逆
	D3DXMatrixTranspose(&tmpMatrixRotation,&tmpMatrixRotation);
	//先平移，再旋转
	D3DXMatrixMultiply(&mMatrixView,&tmpMatrixTranslation,&tmpMatrixRotation);
	//要更新到GPU，TM居然要先转置
	//(2016.4.11)楼上貌似有点不对啊，他妈的shader居然一直写的是矩阵右乘！！！！
	//一直是用行向量！！！但是奇怪的是
	D3DXMatrixTranspose(&mMatrixView,&mMatrixView);

};

void	ICamera::mFunction_UpdateRotation()
{
	//main function: cope with rotation change after the direction changes

	//update Direction
	D3DXVec3Subtract(&mDirection,&mLookat,&mPosition);

	//pitch角： tan = y/sqr(x^2+z^2))
	/*	注意：	atan ranged [-pi/2,pi/2]  
					atan2 ranged [-pi,pi] 		*/

	NVECTOR3 radiusDir(mDirection.x, 0, mDirection.z);
	//always positive
	float radiusLength = D3DXVec3Length(&radiusDir);
	//atan2(y,x) , radiusLength is constantly positive, pitch angle will range [-pi/2,pi/2] 
	//pitch : rotate downside is positive (the definition of rotation in left-handed frame)
	mRotateX_Pitch = atan2(-mDirection.y,radiusLength );

	//yaw角： tan = -x/z
	mRotateY_Yaw = atan2(mDirection.x,mDirection.z);//俯视图yaw是顺时针正角

	//roll角：更新direction不会改变roll角 
	//roll逆时针转是正角
};

void	ICamera::mFunction_UpdateDirection()
{
	//主要功能：这个函数主要是为了处理姿态角改变带来的视线Direction变化

	//要更新Lookat
	float tmpDirectionLength = D3DXVec3Length(&mDirection);
	//直接用三角函数表达Direction	3dscanner的世界变换一章里面有
	//!!!!!!!!!!!!!!!!!!!! (2016.7.21)负号会不会是-z，+x呢，看看射死大鸡怪的camera的这一段
	mDirection.x =- tmpDirectionLength* sin(mRotateY_Yaw)* cos(mRotateX_Pitch);
	mDirection.z =tmpDirectionLength* cos(mRotateY_Yaw)*cos(mRotateX_Pitch);
	mDirection.y =tmpDirectionLength* sin(mRotateX_Pitch);
	D3DXVec3Add(&mLookat,&mPosition,&mDirection);
};
