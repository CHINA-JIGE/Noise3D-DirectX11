
/*******************************************************

								cpp £ºLogical Rect

********************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Noise3D::LogicalRect::LogicalRect():
	mOrientation(NOISE_RECT_ORIENTATION::RECT_XZ),
	mSize(1.0f,1.0f)
{
}

Noise3D::LogicalRect::~LogicalRect()
{
}

void Noise3D::LogicalRect::SetOrientation(NOISE_RECT_ORIENTATION ori)
{
	mOrientation = ori;
}

NOISE_RECT_ORIENTATION Noise3D::LogicalRect::GetOrientation() const
{
	return mOrientation;
}

void Noise3D::LogicalRect::SetSize(float width, float height)
{
	if (width != 0 && height != 0)
	{
		mSize.x = width;
		mSize.y = height;
	}
}

void Noise3D::LogicalRect::SetSize(Vec2 size)
{
	if (size.x != 0 && size.y != 0)
	{
		mSize = size;
	}
}

Vec2 Noise3D::LogicalRect::GetSize() const
{
	return mSize;
}

NOISE_SCENE_OBJECT_TYPE Noise3D::LogicalRect::GetObjectType() const
{
	return NOISE_SCENE_OBJECT_TYPE::LOGICAL_RECT;
}

N_AABB Noise3D::LogicalRect::GetLocalAABB()
{
	N_AABB res;
	Vec3 halfSize = mSize / 2.0f;
	const float epsilon = 1e-4f;//epsilon thickness
	switch (mOrientation)
	{
	case NOISE_RECT_ORIENTATION::RECT_XY:
		res.min = Vec3(-halfSize.x, -halfSize.y, -epsilon);
		res.max = Vec3(halfSize.x, halfSize.y, epsilon);
		break;

	case NOISE_RECT_ORIENTATION::RECT_XZ:
		res.min = Vec3(-halfSize.x, -epsilon,-halfSize.y);
		res.max = Vec3(halfSize.x, epsilon, halfSize.y);
		break;

	case NOISE_RECT_ORIENTATION::RECT_YZ:
		res.min = Vec3(-epsilon, -halfSize.x, -halfSize.y);
		res.max = Vec3(epsilon, halfSize.x, halfSize.y);
		break;
	}
	return res;
}

N_AABB Noise3D::LogicalRect::ComputeWorldAABB_Accurate()
{
	return ISceneObject::ComputeWorldAABB_Fast();
}


float Noise3D::LogicalRect::ComputeArea()
{
	return mSize.x * mSize.y;
}
