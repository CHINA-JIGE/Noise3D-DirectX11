
/***********************************************************************

							class£ºMeshManager

				description: manager class to manage IMesh

***********************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

IGraphicObjectManager::IGraphicObjectManager()
	:IFactory<IGraphicObject>(50000)
{

}

IGraphicObjectManager::~IGraphicObjectManager()
{
	IFactory<IGraphicObject>::DestroyAllObject();
}


IGraphicObject * IGraphicObjectManager::CreateGraphicObj(N_UID objName)
{
	return  IFactory<IGraphicObject>::CreateObject(objName);
}

IGraphicObject * IGraphicObjectManager::GetGraphicObj(N_UID objName)
{
	return  IFactory<IGraphicObject>::GetObjectPtr(objName);
}

IGraphicObject * IGraphicObjectManager::GetGraphicObj(UINT index)
{
	return  IFactory<IGraphicObject>::GetObjectPtr(index);
}

bool IGraphicObjectManager::DestroyGraphicObj(N_UID objName)
{
	return IFactory<IGraphicObject>::DestroyObject(objName);
}

bool IGraphicObjectManager::DestroyGraphicObj(IGraphicObject * pObj)
{
	return IFactory<IGraphicObject>::DestroyObject(pObj);
}

void IGraphicObjectManager::DestroyAllGraphicObj()
{
	IFactory<IGraphicObject>::DestroyAllObject();
}

UINT IGraphicObjectManager::GetGraphicObjCount()
{
	return IFactory<IGraphicObject>::GetObjectCount();
}

