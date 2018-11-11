
/***********************************************************************

							class£ºMeshManager

				description: manager class to manage Mesh

***********************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

GraphicObjectManager::GraphicObjectManager()
	:IFactory<GraphicObject>(50000)
{

}

GraphicObjectManager::~GraphicObjectManager()
{
	IFactory<GraphicObject>::DestroyAllObject();
}


GraphicObject * GraphicObjectManager::CreateGraphicObj(N_UID objName)
{
	return  IFactory<GraphicObject>::CreateObject(objName);
}

GraphicObject * GraphicObjectManager::GetGraphicObj(N_UID objName)
{
	return  IFactory<GraphicObject>::GetObjectPtr(objName);
}

GraphicObject * GraphicObjectManager::GetGraphicObj(UINT index)
{
	return  IFactory<GraphicObject>::GetObjectPtr(index);
}

bool GraphicObjectManager::DestroyGraphicObj(N_UID objName)
{
	return IFactory<GraphicObject>::DestroyObject(objName);
}

bool GraphicObjectManager::DestroyGraphicObj(GraphicObject * pObj)
{
	return IFactory<GraphicObject>::DestroyObject(pObj);
}

void GraphicObjectManager::DestroyAllGraphicObj()
{
	IFactory<GraphicObject>::DestroyAllObject();
}

UINT GraphicObjectManager::GetGraphicObjCount()
{
	return IFactory<GraphicObject>::GetObjectCount();
}

