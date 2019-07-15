
/***********************************************************************

								h£ºSweepingTrailManager

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

Noise3D::SweepingTrailManager::SweepingTrailManager():
	IFactory<SweepingTrail>(10000)
{

}

Noise3D::SweepingTrailManager::~SweepingTrailManager()
{
	IFactory<SweepingTrail>::DestroyAllObject();
}

SweepingTrail * Noise3D::SweepingTrailManager::CreateSweepingTrail(SceneNode* pAttachedNode, N_UID objName, uint32_t maxVertexCount)
{
	if (pAttachedNode == nullptr)
	{
		ERROR_MSG("SweepingTrailMgr: Failed to create mesh. Father scene node is invalid.");
		return nullptr;
	}

	auto pTrail = IFactory<SweepingTrail>::CreateObject(objName);
	if (pTrail->mFunction_InitGpuBuffer(maxVertexCount))
	{
		//init scene object info(necessary for class derived from ISceneObject)
		pTrail->ISceneObject::mFunc_InitSceneObject(objName, pAttachedNode);
		return pTrail;
	}
	else
	{
		ERROR_MSG("SweepingTrailMgr: failed to init Gpu buffer!");
		IFactory<SweepingTrail>::DestroyObject(objName);
		return nullptr;
	}

}