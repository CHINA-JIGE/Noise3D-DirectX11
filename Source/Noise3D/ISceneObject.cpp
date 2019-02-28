/***********************************************************

								ISceneObject

			interface that will be inherited by some 'Object' 
			like mesh/actor/particles/light blahblahblah.
			ISceneObject can be bound to SceneNode to enable
			hierarchical transformation (and add to scene graph)

***********************************************************/
#include "Noise3D.h"

using namespace Noise3D;



Noise3D::ISceneObject::ISceneObject()
	:m_pParentSceneNode(nullptr)
{
}

Noise3D::ISceneObject::~ISceneObject()
{
}

Noise3D::SceneNode*  Noise3D::ISceneObject::GetParentSceneNode()
{
	return m_pParentSceneNode;
}

void Noise3D::ISceneObject::AttachToSceneNode(Noise3D::SceneNode * pNode)
{
	if (pNode != nullptr)
	{
		m_pParentSceneNode = pNode;
		pNode->AttachSceneObject(this);
	}
}

bool Noise3D::ISceneObject::IsAttachedToSceneNode()
{
	return (m_pParentSceneNode!=nullptr);
}

//bounding box of transformed bounding box
N_AABB Noise3D::ISceneObject::ComputeWorldAABB_Fast()
{
	if (m_pParentSceneNode == nullptr)
	{
		ERROR_MSG("ISceneObject: not bound to a scene node. Can't compute world space AABB.");
		return N_AABB();
	}

	//local aabb
	N_AABB localAabb =this->GetLocalAABB();
	const NVECTOR3& a = localAabb.min;
	const NVECTOR3& b = localAabb.max;

	//world transform info
	const AffineTransform& trans = m_pParentSceneNode->GetTransform();

	//get 8 vertices coord of local AABB
	NVECTOR3 vertices[8] = 
	{
		{a.x, a.y, a.z},
		{ b.x, a.y, a.z },
		{ a.x, b.y, a.z },
		{ a.x, a.y, b.z },
		{ b.x, b.y, a.z },
		{ b.x, a.y, b.z },
		{ a.x, b.y, b.z },
		{ b.x, b.y, b.z }
	};

	for (uint32_t i = 0; i < 8; ++i)
	{
		//apply affine transform of scene node to 8 vertices of local AABB
		vertices[i] = trans.TransformVector_Affine(vertices[i]);
	}

	//find the AABB of these 8 transformed vertices
	NVECTOR3 tmpV;
	N_AABB outAabb;

	for (uint32_t i = 0; i < 8; i++)
	{
		tmpV =vertices[i];
		if (tmpV.x < (outAabb.min.x)) { outAabb.min.x = tmpV.x; }
		if (tmpV.y < (outAabb.min.y)) { outAabb.min.y = tmpV.y; }
		if (tmpV.z < (outAabb.min.z)) { outAabb.min.z = tmpV.z; }

		if (tmpV.x >(outAabb.max.x)) { outAabb.max.x = tmpV.x; }
		if (tmpV.y >(outAabb.max.y)) { outAabb.max.y = tmpV.y; }
		if (tmpV.z >(outAabb.max.z)) { outAabb.max.z = tmpV.z; }
	}

	return outAabb;
}

std::string Noise3D::ISceneObject::GetName()
{
	return mUid;
}
