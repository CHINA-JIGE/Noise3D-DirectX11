
/***********************************************************************

                           cpp£º Lights

			Desc£ºdef of light interfaces and light description structure

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

//-------------------Base Light--------------------------

void IBaseLight::SetAmbientColor(const NVECTOR3 & color)
{
	mLightDesc.mAmbientColor = Clamp(color, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
}

void IBaseLight::SetDiffuseColor(const NVECTOR3 & color)
{
	mLightDesc.mDiffuseColor = Clamp(color, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
}

void IBaseLight::SetSpecularColor(const NVECTOR3 & color)
{
	mLightDesc.mSpecularColor = Clamp(color, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
}

void IBaseLight::SetSpecularIntensity(float specInt)
{
	mLightDesc.mSpecularIntensity = Clamp(specInt, 0.0f, 100.0f);
}

void IBaseLight::SetDiffuseIntensity(float diffInt)
{
	mLightDesc.mDiffuseIntensity = Clamp(diffInt, 0.0f, 100.0f);
}

void IBaseLight::SetDesc(const N_CommonLightDesc & desc)
{
	SetDiffuseColor(desc.mDiffuseColor);
	SetAmbientColor(desc.mAmbientColor);
	SetSpecularColor(desc.mSpecularColor);
	SetSpecularIntensity(desc.mSpecularIntensity);
	SetDiffuseIntensity(desc.mDiffuseIntensity);
}

N_CommonLightDesc IBaseLight::GetDesc()
{
	return mLightDesc;
}


//--------------------DYNAMIC DIR LIGHT------------------
IDirLightD::IDirLightD()
{
	ZeroMemory(this, sizeof(*this));
	mLightDesc.mSpecularIntensity = 1.0f;
	mLightDesc.mDirection = NVECTOR3(1.0f, 0, 0);
	mLightDesc.mDiffuseIntensity = 0.5;
}

IDirLightD::~IDirLightD()
{
}

void IDirLightD::SetDirection(const NVECTOR3& dir)
{
	//the length of directional vector must be greater than 0
	if (!(dir.x == 0 && dir.y == 0 && dir.z == 0))
	{
		mLightDesc.mDirection = dir;
	}
}

void IDirLightD::SetDesc(const N_DirLightDesc & desc)
{
	IBaseLight::SetDesc(desc);//only modify the common part
	SetDirection(desc.mDirection);//modify extra part
}

N_DirLightDesc IDirLightD::GetDesc()
{
	return mLightDesc;
}




//--------------------DYNAMIC POINT LIGHT------------------
IPointLightD::IPointLightD()
{
	mLightDesc.mSpecularIntensity = 1.0f;
	mLightDesc.mAttenuationFactor = 0.05f;
	mLightDesc.mLightingRange = 100.0f;
	mLightDesc.mDiffuseIntensity = 0.5;
}

IPointLightD::~IPointLightD()
{
}

void IPointLightD::SetPosition(const NVECTOR3 & pos)
{
	mLightDesc.mPosition = pos;
}

void IPointLightD::SetAttenuationFactor(float attFactor)
{
	mLightDesc.mAttenuationFactor = Clamp(attFactor,0.0f,1.0f);
}

void IPointLightD::SetLightingRange(float range)
{
	mLightDesc.mLightingRange = Clamp(range, 0.0f, 10000000.0f);
}

void IPointLightD::SetDesc(const N_PointLightDesc & desc)
{
	IBaseLight::SetDesc(desc);
	SetPosition(desc.mPosition);
	SetAttenuationFactor(desc.mAttenuationFactor);
	SetLightingRange(desc.mLightingRange);
}

N_PointLightDesc IPointLightD::GetDesc()
{
	return mLightDesc;
}



//--------------------DYNAMIC SPOT LIGHT------------------

ISpotLightD::ISpotLightD()
{
	mLightDesc.mSpecularIntensity = 1.0f;
	mLightDesc.mAttenuationFactor = 1.0f;
	mLightDesc.mLightingRange = 100.0f;
	mLightDesc.mLightingAngle = MATH_PI / 4;
	mLightDesc.mDiffuseIntensity = 0.5;
	mLightDesc.mLitAt = NVECTOR3(1.0f, 0, 0);
	mLightDesc.mPosition = NVECTOR3(0, 0, 0);
}

ISpotLightD::~ISpotLightD()
{
}

void ISpotLightD::SetPosition(const NVECTOR3 & pos)
{
	NVECTOR3 deltaVec = pos - mLightDesc.mLitAt;

	//pos and litAt can't superpose
	if (!(deltaVec.x == 0 && deltaVec.y == 0 && deltaVec.z == 0))
	{
		mLightDesc.mPosition = pos;
	}
}

void ISpotLightD::SetAttenuationFactor(float attFactor)
{
	mLightDesc.mAttenuationFactor = Clamp(attFactor,0.0f,1.0f);
}

void ISpotLightD::SetLitAt(const NVECTOR3 & vLitAt)
{
	NVECTOR3 deltaVec = vLitAt - mLightDesc.mPosition;

	//pos and litAt can't superpose
	if (!(deltaVec.x == 0 && deltaVec.y == 0 && deltaVec.z == 0))
	{
		mLightDesc.mLitAt = vLitAt;
	}
}

void ISpotLightD::SetLightingAngle(float coneAngle_Rad)
{
	// i'm not sure...but spot light should have a cone angle smaller than ¦Ð...??
	mLightDesc.mLightingAngle = Clamp(coneAngle_Rad, 0.0f, MATH_PI-0.01f);
}

void ISpotLightD::SetLightingRange(float range)
{
	mLightDesc.mLightingRange = Clamp(range, 0.0f, 10000000.0f);
}

void ISpotLightD::SetDesc(const N_SpotLightDesc & desc)
{
	IBaseLight::SetDesc(desc);
	SetPosition(desc.mPosition);
	SetLitAt(desc.mLitAt);
	SetAttenuationFactor(desc.mAttenuationFactor);
	SetLightingRange(desc.mLightingRange);
	SetLightingAngle(desc.mLightingAngle);
}

N_SpotLightDesc Noise3D::ISpotLightD::GetDesc()
{
	return mLightDesc;
}



//--------------------STATIC DIR LIGHT------------------

