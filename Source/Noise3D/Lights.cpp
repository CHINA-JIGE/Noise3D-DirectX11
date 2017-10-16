
/***********************************************************************

                           cpp£º Lights

			Desc£ºdef of light interfaces and light description structure

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

//-------------------Base Light--------------------------

void IBaseLight::SetAmbientColor(const NVECTOR3 & color)
{
	mBaseLightDesc.ambientColor = Clamp(color, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
}

void IBaseLight::SetDiffuseColor(const NVECTOR3 & color)
{
	mBaseLightDesc.diffuseColor = Clamp(color, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
}

void IBaseLight::SetSpecularColor(const NVECTOR3 & color)
{
	mBaseLightDesc.specularColor = Clamp(color, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
}

void IBaseLight::SetSpecularIntensity(float specInt)
{
	mBaseLightDesc.specularIntensity = Clamp(specInt, 0.0f, 100.0f);
}

void IBaseLight::SetDiffuseIntensity(float diffInt)
{
	mBaseLightDesc.diffuseIntensity = Clamp(diffInt, 0.0f, 100.0f);
}

void IBaseLight::SetDesc(const N_CommonLightDesc & desc)
{
	SetDiffuseColor(desc.diffuseColor);
	SetAmbientColor(desc.ambientColor);
	SetSpecularColor(desc.specularColor);
	SetSpecularIntensity(desc.specularIntensity);
	SetDiffuseIntensity(desc.diffuseIntensity);
}

void IBaseLight::GetDesc(N_CommonLightDesc & outDesc)
{
	outDesc.ambientColor = mBaseLightDesc.ambientColor;
	outDesc.diffuseColor = mBaseLightDesc.diffuseColor;
	outDesc.specularColor = mBaseLightDesc.specularColor;
	outDesc.diffuseIntensity = mBaseLightDesc.diffuseIntensity;
	outDesc.specularIntensity = mBaseLightDesc.specularIntensity;
}



//--------------------DYNAMIC DIR LIGHT------------------
IDirLightD::IDirLightD()
{
	ZeroMemory(this, sizeof(*this));
	mLightDesc.specularIntensity = 1.0f;
	mLightDesc.direction = NVECTOR3(1.0f, 0, 0);
	mLightDesc.diffuseIntensity = 0.5;
}

IDirLightD::~IDirLightD()
{
}

void IDirLightD::SetDirection(const NVECTOR3& dir)
{
	//the length of directional vector must be greater than 0
	if (!(dir.x == 0 && dir.y == 0 && dir.z == 0))
	{
		mLightDesc.direction = dir;
	}
}

void IDirLightD::SetDesc(const N_DirLightDesc & desc)
{
	IBaseLight::SetDesc(desc);//only modify the common part
	SetDirection(desc.direction);//modify extra part
}

N_DirLightDesc IDirLightD::GetDesc()
{
	//fill in the common attribute part
	IBaseLight::GetDesc(mLightDesc);
	return mLightDesc;
}




//--------------------DYNAMIC POINT LIGHT------------------
IPointLightD::IPointLightD()
{
	mLightDesc.specularIntensity = 1.0f;
	mLightDesc.mAttenuationFactor = 0.05f;
	mLightDesc.mLightingRange = 100.0f;
	mLightDesc.diffuseIntensity = 0.5;
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
	//fill in the common attribute part
	IBaseLight::GetDesc(mLightDesc);
	return mLightDesc;
}



//--------------------DYNAMIC SPOT LIGHT------------------

ISpotLightD::ISpotLightD()
{
	mLightDesc.specularIntensity = 1.0f;
	mLightDesc.mAttenuationFactor = 1.0f;
	mLightDesc.mLightingRange = 100.0f;
	mLightDesc.mLightingAngle = MATH_PI / 4;
	mLightDesc.diffuseIntensity = 0.5;
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
	mLightDesc.mLightingAngle = Clamp(coneAngle_Rad, 0.0f, MATH_PI-0.001f);
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

N_SpotLightDesc ISpotLightD::GetDesc()
{
	//fill in the common attribute part
	IBaseLight::GetDesc(mLightDesc);
	return mLightDesc;
}



//--------------------STATIC DIR LIGHT------------------

N_DirLightDesc IDirLightS::GetDesc()
{
	return mLightDesc;
}

IDirLightS::IDirLightS()
{

};

IDirLightS::~IDirLightS()
{
}

bool IDirLightS::mFunction_Init(const N_DirLightDesc & desc)
{
	mLightDesc.ambientColor = Clamp(desc.ambientColor, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
	mLightDesc.diffuseColor = Clamp(desc.diffuseColor, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
	mLightDesc.specularColor = Clamp(desc.specularColor, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
	mLightDesc.specularIntensity = Clamp(desc.specularIntensity, 0.0f, 100.0f);
	mLightDesc.diffuseIntensity = Clamp(desc.diffuseIntensity, 0.0f, 100.0f);

	//the length of directional vector must be greater than 0
	const NVECTOR3& dir = desc.direction;
	if ((dir.x == 0 && dir.y == 0 && dir.z == 0))
	{
		ERROR_MSG("Dir Light Init: direction can't be (0,0,0)");
		return false;
	}
	else
	{
		mLightDesc.direction = dir;
		return true;
	}
}



//--------------------STATIC POINT LIGHT------------------

N_PointLightDesc IPointLightS::GetDesc()
{
	return mLightDesc;
};

IPointLightS::IPointLightS()
{
}

IPointLightS::~IPointLightS()
{
}

bool IPointLightS::mFunction_Init(const N_PointLightDesc & desc)
{
	mLightDesc.ambientColor = Clamp(desc.ambientColor, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
	mLightDesc.diffuseColor = Clamp(desc.diffuseColor, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
	mLightDesc.specularColor = Clamp(desc.specularColor, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
	mLightDesc.specularIntensity = Clamp(desc.specularIntensity, 0.0f, 100.0f);
	mLightDesc.diffuseIntensity = Clamp(desc.diffuseIntensity, 0.0f, 100.0f);
	mLightDesc.mPosition = desc.mPosition;
	mLightDesc.mAttenuationFactor = Clamp(desc.mAttenuationFactor, 0.0f, 1.0f);
	mLightDesc.mLightingRange = Clamp(desc.mAttenuationFactor, 0.0f, 10000000.0f);
	return true;
}


//--------------------STATIC SPOT LIGHT------------------

N_SpotLightDesc ISpotLightS::GetDesc()
{
	return mLightDesc;
}

ISpotLightS::ISpotLightS()
{
}

ISpotLightS::~ISpotLightS()
{
}

bool ISpotLightS::mFunction_Init(const N_SpotLightDesc & desc)
{
	mLightDesc.ambientColor = Clamp(desc.ambientColor, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
	mLightDesc.diffuseColor = Clamp(desc.diffuseColor, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
	mLightDesc.specularColor = Clamp(desc.specularColor, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
	mLightDesc.specularIntensity = Clamp(desc.specularIntensity, 0.0f, 100.0f);
	mLightDesc.diffuseIntensity = Clamp(desc.diffuseIntensity, 0.0f, 100.0f);
	mLightDesc.mPosition = desc.mPosition;
	mLightDesc.mAttenuationFactor = Clamp(desc.mAttenuationFactor, 0.0f, 1.0f);
	mLightDesc.mLightingRange = Clamp(desc.mAttenuationFactor, 0.0f, 10000000.0f);

	//pos and litAt can't superpose
	NVECTOR3 deltaVec = desc.mLitAt - desc.mPosition;
	if (!(deltaVec.x == 0 && deltaVec.y == 0 && deltaVec.z == 0))
	{
		mLightDesc.mLitAt = desc.mLitAt;
	}
	else
	{
		ERROR_MSG("Spot Light Init: pos and LitAt can't be the same.");
		return false;
	}

	// i'm not sure...but spot light should have a cone angle smaller than ¦Ð...??
	mLightDesc.mLightingAngle = Clamp(desc.mLightingAngle, 0.0f, MATH_PI - 0.001f);
	return true;
}
