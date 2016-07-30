
/***********************************************************************

                           cpp£º Lights

			Desc£ºdef of light interfaces and light description structure

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

IDirLightD::IDirLightD()
{
	ZeroMemory(this, sizeof(*this));
	mSpecularIntensity = 1.0f;
	mDirection = NVECTOR3(1.0f, 0, 0);
	mDiffuseIntensity = 0.5;
}



IPointLightD::IPointLightD()
{
	mSpecularIntensity = 1.0f;
	mAttenuationFactor = 0.05f;
	mLightingRange = 100.0f;
	mDiffuseIntensity = 0.5;
}

ISpotLightD::ISpotLightD()
{
	mSpecularIntensity = 1.0f;
	mAttenuationFactor = 1.0f;
	mLightingRange = 100.0f;
	mLightingAngle = MATH_PI / 4;
	mDiffuseIntensity = 0.5;
	mLitAt = NVECTOR3(0, 0, 0);
}