
/***********************************************************************

								h£ºSweepingTrailManager

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

Noise3D::ISweepingTrailManager::ISweepingTrailManager():
	IFactory<ISweepingTrail>(10000)
{

}

Noise3D::ISweepingTrailManager::~ISweepingTrailManager()
{
	IFactory<ISweepingTrail>::DestroyAllObject();
}

ISweepingTrail * Noise3D::ISweepingTrailManager::GetSweepingTrail(N_UID objName)
{
	return IFactory<ISweepingTrail>::GetObjectPtr(objName);
}

ISweepingTrail * Noise3D::ISweepingTrailManager::GetSweepingTrail(UINT index)
{
	return IFactory<ISweepingTrail>::GetObjectPtr(index);
}

bool Noise3D::ISweepingTrailManager::DestroySweepingTrail(N_UID objName)
{
	return IFactory<ISweepingTrail>::DestroyObject(objName);
}

bool Noise3D::ISweepingTrailManager::DestroySweepingTrail(ISweepingTrail * pST)
{
	return IFactory<ISweepingTrail>::DestroyObject(pST);
}

void Noise3D::ISweepingTrailManager::DestroyAllSweepingTrail()
{
	IFactory<ISweepingTrail>::DestroyAllObject();
}

UINT Noise3D::ISweepingTrailManager::GetSweepingTrailCount()
{
	return IFactory<ISweepingTrail>::GetObjectCount();
}
