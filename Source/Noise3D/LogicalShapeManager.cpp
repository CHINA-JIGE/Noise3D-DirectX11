
/*******************************************************

				cpp £ºLogical Shape Manager

********************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Noise3D::LogicalShapeManager::LogicalShapeManager() :
	IFactoryEx<LogicalBox, LogicalSphere,LogicalRect>({ 1000000,1000000,1000000 })
{
}

Noise3D::LogicalShapeManager::~LogicalShapeManager()
{
}

LogicalBox * Noise3D::LogicalShapeManager::CreateBox(SceneNode * pAttachedNode, N_UID uid, Vec3 size)
{
	if (pAttachedNode == nullptr)
	{
		ERROR_MSG("LogicalShapeMgr: Failed to create shape. Father scene node is invalid.");
		return nullptr;
	}
	LogicalBox* pBox = IFactory<LogicalBox>::CreateObject(uid);

	//init scene object info(necessary for class derived from ISceneObject)
	pBox->ISceneObject::mFunc_InitSceneObject(uid, pAttachedNode);
	pBox->SetSizeXYZ(size);
	return pBox;
}

LogicalSphere * Noise3D::LogicalShapeManager::CreateSphere(SceneNode * pAttachedNode, N_UID uid,float r)
{
	if (pAttachedNode == nullptr)
	{
		ERROR_MSG("LogicalShapeMgr: Failed to create shape. Father scene node is invalid.");
		return nullptr;
	}
	LogicalSphere* pSphere = IFactory<LogicalSphere>::CreateObject(uid);

	//init scene object info(necessary for class derived from ISceneObject)
	pSphere->ISceneObject::mFunc_InitSceneObject(uid, pAttachedNode);
	pSphere->SetRadius(r);
	return pSphere;
}

LogicalRect * Noise3D::LogicalShapeManager::CreateRect(SceneNode * pAttachedNode, N_UID uid, Vec2 size, NOISE_RECT_ORIENTATION ori)
{
	if (pAttachedNode == nullptr)
	{
		ERROR_MSG("LogicalShapeMgr: Failed to create shape. Father scene node is invalid.");
		return nullptr;
	}
	LogicalRect* pRect = IFactory<LogicalRect>::CreateObject(uid);

	//init scene object info(necessary for class derived from ISceneObject)
	pRect->ISceneObject::mFunc_InitSceneObject(uid, pAttachedNode);
	pRect->SetSize(size);
	pRect->SetOrientation(ori);
	return pRect;
}
