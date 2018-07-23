
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
}
