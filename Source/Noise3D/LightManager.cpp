
/***********************************************************************

								class£ºLight Mgr

					desc: store light resources data

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

LightManager::LightManager():
	IFactory<DirLightD>(10),
	IFactory<PointLightD>(10),
	IFactory<SpotLightD>(10),
	IFactory<DirLightS>(50),
	IFactory<PointLightS>(50),
	IFactory<SpotLightS>(50)
{
	mIsDynamicLightingEnabled = true;
	mIsStaticLightingEnabled = true;
	mCanUpdateStaticLights = false;
}

LightManager::~LightManager()
{
}

//-----------CREATION
DirLightD * LightManager::CreateDynamicDirLight(N_UID lightName)
{
	return IFactory<DirLightD>::CreateObject(lightName);
}

PointLightD* LightManager::CreateDynamicPointLight(N_UID lightName)
{
	return IFactory<PointLightD>::CreateObject(lightName);
}

SpotLightD* LightManager::CreateDynamicSpotLight(N_UID lightName)
{
	return IFactory<SpotLightD>::CreateObject(lightName);
}

DirLightS * LightManager::CreateStaticDirLight(N_UID lightName, const N_DirLightDesc& desc)
{
	DirLightS* pLight = IFactory<DirLightS>::CreateObject(lightName);

	//init could fail and we must check
	bool isSucceeded  = pLight->mFunction_Init(desc);
	
	if (isSucceeded)
	{
		mCanUpdateStaticLights = true;
		return pLight;
	}
	else
	{
		//clear invalid ptr
		IFactory<DirLightS>::DestroyObject(lightName);
		return nullptr;
	}
};

PointLightS * LightManager::CreateStaticPointLight(N_UID lightName, const N_PointLightDesc& desc)
{
	PointLightS* pLight = IFactory<PointLightS>::CreateObject(lightName);

	//init could fail and we must check
	bool isSucceeded = pLight->mFunction_Init(desc);

	if (isSucceeded)
	{
		mCanUpdateStaticLights = true;
		return pLight;
	}
	else
	{
		//clear invalid ptr
		IFactory<PointLightS>::DestroyObject(lightName);
		return nullptr;
	}
}

SpotLightS * LightManager::CreateStaticSpotLight(N_UID lightName, const N_SpotLightDesc& desc)
{
	SpotLightS* pLight = IFactory<SpotLightS>::CreateObject(lightName);

	//init could fail and we must check
	bool isSucceeded = pLight->mFunction_Init(desc);

	if (isSucceeded)
	{
		mCanUpdateStaticLights = true;
		return pLight;
	}
	else
	{
		//clear invalid ptr
		IFactory<SpotLightS>::DestroyObject(lightName);
		return nullptr;
	}
}

//-----------Interface GETTER----------------------------
DirLightD * LightManager::GetDirLightD(N_UID lightName)
{
	return IFactory<DirLightD>::GetObjectPtr(lightName);
}

DirLightD * Noise3D::LightManager::GetDirLightD(UINT index)
{
	return IFactory<DirLightD>::GetObjectPtr(index);
}

PointLightD * LightManager::GetPointLightD(N_UID lightName)
{
	return  IFactory<PointLightD>::GetObjectPtr(lightName);
}

PointLightD * LightManager::GetPointLightD(UINT index)
{
	return IFactory<PointLightD>::GetObjectPtr(index);
}

SpotLightD * Noise3D::LightManager::GetSpotLightD(N_UID lightName)
{
	return IFactory<SpotLightD>::GetObjectPtr(lightName);
}

SpotLightD * LightManager::GetSpotLightD(UINT index)
{
	return IFactory<SpotLightD>::GetObjectPtr(index);
}

DirLightS * LightManager::GetDirLightS(N_UID lightName)
{
	return IFactory<DirLightS>::GetObjectPtr(lightName);
}

DirLightS * LightManager::GetDirLightS(UINT index)
{
	return IFactory<DirLightS>::GetObjectPtr(index);
}

PointLightS * LightManager::GetPointLightS(N_UID lightName)
{
	return IFactory<PointLightS>::GetObjectPtr(lightName);
}

PointLightS * LightManager::GetPointLightS(UINT index)
{
	return IFactory<PointLightS>::GetObjectPtr(index);
}

SpotLightS * LightManager::GetSpotLightS(N_UID lightName)
{
	return IFactory<SpotLightS>::GetObjectPtr(lightName);
}

SpotLightS * LightManager::GetSpotLightS(UINT index)
{
	return  IFactory<SpotLightS>::GetObjectPtr(index);
}

//-----------Dynamic Light Deletion-------------
bool LightManager::DeleteDirLightD(N_UID lightName)
{
	return IFactory<DirLightD>::DestroyObject(lightName);
}

bool LightManager::DeleteDirLightD(DirLightD * pLight)
{
	return IFactory<DirLightD>::DestroyObject(pLight);
}

bool LightManager::DeletePointLightD(N_UID lightName)
{
	return IFactory<PointLightD>::DestroyObject(lightName);
}

bool LightManager::DeletePointLightD(PointLightD * pLight)
{
	return IFactory<PointLightD>::DestroyObject(pLight);
}

bool LightManager::DeleteSpotLightD(N_UID lightName)
{
	return IFactory<SpotLightD>::DestroyObject(lightName);
}

bool LightManager::DeleteSpotLightD(SpotLightD * pLight)
{
	return IFactory<SpotLightD>::DestroyObject(pLight);
}


//---------Static Light Deletion---------------
bool LightManager::DeleteDirLightS(N_UID lightName)
{
	mCanUpdateStaticLights = true;
	return IFactory<DirLightS>::DestroyObject(lightName);
}

bool  LightManager::DeleteDirLightS(DirLightS * pLight)
{
	mCanUpdateStaticLights = true;
	return IFactory<DirLightS>::DestroyObject(pLight);
}

bool LightManager::DeletePointLightS(N_UID lightName)
{
	mCanUpdateStaticLights = true;
	return IFactory<PointLightS>::DestroyObject(lightName);
}

bool LightManager::DeletePointLightS(PointLightS * pLight)
{
	mCanUpdateStaticLights = true;
	return IFactory<PointLightS>::DestroyObject(pLight);
}

bool LightManager::DeleteSpotLightS(N_UID lightName)
{
	mCanUpdateStaticLights = true;
	return IFactory<SpotLightS>::DestroyObject(lightName);
}

bool LightManager::DeleteSpotLightS(SpotLightS * pLight)
{
	mCanUpdateStaticLights = true;
	return IFactory<SpotLightS>::DestroyObject(pLight);
}

void	LightManager::SetDynamicLightingEnabled(bool isEnabled)
{
	mIsDynamicLightingEnabled = isEnabled;
};


bool LightManager::IsDynamicLightingEnabled()
{
	return mIsDynamicLightingEnabled;
}



UINT	LightManager::GetLightCount(NOISE_LIGHT_TYPE lightType)
{
	switch(lightType)
	{
	case NOISE_LIGHT_TYPE_DYNAMIC_DIR :
		return IFactory<DirLightD>::GetObjectCount();
		break;
	
	case NOISE_LIGHT_TYPE_STATIC_DIR:
		return IFactory<DirLightS>::GetObjectCount();
		break;

	case NOISE_LIGHT_TYPE_DYNAMIC_POINT :
		return IFactory<PointLightD>::GetObjectCount();
		break;

	case NOISE_LIGHT_TYPE_STATIC_POINT:
		return IFactory<PointLightS>::GetObjectCount();
		break;
	
	case NOISE_LIGHT_TYPE_DYNAMIC_SPOT :
		return IFactory<SpotLightD>::GetObjectCount();
		break;

	case NOISE_LIGHT_TYPE_STATIC_SPOT:
		return IFactory<SpotLightS>::GetObjectCount();
		break;

	default:
		return 0;
	}
	return 0;
}

UINT	LightManager::GetDynamicLightCount()
{
	return IFactory<DirLightD>::GetObjectCount() + IFactory<PointLightD>::GetObjectCount() + IFactory<SpotLightD>::GetObjectCount();
};

UINT	LightManager::GetStaticLightCount()
{
	return IFactory<DirLightS>::GetObjectCount() + IFactory<PointLightS>::GetObjectCount() + IFactory<SpotLightS>::GetObjectCount();
};

UINT	LightManager::GetTotalLightCount()
{
	return GetDynamicLightCount() + GetStaticLightCount();
};


/***********************************************************************
								PRIVATE					                    
***********************************************************************/
