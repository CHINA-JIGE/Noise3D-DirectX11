/***********************************************************************

							 ShadowXXX

			Desc: Shadow Caster and Shadow receiver

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

/***********************************************************************

									 ShadowCaster

			Desc: common interface of a shadow caster
			(usually a solid object that can be rendered to shadow map)
			will be inherited by classes like 'Mesh'

************************************************************************/
Noise3D::IShadowCaster::IShadowCaster():
	m_pShadowMapDSV(nullptr),
	mIsCastingShadowEnabled(true),
	mShadowMapProjectionType(SHADOW_MAP_PROJECTION_TYPE::PERSPECTIVE),
	mRectRealWidth(0),
	mRectRealHeight(0),
	mPixelWidth(0),
	mPixelHeight(0)
{
}


/***********************************************************************

									 ShadowReceiver

			Desc: common interface of a shadow Receiver
			(usually a solid object that have shadow projected on itself)
			will be inherited by classes like 'lights'

************************************************************************/