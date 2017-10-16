
/***********************************************************************

							class£ºMeshManager

				description: manager class to manage IMesh

***********************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

IMesh * IMeshManager::CreateMesh(N_UID meshName)
{
	return IFactory<IMesh>::CreateObject(meshName);
}

IMesh * IMeshManager::GetMesh(N_UID meshName)
{
	return IFactory<IMesh>::GetObjectPtr(meshName);
}

IMesh * IMeshManager::GetMesh(UINT index)
{
	return IFactory<IMesh>::GetObjectPtr(index);
}

bool IMeshManager::DestroyMesh(N_UID meshName)
{
	return IFactory<IMesh>::DestroyObject(meshName);
}

bool IMeshManager::DestroyMesh(IMesh* ppMesh)
{
	return IFactory<IMesh>::DestroyObject(ppMesh);
}

void IMeshManager::DestroyAllMesh()
{
	IFactory<IMesh>::DestroyAllObject();
}

UINT IMeshManager::GetMeshCount()
{
	return IFactory<IMesh>::GetObjectCount();
}

bool Noise3D::IMeshManager::IsMeshExisted(N_UID meshName)
{
	return IFactory<IMesh>::FindUid(meshName);
}

/***********************************************************************
								P R I V A T E					                    
***********************************************************************/


IMeshManager::IMeshManager():IFactory<IMesh>(50000)
{

}

IMeshManager::~IMeshManager()
{
	DestroyAllMesh();
}
