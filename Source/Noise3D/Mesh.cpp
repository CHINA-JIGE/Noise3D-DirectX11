
/***********************************************************************

									Mesh 
		An important scene object class. Derived from ISceneObject
		and its geometry data class 'GeometryEntity<>'

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::D3D;

Noise3D::NOISE_SCENE_OBJECT_TYPE Noise3D::Mesh::GetObjectType()
{
	return NOISE_SCENE_OBJECT_TYPE::MESH;
}

Mesh::Mesh()
{
	SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
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

N_AABB Noise3D::Mesh::ComputeWorldAABB_Accurate()
{
	//implementation is very similar to ISceneObject::GetLocalAABB()

	//get accumulated transform from scene graph (relative to root node)
	SceneNode* pNode = this->ISceneObject::GetParentNode();
	if (! pNode->IsAttachedToSceneNode())
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
	N_AABB outAabb;
	NVECTOR3 tmpV;
	for (uint32_t i = 0; i < mVB_Mem.size(); i++)
	{
		const AffineTransform& worldTrans = pNode->EvalWorldTransform();
		tmpV = worldTrans.TransformVector_Affine(mVB_Mem.at(i).Pos);

		if (tmpV.x < (outAabb.min.x)) { outAabb.min.x = tmpV.x; }
		if (tmpV.y < (outAabb.min.y)) { outAabb.min.y = tmpV.y; }
		if (tmpV.z < (outAabb.min.z)) { outAabb.min.z = tmpV.z; }

		if (tmpV.x >(outAabb.max.x)) { outAabb.max.x = tmpV.x; }
		if (tmpV.y >(outAabb.max.y)) { outAabb.max.y = tmpV.y; }
		if (tmpV.z >(outAabb.max.z)) { outAabb.max.z = tmpV.z; }
	}

	return outAabb;
}

/***********************************************************************
											PRIVATE					                    
***********************************************************************/
//this function could be externally invoked by ModelLoader..etc
