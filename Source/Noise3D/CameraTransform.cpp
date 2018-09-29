/************************************************************************

			cpp:   CameraTransform(based on RigidTransform)
				
************************************************************************/

#pragma once
#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::Ut;

Noise3D::CameraTransform::CameraTransform():
	mIsPerspective(true),
	mViewAngleY_Radian((float)60 / 180 * Ut::PI),
	mAspectRatio(1.5f),
	mLookat(1.0f, 0, 0),
	mNearPlane(1.0f),
	mFarPlane(1000.0f),
	mOrthoViewWidth(1.0f),
	mOrthoViewHeight(1.0f)
{
}

void Noise3D::CameraTransform::LookAt(NVECTOR3 vLookat)
{
	mLookat = vLookat;
	mFunc_UpdateRotation();
}

void Noise3D::CameraTransform::LookAt(float x, float y, float z)
{
	mLookat = NVECTOR3(x,y,z);
	mFunc_UpdateRotation();
}

void Noise3D::CameraTransform::SetDirection(NVECTOR3 viewDir)
{
	if (viewDir != NVECTOR3(0, 0, 0))
	{
		mLookat = RigidTransform::GetPosition() + viewDir;
		mFunc_UpdateRotation();
	}
}

void Noise3D::CameraTransform::SetDirection(float x, float y, float z)
{
	if (x!=0 && y!=0 && z!=0)
	{
		mLookat = RigidTransform::GetPosition() + NVECTOR3(x, y, z);
		mFunc_UpdateRotation();
	}
}

NVECTOR3 Noise3D::CameraTransform::GetLookAtPos()
{
	mFunc_UpdateViewDir();
	return mLookat;
}

NVECTOR3 Noise3D::CameraTransform::GetDirection()
{
	mFunc_UpdateViewDir();
	return mLookat- RigidTransform::GetPosition();
}

void Noise3D::CameraTransform::SetProjectionType(bool isPerspective)
{
	mIsPerspective = isPerspective;
}

void Noise3D::CameraTransform::GetViewMatrix(NMATRIX & outMat)
{
	//WARNING: Row-Major matrix
	NVECTOR3 pos = RigidTransform::GetPosition();

	//translation to align the camera pos to origin
	NMATRIX tmpTranslationMat = XMMatrixTranslation(-pos.x, -pos.y, -pos.z);
	
	//rotate the align the camera view dir to +z/-z axis
	NMATRIX tmpRotationMat=RigidTransform::GetRotationMatrix();

	////inverse
	tmpRotationMat = tmpRotationMat.Transpose();

	//combine 2 ROW-MAJOR matrix, translate first, rotate later
	outMat = XMMatrixMultiply(tmpTranslationMat, tmpRotationMat);
}

void Noise3D::CameraTransform::GetProjMatrix(NMATRIX & outMat)
{
	if (mIsPerspective)
	{
		outMat = XMMatrixPerspectiveFovLH(mViewAngleY_Radian, mAspectRatio, mNearPlane, mFarPlane);
	}
	else
	{
		outMat = XMMatrixOrthographicLH(mOrthoViewWidth, mOrthoViewHeight, mNearPlane, mFarPlane);
	}

}

void Noise3D::CameraTransform::GetInvViewMatrix(NMATRIX & outMat)
{
	NMATRIX viewMat;
	CameraTransform::GetViewMatrix(viewMat);
	outMat = XMMatrixInverse(nullptr, viewMat);
	//if the inverse doesn't exist, then matrix will be set to infinite
	if (XMMatrixIsInfinite(outMat))ERROR_MSG("CameraTransform : Inverse of View Matrix not exist!");
}

void Noise3D::CameraTransform::SetViewFrustumPlane(float fNearPlaneZ, float fFarPlaneZ)
{
	mNearPlane = fNearPlaneZ;
	mFarPlane = fFarPlaneZ;
}

void Noise3D::CameraTransform::SetViewAngle_Degree(float fovY_Degree, float fAspectRatio)
{
	mViewAngleY_Radian = fovY_Degree * Ut::PI / 180.0f;
	mAspectRatio = fAspectRatio;
}

void Noise3D::CameraTransform::SetViewAngle_Radian(float fovY_Radian, float fAspectRatio)
{
	mViewAngleY_Radian = fovY_Radian;
	mAspectRatio = fAspectRatio;
}

void Noise3D::CameraTransform::SetOrthoViewSize(float width, float height)
{
	if (width > 0.0f)mOrthoViewWidth = width;
	if (height > 0.0f)mOrthoViewHeight = height;
}

void Noise3D::CameraTransform::mFunc_UpdateViewDir()
{
	//update lookat according to new posture
	NVECTOR3 pos = RigidTransform::GetPosition();
	NVECTOR3 euler = RigidTransform::GetEulerAngle();
	float tmpDirectionLength = (mLookat - pos).Length();
	//z+ axis is the original posture.
	//mDirection.x =- tmpDirectionLength* sin(mRotateY_Yaw)* cos(mRotateX_Pitch);
	NVECTOR3 dir;
	dir.x = tmpDirectionLength* sin(euler.y)* cos(euler.x);
	dir.y = tmpDirectionLength* sin(euler.x);
	dir.z = tmpDirectionLength* cos(euler.y)*cos(euler.x);
	mLookat = pos + dir;
}

void Noise3D::CameraTransform::mFunc_UpdateRotation()
{
	//main function: change Euler Angle after the direction changes

	//update Direction
	NVECTOR3 dir = mLookat - RigidTransform::GetPosition();

	/*	NOTE£º
	atan ranged [-pi/2,pi/2]
	atan2 ranged [-pi,pi] 		
	*/

	NVECTOR3 xzProjDir(dir.x, 0, dir.z);
	//always positive
	float radiusLength = xzProjDir.Length();
	//atan2(y,x) , radiusLength is constantly positive, pitch angle will range [-pi/2,pi/2] 

	//pitch : rotate downside is positive (the definition of rotation in left-handed frame)
	float pitch_x = atan2(-dir.y, radiusLength);

	//yaw angle y£º tan = -x/z
	float yaw_y = atan2(dir.x, dir.z);

	//roll angle z: direction update doesn't change ROLL angle

	//apply to RigidTransform
	NVECTOR3 euler = RigidTransform::GetEulerAngle();
	RigidTransform::SetRotation(pitch_x, yaw_y, euler.z);
}
