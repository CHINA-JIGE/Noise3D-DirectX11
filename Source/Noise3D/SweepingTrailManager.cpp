
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

SweepingTrail * Noise3D::SweepingTrailManager::CreateSweepingTrail(N_UID objName, uint32_t maxVertexCount)
{
	if (IFactory<SweepingTrail>::FindUid(objName))
	{
		ERROR_MSG("CreateSweepingTrail: UID existed!");
		return nullptr;
	}

	auto pTrail = IFactory<SweepingTrail>::CreateObject(objName);
	if (!pTrail->mFunction_InitGpuBuffer(maxVertexCount))
	{
		ERROR_MSG("CreateSweepingTrail: Failed to init Gpu buffer!");
		IFactory<SweepingTrail>::DestroyObject(objName);
		return nullptr;
	}

	//init succeed
	return pTrail;
}

SweepingTrail * Noise3D::SweepingTrailManager::GetSweepingTrail(N_UID objName)
{
	return IFactory<SweepingTrail>::GetObjectPtr(objName);
}

SweepingTrail * Noise3D::SweepingTrailManager::GetSweepingTrail(UINT index)
{
	return IFactory<SweepingTrail>::GetObjectPtr(index);
}

bool Noise3D::SweepingTrailManager::DestroySweepingTrail(N_UID objName)
{
	return IFactory<SweepingTrail>::DestroyObject(objName);
}

bool Noise3D::SweepingTrailManager::DestroySweepingTrail(SweepingTrail * pST)
{
	return IFactory<SweepingTrail>::DestroyObject(pST);
}

void Noise3D::SweepingTrailManager::DestroyAllSweepingTrail()
{
	IFactory<SweepingTrail>::DestroyAllObject();
}

UINT Noise3D::SweepingTrailManager::GetSweepingTrailCount()
{
	return IFactory<SweepingTrail>::GetObjectCount();
}
