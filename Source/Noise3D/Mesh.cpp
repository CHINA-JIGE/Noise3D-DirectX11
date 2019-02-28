
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

/***********************************************************************
											PRIVATE					                    
***********************************************************************/
//this function could be externally invoked by ModelLoader..etc
