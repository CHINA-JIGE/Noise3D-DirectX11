
/***********************************************************************

							class£ºMeshManager

				description: manager class to manage Mesh

***********************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

Mesh * MeshManager::CreateMesh(N_UID meshName)
{
	Mesh* pMesh = IFactory<Mesh>::CreateObject(meshName);
	pMesh->ISceneObject::mUid = meshName;
	return pMesh;
}

Mesh * MeshManager::GetMesh(N_UID meshName)
{
	return IFactory<Mesh>::GetObjectPtr(meshName);
}

Mesh * MeshManager::GetMesh(UINT index)
{
	return IFactory<Mesh>::GetObjectPtr(index);
}

bool MeshManager::DestroyMesh(N_UID meshName)
{
	return IFactory<Mesh>::DestroyObject(meshName);
}

bool MeshManager::DestroyMesh(Mesh* ppMesh)
{
	return IFactory<Mesh>::DestroyObject(ppMesh);
}

void MeshManager::DestroyAllMesh()
{
	IFactory<Mesh>::DestroyAllObject();
}

UINT MeshManager::GetMeshCount()
{
	return IFactory<Mesh>::GetObjectCount();
}

bool Noise3D::MeshManager::IsMeshExisted(N_UID meshName)
{
	return IFactory<Mesh>::FindUid(meshName);
}

/***********************************************************************
								P R I V A T E					                    
***********************************************************************/


MeshManager::MeshManager():IFactory<Mesh>(50000)
{

}

MeshManager::~MeshManager()
{
	IFactory<Mesh>::DestroyAllObject();
}
