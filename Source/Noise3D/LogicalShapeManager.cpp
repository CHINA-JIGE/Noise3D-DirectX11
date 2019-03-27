
/*******************************************************

				cpp £ºLogical Shape Manager

********************************************************/

#include "Noise3D.h"

using namespace Noise3D;

LogicalBox * Noise3D::LogicalShapeManager::CreateBox(SceneNode * pAttachedNode, N_UID uid)
{
	if (pAttachedNode == nullptr)
	{
		ERROR_MSG("LogicalShapeMgr: Failed to create shape. Father scene node is invalid.");
		return nullptr;
	}
	LogicalBox* pBox = IFactory<LogicalBox>::CreateObject(uid);

	//init scene object info(necessary for class derived from ISceneObject)
	pBox->ISceneObject::mFunc_InitSceneObject(uid, pAttachedNode);
	return pBox;
}

LogicalSphere * Noise3D::LogicalShapeManager::CreateSphere(SceneNode * pAttachedNode, N_UID uid)
{
	if (pAttachedNode == nullptr)
	{
		ERROR_MSG("LogicalShapeMgr: Failed to create shape. Father scene node is invalid.");
		return nullptr;
	}
	LogicalSphere* pSphere = IFactory<LogicalSphere>::CreateObject(uid);

	//init scene object info(necessary for class derived from ISceneObject)
	pSphere->ISceneObject::mFunc_InitSceneObject(uid, pAttachedNode);
	return pSphere;
}

Noise3D::LogicalShapeManager::LogicalShapeManager():
	IFactoryEx<LogicalBox, LogicalSphere>({ 1000000,1000000 })
{
}

Noise3D::LogicalShapeManager::~LogicalShapeManager()
{
}
