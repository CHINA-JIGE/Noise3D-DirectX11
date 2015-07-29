/************************************************************************

							CPP:  	Noise Engine
				
1，关于YawPitchRoll的正角定义：（其实是左手系正角的定义）
			Yaw:俯视图顺时针转
			Pitch:y往负的方向转（低头）
			Roll:逆时针转


************************************************************************/

#pragma once
#include "Noise3D.h"

NoiseCamera::NoiseCamera()
{
	m_pFatherScene = NULL;
	mRotateX_Pitch=0;
	mRotateY_Yaw=0;
	mRotateZ_Roll=0;
	mViewAngleY = (float)60/180 * MATH_PI;
	mAspectRatio = 1.5;
	m_pPosition = new D3DXVECTOR3(0,0,0);
	m_pLookat = new D3DXVECTOR3(1,0,0);
	m_pDirection = new D3DXVECTOR3(1,0,0);
	mNearPlane=1;
	mFarPlane= 1000;
	mCanUpdateProjMatrix = TRUE;

	m_pMatrixProjection = new D3DXMATRIX();
	m_pMatrixView = new D3DXMATRIX();
	D3DXMatrixPerspectiveFovLH(m_pMatrixProjection,mViewAngleY,mAspectRatio,mNearPlane,mFarPlane);
	D3DXMatrixIdentity(m_pMatrixView);
};

NoiseCamera::~NoiseCamera()
{
	delete m_pMatrixView;
	delete m_pMatrixProjection;
	delete m_pDirection;
	delete m_pPosition;
	delete m_pLookat;
};


void	NoiseCamera::SetLookAt(NVECTOR3* vLookat)
{
	*m_pLookat=*vLookat;
	mFunction_UpdateRotation();
};

void	NoiseCamera::SetLookAt(float x,float y,float z)
{
	NVECTOR3 tmpLookat(x,y,z);
	*m_pLookat=tmpLookat;
	mFunction_UpdateRotation();
};

void	NoiseCamera::GetLookAt(NVECTOR3& out_vLookat)
{
	out_vLookat = *m_pLookat;
};

void	NoiseCamera::SetPosition(NVECTOR3* vPos)
{
	//lookat和位置不能重合啊
		*m_pPosition=*vPos;
		mFunction_UpdateRotation();
}

void	NoiseCamera::SetPosition(float x,float y,float z)
{
	NVECTOR3 tmpPos(x,y,z);
	*m_pPosition=tmpPos;
	mFunction_UpdateRotation();
};

void NoiseCamera::GetPosition(NVECTOR3& out_vPos)
{
	out_vPos=*m_pPosition;
};

void	NoiseCamera::Move(NVECTOR3* vRelativePos)
{
	D3DXVec3Add(m_pPosition,m_pPosition,vRelativePos);
	SetPosition(m_pPosition);
};

void	NoiseCamera::Move(float relativeX,float relativeY,float relativeZ)
{
	NVECTOR3 tmpRelativePos(relativeX,relativeY,relativeZ);
	D3DXVec3Add(m_pPosition,m_pPosition,&tmpRelativePos);
	SetPosition(m_pPosition);
};

void	NoiseCamera::SetRotation(float RX_Pitch,float RY_Yaw,float RZ_Roll)//要更新Lookat
{
	mRotateX_Pitch = RX_Pitch;
	mRotateY_Yaw = RY_Yaw;
	mRotateZ_Roll = RZ_Roll;
	mFunction_UpdateDirection();
};

void	NoiseCamera::SetRotationY_Yaw(float angleY)
{
	mRotateY_Yaw = angleY;
	mFunction_UpdateDirection();
};

void	NoiseCamera::SetRotationX_Pitch(float AngleX)
{
		mRotateX_Pitch = AngleX;
		mFunction_UpdateDirection();
};

void	NoiseCamera::SetRotationZ_Roll(float AngleZ)
{
	//roll翻滚不需要更新lookat
	mRotateZ_Roll = AngleZ;
};

void	NoiseCamera::SetViewFrustumPlane(float iNearPlaneZ,float iFarPlaneZ)
{
	if ( (iNearPlaneZ >0) && (iFarPlaneZ>iNearPlaneZ))
	{
		mNearPlane	= iNearPlaneZ;
		mFarPlane	=	iFarPlaneZ;
		mCanUpdateProjMatrix =TRUE;
	}

};

void NoiseCamera::SetViewAngle(float iViewAngleY,float iAspectRatio)
{
	if(iViewAngleY>0 && (mViewAngleY <(MATH_PI/2))){mViewAngleY	=	iViewAngleY;	}
	if(iAspectRatio>0){mAspectRatio	= iAspectRatio;}
	mCanUpdateProjMatrix =TRUE;
};


/************************************************************************
											PRIVATE	
************************************************************************/

void	NoiseCamera::mFunction_UpdateProjMatrix()
{
	D3DXMatrixPerspectiveFovLH(
		m_pMatrixProjection,
		mViewAngleY,
		mAspectRatio,
		mNearPlane,
		mFarPlane);
	mCanUpdateProjMatrix = FALSE;
	//要更新到GPU，TM居然要先转置
	D3DXMatrixTranspose(m_pMatrixProjection,m_pMatrixProjection);
};

void	NoiseCamera::mFunction_UpdateViewMatrix()
{

	D3DXMATRIX	tmpMatrixTranslation;
	D3DXMATRIX	tmpMatrixRotation;
	//先对齐原点
	D3DXMatrixTranslation(&tmpMatrixTranslation, -m_pPosition->x, -m_pPosition->y, -m_pPosition->z);
	//然后用yawpitchroll的逆阵转到view空间
	D3DXMatrixRotationYawPitchRoll(&tmpMatrixRotation, mRotateY_Yaw, mRotateX_Pitch, mRotateZ_Roll);
	//正交矩阵的转置是逆
	D3DXMatrixTranspose(&tmpMatrixRotation,&tmpMatrixRotation);
	//先平移，再旋转
	D3DXMatrixMultiply(m_pMatrixView,&tmpMatrixTranslation,&tmpMatrixRotation);
	//要更新到GPU，TM居然要先转置
	D3DXMatrixTranspose(m_pMatrixView,m_pMatrixView);

};

void	NoiseCamera::mFunction_UpdateRotation()
{
	//主要功能：这个函数主要是为了处理Direction改变带来的姿态角变化

	//更新direction
	D3DXVECTOR3	tmpDirection;
	//检查新direction是否为0
	D3DXVec3Subtract(&tmpDirection,m_pLookat,m_pPosition);
	float mLength = D3DXVec3Length(&tmpDirection);
	//注意浮点数误差，视点和位置不能重合
	if (mLength<0.001)
	{
		//重置摄像机啊 尼玛 这样都set得出来不如去死啊
		mRotateX_Pitch = 0;
		mRotateY_Yaw = 0;
		mRotateZ_Roll = 0;
		*m_pDirection = NVECTOR3(1.0f, 0, 0);
		*m_pLookat = *m_pPosition + *m_pDirection;
		return;
	}
	else
	//视点和位置不重合 ，再赋值
	{ *m_pDirection = tmpDirection; }
	;

	//临时变量，储存的比值用来arctan获得Pitch角度
	float tmpRatio;
	//pitch角： tan = y/sqr(x^2+z^2))
	/*	注意：	atan取值范围是 [-pi/2,pi/2]  
					atan2取值范围是 [-pi,pi] 		*/
	if((m_pDirection->x==0) && (m_pDirection->z==0))
	{
		//是否在原点的正上下方
		if(m_pDirection->y>=0)
		{mRotateX_Pitch=-MATH_PI/2;}
		else
		{mRotateX_Pitch=MATH_PI/2;}
	}
	else
	{
		//去你的 难道y负的时候是正角————对的没错，DX用左手系，左手系旋转正方向用左手决定
		//用大拇指指着旋转轴正向，四指指向正旋转方向
		tmpRatio =-m_pDirection->y /  sqrt(pow(m_pDirection->x,2.0f)+pow(m_pDirection->z,2.0f));
		mRotateX_Pitch = atan(tmpRatio);//返回[-0.5pi,0.5pi]
	}

	//yaw角： tan = -x/z
		mRotateY_Yaw = atan2(m_pDirection->x,m_pDirection->z);//俯视图yaw是顺时针正角

	//roll角：更新direction不会改变roll角 
	//roll逆时针转是正角
};

void	NoiseCamera::mFunction_UpdateDirection()
{
	//主要功能：这个函数主要是为了处理姿态角改变带来的视线Direction变化

	//要更新Lookat
	float tmpDirectionLength = D3DXVec3Length(m_pDirection);
	//直接用三角函数表达Direction	3dscanner的世界变换一章里面有
	m_pDirection->x =- tmpDirectionLength* sin(mRotateY_Yaw)* cos(mRotateX_Pitch);
	m_pDirection->z =tmpDirectionLength* cos(mRotateY_Yaw)*cos(mRotateX_Pitch);
	m_pDirection->y =tmpDirectionLength* sin(mRotateX_Pitch);
	D3DXVec3Add(m_pLookat,m_pPosition,m_pDirection);
};
