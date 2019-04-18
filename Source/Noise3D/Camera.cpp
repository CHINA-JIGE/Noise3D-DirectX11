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

void Noise3D::Camera::LookAt(Vec3 vLookat)
{
	mLookat = vLookat;
	mFunc_UpdateRotation();
}

void Noise3D::Camera::LookAt(float x, float y, float z)
{
	mLookat = Vec3(x, y, z);
	mFunc_UpdateRotation();
}

void Noise3D::Camera::SetDirection(Vec3 viewDir)
{
	if (viewDir != Vec3(0, 0, 0))
	{
		mLookat =Camera::GetWorldTransform().GetPosition() + viewDir;
		mFunc_UpdateRotation();
	}
}

void Noise3D::Camera::SetDirection(float x, float y, float z)
{
	if (x != 0 && y != 0 && z != 0)
	{
		mLookat = Camera::GetWorldTransform().GetPosition() + Vec3(x, y, z);
		mFunc_UpdateRotation();
	}
}

Vec3 Noise3D::Camera::GetLookAtPos()
{
	mFunc_UpdateViewDir();
	return mLookat;
}

Vec3 Noise3D::Camera::GetDirection()
{
	mFunc_UpdateViewDir();
	return mLookat - Camera::GetWorldTransform().GetPosition();
}

void Noise3D::Camera::SetProjectionType(bool isPerspective)
{
	mIsPerspective = isPerspective;
}

void Noise3D::Camera::GetViewMatrix(Matrix & outMat)
{
	//view  matrix: coordinate transform from WORLD to VIEW
	//WARNING: Row-Major matrix
	Vec3 pos = Camera::GetWorldTransform().GetPosition();

	//translation to align the camera pos to origin
	Matrix tmpTranslationMat = XMMatrixTranslation(-pos.x, -pos.y, -pos.z);

	//rotate the align the camera view dir to +z/-z axis
	Matrix tmpRotationMat = Camera::GetWorldTransform().GetRotationMatrix();

	//transpose == inverse (orthonormal matrix)
	tmpRotationMat = tmpRotationMat.Transpose();

	//combine 2 ROW-MAJOR matrix, translate first, rotate later
	outMat = XMMatrixMultiply(tmpTranslationMat, tmpRotationMat);
}

void Noise3D::Camera::GetProjMatrix(Matrix & outMat)
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

void Noise3D::Camera::GetViewInvMatrix(Matrix & outMat)
{
	//(2019.3.30)view matrix is coordinate transform from WORLD to VIEW
	//thus viewInv matrix is equivalent to the coordinate transform from VIEW to WORLD
	RigidTransform t = Camera::GetWorldTransform();
	outMat = t.GetRigidTransformMatrix();

	/*Camera::GetViewMatrix(viewMat);
	outMat = XMMatrixInverse(nullptr, viewMat);
	//if the inverse doesn't exist, then matrix will be set to infinite
	if (XMMatrixIsInfinite(outMat))ERROR_MSG("CameraTransform : Inverse of View Matrix not exist!");*/
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

Vec3 Noise3D::Camera::FireRay_ViewSpace(PixelCoord2 pixelCoord, size_t backBuffPxWidth, size_t backBuffPxHeight)
{
	float normalizedX = pixelCoord.x / float(backBuffPxWidth);// [0,1]  -> x
	float normalizedY = pixelCoord.y / float(backBuffPxHeight);// [1,0] ^ y
	float u = 2.0f*normalizedX - 1.0f;//left to right: [-1,1]
	float v = -2.0f*normalizedY + 1.0f;//bottom to up: [-1,1]
	//assume that z = 1.0f
	float tanFovY = tanf(mViewAngleY_Radian/2.0f); //(y/z)
	float tanFovX = tanFovY * mAspectRatio; //(x/z) = (y/z) * (x/y)
	Vec3 rayDir = { u * tanFovX, v * tanFovY, 1.0f };
	return rayDir;
}

Vec3 Noise3D::Camera::FireRay_ViewSpace(Vec2 uv)
{
	//assume that z = 1.0f
	float tanFovY = (mViewAngleY_Radian/2.0f); //(y/z)
	float tanFovX = tanFovY * mAspectRatio; //(x/z) = (y/z) * (x/y)
	Vec3 rayDir = { uv.x *tanFovX, uv.y *tanFovY, 1.0f };
	return rayDir;
}

N_Ray Noise3D::Camera::FireRay_WorldSpace(PixelCoord2 pixelCoord, size_t backBuffPxWidth, size_t backBuffPxHeight)
{
	//rayEnd = (0,0,0) + rayDir
	Vec3 rayEndV = Camera::FireRay_ViewSpace(pixelCoord, backBuffPxWidth, backBuffPxHeight);
	RigidTransform& t = Camera::GetWorldTransform();
	Vec3 rayStartW = t.GetPosition();
	Vec3 rayEndW = t.TransformVector_Rigid(rayEndV);
	//Vec3 rayEndW = AffineTransform::TransformVector_MatrixMul(rayEndV, t.GetRigidTransformMatrix());
	return N_Ray(rayStartW, rayEndW - rayStartW);
}

N_Ray Noise3D::Camera::FireRay_WorldSpace(Vec2 uv)
{
	//rayEnd = (0,0,0) + rayDir
	Vec3 rayEndV = Camera::FireRay_ViewSpace(uv);
	RigidTransform t = Camera::GetWorldTransform();
	Vec3 rayStartW = t.GetPosition();
	Vec3 rayEndW = t.TransformVector_Rigid(rayEndV);
	return N_Ray(rayStartW, rayEndW- rayStartW);
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

	Vec3 relativePos;
	Vec3 euler = Camera::GetWorldTransform().GetEulerAngleZXY();
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

		Vec3 relativePos;
		Vec3 euler = Camera::GetWorldTransform().GetEulerAngleZXY();
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
	Vec3 relativePos = Vec3(0, fSignedDistance, 0);
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
	Vec3 scrCenterPos = { 0,0,0 };
	for (auto v : vb)
	{
		scrCenterPos += v.Pos;
	}
	scrCenterPos /= float(vb.size());
	Camera::LookAt(scrCenterPos);
	Vec3 euler = Camera::GetWorldTransform().GetEulerAngleZXY();
	euler.z = 0;
	Camera::GetWorldTransform().SetRotation(euler);

	//2. Adjust projection matrix (fov & aspectRatio)
	float halfBoundingRectWidth=0.0f;
	float halfBoundingRectHeight = 0.0f;

	//traverse vertices of scr descriptor to calculate a bounding rectangle, then fov/aspectRatio
	for (auto v : vb)
	{
		Matrix viewMat;
		Camera::GetViewMatrix(viewMat);
		Vec3 posV = {
			v.Pos.x *  viewMat.m[0][0] + v.Pos.y *  viewMat.m[1][0] + v.Pos.z *  viewMat.m[2][0] + 1.0f * viewMat.m[3][0],
			v.Pos.x *  viewMat.m[0][1] + v.Pos.y *  viewMat.m[1][1] + v.Pos.z *  viewMat.m[2][1] + 1.0f * viewMat.m[3][1],
			v.Pos.x *  viewMat.m[0][2] + v.Pos.y *  viewMat.m[1][2] + v.Pos.z *  viewMat.m[2][2] + 1.0f * viewMat.m[3][2]
		};

		//project to plane z=1
		Vec2 posH = { posV.x / posV.z,posV.y / posV.z };
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

N_BoundingSphere Noise3D::Camera::ComputeWorldBoundingSphere_Accurate()
{
	return N_BoundingSphere();
}

NOISE_SCENE_OBJECT_TYPE Noise3D::Camera::GetObjectType()const
{
	return NOISE_SCENE_OBJECT_TYPE::CAMERA;
}

RigidTransform & Noise3D::Camera::GetWorldTransform()
{
	SceneNode* pNode = ISceneObject::GetAttachedSceneNode();
	if (pNode == nullptr)ERROR_MSG("Camera: not attached to scene node.");
	return pNode->GetLocalTransform();
}

/************************************************************************
											PRIVATE	
************************************************************************/

void Noise3D::Camera::mFunc_UpdateViewDir()
{
	//update lookat according to new posture
	Vec3 pos = Camera::GetWorldTransform().GetPosition();
	Vec3 euler = Camera::GetWorldTransform().GetEulerAngleZXY();
	float tmpDirectionLength = (mLookat - pos).Length();
	//z+ axis is the original posture.
	//mDirection.x =- tmpDirectionLength* sin(mRotateY_Yaw)* cos(mRotateX_Pitch);
	Vec3 dir;
	dir.x = tmpDirectionLength* sin(euler.y)* cos(euler.x);
	dir.y = tmpDirectionLength* sin(euler.x);
	dir.z = tmpDirectionLength* cos(euler.y)*cos(euler.x);
	mLookat = pos + dir;
}

void Noise3D::Camera::mFunc_UpdateRotation()
{
	//main function: change Euler Angle after the direction changes

	//update Direction
	Vec3 dir = mLookat - Camera::GetWorldTransform().GetPosition();

	/*	NOTE£º
	atan ranged [-pi/2,pi/2]
	atan2 ranged [-pi,pi]
	*/

	Vec3 xzProjDir(dir.x, 0, dir.z);
	//always positive
	float radiusLength = xzProjDir.Length();
	//atan2(y,x) , radiusLength is constantly positive, pitch angle will range [-pi/2,pi/2] 

	//pitch : rotate downside is positive (the definition of rotation in left-handed frame)
	float pitch_x = atan2(-dir.y, radiusLength);

	//yaw angle y£º tan = -x/z
	float yaw_y = atan2(dir.x, dir.z);

	//roll angle z: direction update doesn't change ROLL angle

	//apply to RigidTransform
	Vec3 euler = Camera::GetWorldTransform().GetEulerAngleZXY();
	Camera::GetWorldTransform().SetRotation(pitch_x, yaw_y, euler.z);
}
