/**********************************************************

						cpp :   Camera
				
**********************************************************/

#pragma once
#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::Ut;

Noise3D::Camera::Camera():
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

Noise3D::Camera::~Camera()
{
};

void Noise3D::Camera::LookAt(NVECTOR3 vLookat)
{
	mLookat = vLookat;
	mFunc_UpdateRotation();
}

void Noise3D::Camera::LookAt(float x, float y, float z)
{
	mLookat = NVECTOR3(x, y, z);
	mFunc_UpdateRotation();
}

void Noise3D::Camera::SetDirection(NVECTOR3 viewDir)
{
	if (viewDir != NVECTOR3(0, 0, 0))
	{
		mLookat =Camera::GetWorldTransform().GetPosition() + viewDir;
		mFunc_UpdateRotation();
	}
}

void Noise3D::Camera::SetDirection(float x, float y, float z)
{
	if (x != 0 && y != 0 && z != 0)
	{
		mLookat = Camera::GetWorldTransform().GetPosition() + NVECTOR3(x, y, z);
		mFunc_UpdateRotation();
	}
}

NVECTOR3 Noise3D::Camera::GetLookAtPos()
{
	mFunc_UpdateViewDir();
	return mLookat;
}

NVECTOR3 Noise3D::Camera::GetDirection()
{
	mFunc_UpdateViewDir();
	return mLookat - Camera::GetWorldTransform().GetPosition();
}

void Noise3D::Camera::SetProjectionType(bool isPerspective)
{
	mIsPerspective = isPerspective;
}

void Noise3D::Camera::GetViewMatrix(NMATRIX & outMat)
{
	//WARNING: Row-Major matrix
	NVECTOR3 pos = Camera::GetWorldTransform().GetPosition();

	//translation to align the camera pos to origin
	NMATRIX tmpTranslationMat = XMMatrixTranslation(-pos.x, -pos.y, -pos.z);

	//rotate the align the camera view dir to +z/-z axis
	NMATRIX tmpRotationMat = Camera::GetWorldTransform().GetRotationMatrix();

	////inverse
	tmpRotationMat = tmpRotationMat.Transpose();

	//combine 2 ROW-MAJOR matrix, translate first, rotate later
	outMat = XMMatrixMultiply(tmpTranslationMat, tmpRotationMat);
}

void Noise3D::Camera::GetProjMatrix(NMATRIX & outMat)
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

void Noise3D::Camera::GetViewInvMatrix(NMATRIX & outMat)
{
	NMATRIX viewMat;
	Camera::GetViewMatrix(viewMat);
	outMat = XMMatrixInverse(nullptr, viewMat);
	//if the inverse doesn't exist, then matrix will be set to infinite
	if (XMMatrixIsInfinite(outMat))ERROR_MSG("CameraTransform : Inverse of View Matrix not exist!");
}

void Noise3D::Camera::SetViewFrustumPlane(float fNearPlaneZ, float fFarPlaneZ)
{
	mNearPlane = fNearPlaneZ;
	mFarPlane = fFarPlaneZ;
}

void Noise3D::Camera::SetViewAngle_Degree(float fovY_Degree, float fAspectRatio)
{
	mViewAngleY_Radian = fovY_Degree * Ut::PI / 180.0f;
	mAspectRatio = fAspectRatio;
}

void Noise3D::Camera::SetViewAngle_Radian(float fovY_Radian, float fAspectRatio)
{
	mViewAngleY_Radian = fovY_Radian;
	mAspectRatio = fAspectRatio;
}

void Noise3D::Camera::SetOrthoViewSize(float width, float height)
{
	if (width > 0.0f)mOrthoViewWidth = width;
	if (height > 0.0f)mOrthoViewHeight = height;
}


void Noise3D::Camera::fps_MoveForward(float fSignedDistance, bool enableYAxisMovement)
{
	//...Yaw Angle Starts at Z axis ( left-handed system) 
	/*		Z
			|    / A
			|_ /   
		O	|/__________ X		

			angle AOZ is the yaw angle
	
	*/

	NVECTOR3 relativePos;
	NVECTOR3 euler = Camera::GetWorldTransform().GetEulerAngleZXY();
	//generate a direction first (later multiply it with fDist)
	if(enableYAxisMovement)
	{ 
		relativePos.x = sinf(euler.y) *cosf(euler.x);
		relativePos.z = cosf(euler.y) *cosf(euler.x);
		relativePos.y = sinf(euler.x);
	}
	else
	{
		relativePos.x = sinf(euler.y);
		relativePos.z = cosf(euler.y);
		relativePos.y = 0;
	}

	// length * unit_dir
	relativePos *= fSignedDistance;

	//relative movement
	Camera::GetWorldTransform().Move(relativePos);
	mLookat += relativePos;
}

void Noise3D::Camera::fps_MoveRight(float fSignedDistance, bool enableYAxisMovement)
{
	//...Yaw Angle Starts at Z axis ( left-handed system) 
	/*		
		Z
		|    / A
		|_ /
	O	|/__________ X

	angle AOZ is the yaw angle
	y-yaw, x-pitch, z-roll
	*/

		NVECTOR3 relativePos;
		NVECTOR3 euler = Camera::GetWorldTransform().GetEulerAngleZXY();
		//generate a direction first (later multiply it with fDist)
		if (enableYAxisMovement)
		{
			relativePos.x = cosf(euler.y) *cosf(euler.x);
			relativePos.z = -sinf(euler.y) *cosf(euler.x);
			relativePos.y = sinf(euler.x);
		}
		else
		{
			relativePos.x = cosf(euler.y);
			relativePos.z = -sinf(euler.y);
			relativePos.y = 0;
		}

		// length * unit_dir
		relativePos *= fSignedDistance;

		//relative movement
		Camera::GetWorldTransform().Move(relativePos);
		mLookat += relativePos;
}

void Noise3D::Camera::fps_MoveUp(float fSignedDistance)
{
	NVECTOR3 relativePos = NVECTOR3(0, fSignedDistance, 0);
	Camera::GetWorldTransform().Move(relativePos);
	mLookat += relativePos;
}

void Noise3D::Camera::OptimizeForQwertyPass1(const Mesh * pScreenDescriptor)
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
	Camera::LookAt(scrCenterPos);
	NVECTOR3 euler = Camera::GetWorldTransform().GetEulerAngleZXY();
	euler.z = 0;
	Camera::GetWorldTransform().SetRotation(euler);

	//2. Adjust projection matrix (fov & aspectRatio)
	float halfBoundingRectWidth=0.0f;
	float halfBoundingRectHeight = 0.0f;

	//traverse vertices of scr descriptor to calculate a bounding rectangle, then fov/aspectRatio
	for (auto v : vb)
	{
		NMATRIX viewMat;
		Camera::GetViewMatrix(viewMat);
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
		Camera::SetViewAngle_Radian(2.0f * atan(halfBoundingRectHeight / 1.0f), halfBoundingRectWidth / halfBoundingRectHeight);
	}
}

N_AABB Noise3D::Camera::GetLocalAABB()
{
	return N_AABB();
}

N_AABB Noise3D::Camera::ComputeWorldAABB_Accurate()
{
	return N_AABB();
}

NOISE_SCENE_OBJECT_TYPE Noise3D::Camera::GetObjectType()const
{
	return NOISE_SCENE_OBJECT_TYPE::CAMERA;
}

AffineTransform & Noise3D::Camera::GetWorldTransform()
{
	return ISceneObject::GetAttachedSceneNode()->GetLocalTransform();
}

/************************************************************************
											PRIVATE	
************************************************************************/

void Noise3D::Camera::mFunc_UpdateViewDir()
{
	//update lookat according to new posture
	NVECTOR3 pos = Camera::GetWorldTransform().GetPosition();
	NVECTOR3 euler = Camera::GetWorldTransform().GetEulerAngleZXY();
	float tmpDirectionLength = (mLookat - pos).Length();
	//z+ axis is the original posture.
	//mDirection.x =- tmpDirectionLength* sin(mRotateY_Yaw)* cos(mRotateX_Pitch);
	NVECTOR3 dir;
	dir.x = tmpDirectionLength* sin(euler.y)* cos(euler.x);
	dir.y = tmpDirectionLength* sin(euler.x);
	dir.z = tmpDirectionLength* cos(euler.y)*cos(euler.x);
	mLookat = pos + dir;
}

void Noise3D::Camera::mFunc_UpdateRotation()
{
	//main function: change Euler Angle after the direction changes

	//update Direction
	NVECTOR3 dir = mLookat - Camera::GetWorldTransform().GetPosition();

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
	NVECTOR3 euler = Camera::GetWorldTransform().GetEulerAngleZXY();
	Camera::GetWorldTransform().SetRotation(pitch_x, yaw_y, euler.z);
}
