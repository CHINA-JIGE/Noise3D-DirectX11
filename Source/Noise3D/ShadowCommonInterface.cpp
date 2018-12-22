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

void Noise3D::IShadowCaster::EnableCastingShadow(bool enabledSC)
{
	mIsCastingShadowEnabled = enabledSC;
}

bool Noise3D::IShadowCaster::IsCastingShadowEnabled()
{
	return mIsCastingShadowEnabled;
}

ID3D11DepthStencilView * Noise3D::IShadowCaster::GetShadowMapDsv()
{
	return m_pShadowMapDSV;
}


/***********************************************************************

									 ShadowReceiver

			Desc: common interface of a shadow Receiver
			(usually a solid object that have shadow projected on itself)
			will be inherited by classes like 'lights'

************************************************************************/

Noise3D::IShadowReceiver::IShadowReceiver() :
	mIsReceivingShadowEnabled(false),
	mShadowColor(NColor4f(0, 0, 0, 0.5f))
{
}

void Noise3D::IShadowReceiver::EnableReceivingShadow(bool enabledRC)
{
	mIsReceivingShadowEnabled = enabledRC;
}

bool Noise3D::IShadowReceiver::IsReceivingShadowEnabled()
{
	return mIsReceivingShadowEnabled;
}

void Noise3D::IShadowReceiver::SetShadowColor(NColor4f c)
{
	mShadowColor = c;
}
