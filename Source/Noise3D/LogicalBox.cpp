
/*******************************************************

								cpp £ºLogical Box

********************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Noise3D::LogicalBox::LogicalBox():
	//mLocalBox(NVECTOR3(-1.0f,-1.0f,-1.0f),NVECTOR3(1.0f,1.0f,1.0f))
	mSize(1.0f,1.0f,1.0f)
{
}

Noise3D::LogicalBox::~LogicalBox()
{
}

NVECTOR3 Noise3D::LogicalBox::ComputeNormal(NOISE_BOX_FACET facet)
{
	switch (facet)
	{
	case NOISE_BOX_FACET::POS_X: return NVECTOR3(1.0f, 0, 0);
	case NOISE_BOX_FACET::NEG_X: return NVECTOR3(-1.0f, 0, 0);
	case NOISE_BOX_FACET::POS_Y: return NVECTOR3(0, 1.0f, 0);
	case NOISE_BOX_FACET::NEG_Y: return NVECTOR3(0, -1.0f, 0);
	case NOISE_BOX_FACET::POS_Z: return NVECTOR3(0, 0, 1.0f);
	case NOISE_BOX_FACET::NEG_Z: return NVECTOR3(0, 0, -1.0f);
	}
	return NVECTOR3(0, 0, 0);
}


void Noise3D::LogicalBox::SetSizeXYZ(NVECTOR3 size)
{
	mSize = NVECTOR3(abs(size.x), abs(size.y), abs(size.z));
}

N_AABB Noise3D::LogicalBox::GetLocalBox()
{
	//return mLocalBox;
	N_AABB a;
	a.min = NVECTOR3(-mSize.x / 2.0f, -mSize.y / 2.0f, -mSize.z / 2.0f);
	a.max = NVECTOR3(mSize.x / 2.0f, mSize.y / 2.0f, mSize.z / 2.0f);
	return a;
}

NOISE_SCENE_OBJECT_TYPE Noise3D::LogicalBox::GetObjectType() const
{
	return NOISE_SCENE_OBJECT_TYPE::LOGICAL_BOX;
}

N_AABB Noise3D::LogicalBox::GetLocalAABB()
{
	return LogicalBox::GetLocalBox();
}

N_AABB Noise3D::LogicalBox::ComputeWorldAABB_Accurate()
{
	//AABB of AABB (note that local box is also an AABB)
	return ISceneObject::ComputeWorldAABB_Fast();
}

float Noise3D::LogicalBox::ComputeArea()
{
	N_AABB aabb = GetLocalAABB();
	NVECTOR3 delta = aabb.max - aabb.min;
	return 2.0f*(delta.x * delta.y + delta.y * delta.z + delta.z * delta.x);
}
