
/*******************************************************

								cpp £ºLogical Box

********************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Noise3D::LogicalBox::LogicalBox():
	//mLocalBox(Vec3(-1.0f,-1.0f,-1.0f),Vec3(1.0f,1.0f,1.0f))
	mSize(1.0f,1.0f,1.0f)
{
}

Noise3D::LogicalBox::~LogicalBox()
{
}

Vec3 Noise3D::LogicalBox::ComputeNormal(NOISE_BOX_FACET facet)
{
	switch (facet)
	{
	case NOISE_BOX_FACET::POS_X: return Vec3(1.0f, 0, 0);
	case NOISE_BOX_FACET::NEG_X: return Vec3(-1.0f, 0, 0);
	case NOISE_BOX_FACET::POS_Y: return Vec3(0, 1.0f, 0);
	case NOISE_BOX_FACET::NEG_Y: return Vec3(0, -1.0f, 0);
	case NOISE_BOX_FACET::POS_Z: return Vec3(0, 0, 1.0f);
	case NOISE_BOX_FACET::NEG_Z: return Vec3(0, 0, -1.0f);
	default:
		WARNING_MSG("logical box: compute normal: facet param invalid.")
	}
	return Vec3(0, 0, 0);
}

Vec2 Noise3D::LogicalBox::ComputeUV(const N_AABB & aabb, NOISE_BOX_FACET facet, Vec3 pos)
{
	Vec3 extent = aabb.max - aabb.min;
	float t_x = (pos.x - aabb.min.x) / extent.x;
	float t_y = (pos.y - aabb.min.y) / extent.y;
	float t_z = (pos.z - aabb.min.z) / extent.z;
	t_x = std::modff(t_x, nullptr);//get its fractional part
	t_y = std::modff(t_y, nullptr);
	t_z = std::modff(t_z, nullptr);
	switch (facet)
	{
	case NOISE_BOX_FACET::POS_X: return Vec2(t_y, t_z);
	case NOISE_BOX_FACET::NEG_X: return Vec2(1.0f - t_y, 1.0f - t_z);
	case NOISE_BOX_FACET::POS_Y: return Vec2(t_x, t_z);
	case NOISE_BOX_FACET::NEG_Y: return Vec2(1.0f - t_x, 1.0f - t_z);
	case NOISE_BOX_FACET::POS_Z: return Vec2(t_x, t_y);
	case NOISE_BOX_FACET::NEG_Z: return Vec2(1.0f - t_x, 1.0f - t_y);
	default:
		WARNING_MSG("logical box: compute uv: facet param invalid.")
	}
	return Vec2(0, 0);
}

Vec2 Noise3D::LogicalBox::ComputeUV(NOISE_BOX_FACET facet, Vec3 pos)
{
	N_AABB aabb = LogicalBox::GetLocalBox();
	return LogicalBox::ComputeUV(aabb, facet, pos);
}


void Noise3D::LogicalBox::SetSizeXYZ(Vec3 size)
{
	mSize = Vec3(abs(size.x), abs(size.y), abs(size.z));
}

inline N_AABB Noise3D::LogicalBox::GetLocalBox()const
{
	//return mLocalBox;
	N_AABB a;
	a.min = -mSize / 2.0f;
	a.max = mSize / 2.0f;
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

N_BoundingSphere Noise3D::LogicalBox::ComputeWorldBoundingSphere_Accurate()
{
	N_AABB aabb= LogicalBox::ComputeWorldAABB_Accurate();
	N_BoundingSphere sphere;
	sphere.pos = aabb.Centroid();
	sphere.radius = (aabb.max - aabb.Centroid()).Length();
	return sphere;
}

float Noise3D::LogicalBox::ComputeArea()
{
	N_AABB aabb = GetLocalAABB();
	Vec3 delta = aabb.max - aabb.min;
	return 2.0f*(delta.x * delta.y + delta.y * delta.z + delta.z * delta.x);
}
