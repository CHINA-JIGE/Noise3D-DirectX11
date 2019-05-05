
/***********************************************************************

												Mesh 
		An important scene object class. Derived from ISceneObject
		and its geometry data class 'GeometryEntity<>'

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::D3D;

Noise3D::NOISE_SCENE_OBJECT_TYPE Noise3D::Mesh::GetObjectType()const
{
	return NOISE_SCENE_OBJECT_TYPE::MESH;
}

Mesh::Mesh():
	mIsBvhTreeBuilt(false)
{
	Mesh::SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
};

Mesh::~Mesh()
{
}

void Mesh::ResetMaterialToDefault()
{
	SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
}

void Mesh::SetMaterial(N_UID matName)
{
	N_MeshSubsetInfo tmpSubset;
	tmpSubset.startPrimitiveID = 0;
	tmpSubset.primitiveCount = GeometryEntity::GetTriangleCount() ;//count of triangles
	tmpSubset.matName = matName;
	
	//because this SetMaterial aim to the entire mesh (all primitives) ,so
	//previously-defined material will be wiped,and set to this material
	mSubsetInfoList.clear();
	mSubsetInfoList.push_back(tmpSubset);
}

void Mesh::SetSubsetList(const std::vector<N_MeshSubsetInfo>& subsetList)
{
	mSubsetInfoList = subsetList;
}

void Mesh::GetSubsetList(std::vector<N_MeshSubsetInfo>& outRefSubsetList)
{
	outRefSubsetList = mSubsetInfoList;
}

void Noise3D::Mesh::SetPbrtMaterialSubset(const std::vector<N_MeshPbrtSubsetInfo>& subsetList)
{
	mPbrtMatSubsetInfoList = subsetList;
}

GI::PbrtMaterial * Noise3D::Mesh::GetPbrtMaterial(int triangleId)
{
	for (auto& s : mPbrtMatSubsetInfoList)
	{
		if (triangleId >= s.startPrimitiveID && triangleId < (s.startPrimitiveID + s.primitiveCount))
		{
			return s.pMat;
		}
	}
	return nullptr;
}

void Noise3D::Mesh::SetPbrtMaterial(GI::PbrtMaterial * pMat)
{
	N_MeshPbrtSubsetInfo tmpSubset;
	tmpSubset.startPrimitiveID = 0;
	tmpSubset.primitiveCount = GeometryEntity::GetTriangleCount();//count of triangles
	tmpSubset.pMat = pMat;

	//because this SetMaterial aim to the entire mesh (all primitives) ,so
	//previously-defined material will be wiped,and set to this material
	mPbrtMatSubsetInfoList.clear();
	mPbrtMatSubsetInfoList.push_back(tmpSubset);
}

GI::PbrtMaterial * Noise3D::Mesh::GetPbrtMaterial()
{
	if (mPbrtMatSubsetInfoList.empty())return nullptr;
	return mPbrtMatSubsetInfoList.front().pMat;
}

N_AABB Noise3D::Mesh::ComputeWorldAABB_Accurate()
{
	//implementation is very similar to ISceneObject::GetLocalAABB()

	//get accumulated transform from scene graph (relative to root node)
	SceneNode* pNode = ISceneObject::GetAttachedSceneNode();
	if (pNode==nullptr)
	{
		ERROR_MSG("ISceneObject: not bound to a scene node. Can't compute world space AABB.");
		return N_AABB();
	}

	//reset to infinite far
	if (mVB_Mem.size() == 0)
	{
		return  N_AABB();//min/max are initialized infinite far
	}

	//min / max are initialized infinite far
	const Matrix& worldMat = pNode->EvalWorldTransform().GetAffineTransformMatrix();
	N_AABB outAabb;
	Vec3 tmpV;
	for (uint32_t i = 0; i < mVB_Mem.size(); i++)
	{
		tmpV = AffineTransform::TransformVector_MatrixMul(mVB_Mem.at(i).Pos, worldMat);

		if (tmpV.x < (outAabb.min.x)) { outAabb.min.x = tmpV.x; }
		if (tmpV.y < (outAabb.min.y)) { outAabb.min.y = tmpV.y; }
		if (tmpV.z < (outAabb.min.z)) { outAabb.min.z = tmpV.z; }

		if (tmpV.x >(outAabb.max.x)) { outAabb.max.x = tmpV.x; }
		if (tmpV.y >(outAabb.max.y)) { outAabb.max.y = tmpV.y; }
		if (tmpV.z >(outAabb.max.z)) { outAabb.max.z = tmpV.z; }
	}

	return outAabb;
}

N_BoundingSphere Noise3D::Mesh::ComputeWorldBoundingSphere_Accurate()
{
	//get accumulated transform from scene graph (relative to root node)
	SceneNode* pNode = ISceneObject::GetAttachedSceneNode();
	if (pNode == nullptr)
	{
		ERROR_MSG("ISceneObject: not bound to a scene node. Can't compute world space Bounding Sphere.");
		return N_BoundingSphere();
	}

	if (mVB_Mem.size() == 0)
	{
		return  N_BoundingSphere();//radius is initialized to 0
	}

	AffineTransform t = pNode->EvalWorldTransform();
	N_BoundingSphere outSphere;
	for (uint32_t i = 0; i < mVB_Mem.size(); i++)
	{
		Vec3 transformedVecPos = t.TransformVector_Affine(mVB_Mem.at(i).Pos);
		float currentDist = transformedVecPos.Length();
		if (currentDist > outSphere.radius)outSphere.radius = currentDist;

		//accumulate average pos
		//e.g. : (a+b+c+d)/4 = (a+b+c)/3 * (3/4) + d /4
		float one_over_i_plus_one = 1.0f / float(i + 1);
		outSphere.pos = outSphere.pos * (float(i)*one_over_i_plus_one) + transformedVecPos * one_over_i_plus_one;
	}

	return outSphere;
}

bool Noise3D::Mesh::IsBvhTreeBuilt()
{
	return mIsBvhTreeBuilt;
}

void Noise3D::Mesh::RebuildBvhTree()
{
	mBvhTreeLocalSpace.Construct(this);
	mIsBvhTreeBuilt = true;
}

BvhTreeForTriangularMesh & Noise3D::Mesh::GetBvhTree()
{
	return mBvhTreeLocalSpace;
}

/***********************************************************************
											PRIVATE					                    
***********************************************************************/
//this function could be externally invoked by MeshLoader..etc
