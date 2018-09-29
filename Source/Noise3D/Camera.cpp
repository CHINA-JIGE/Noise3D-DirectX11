/************************************************************************

			cpp:   Camera(transform based on CameraTransform)
				
************************************************************************/

#pragma once
#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::Ut;

Noise3D::ICamera::ICamera()
{
}

Noise3D::ICamera::~ICamera()
{
};

void Noise3D::ICamera::Destroy()
{

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
	NVECTOR3 euler = CameraTransform::GetEulerAngle();
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
	y-yaw, x-pitch, z-roll
	*/

		NVECTOR3 relativePos;
		NVECTOR3 euler = CameraTransform::GetEulerAngle();
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
		Move(relativePos);
}

void Noise3D::ICamera::fps_MoveUp(float fSignedDistance)
{
	CameraTransform::Move(NVECTOR3(0, fSignedDistance, 0));
	CameraTransform::LookAt(CameraTransform::GetLookAtPos() + NVECTOR3(0, fSignedDistance, 0));
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
	ICamera::LookAt(scrCenterPos);
	NVECTOR3 euler = ICamera::CameraTransform::GetEulerAngle();
	euler.z = 0;
	ICamera::SetRotation(euler);

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

/************************************************************************
											PRIVATE	
************************************************************************/