
/***********************************************************************

								class£ºLight Mgr

					desc: store light resources data

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

ILightManager::ILightManager():
	 IFactory<IDirLightD>(10),
	IFactory<IPointLightD>(10),
	IFactory<ISpotLightD>(10),
	IFactory<IDirLightS>(50),
	IFactory<IPointLightS>(50),
	IFactory<ISpotLightS>(50)
{
	mIsDynamicLightingEnabled = true;
	mIsStaticLightingEnabled = true;
	mCanUpdateStaticLights = false;
}

ILightManager::~ILightManager()
{
}

//-----------CREATION
IDirLightD * ILightManager::CreateDynamicDirLight(N_UID lightName)
{
	return IFactory<IDirLightD>::CreateObject(lightName);
}

IPointLightD* ILightManager::CreateDynamicPointLight(N_UID lightName)
{
	return IFactory<IPointLightD>::CreateObject(lightName);
}

ISpotLightD* ILightManager::CreateDynamicSpotLight(N_UID lightName)
{
	return IFactory<ISpotLightD>::CreateObject(lightName);
}

IDirLightS * ILightManager::CreateStaticDirLight(N_UID lightName, const N_DirLightDesc& desc)
{
	IDirLightS* pLight = IFactory<IDirLightS>::CreateObject(lightName);

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
		IFactory<IDirLightS>::DestroyObject(lightName);
		return nullptr;
	}
};

IPointLightS * ILightManager::CreateStaticPointLight(N_UID lightName, const N_PointLightDesc& desc)
{
	IPointLightS* pLight = IFactory<IPointLightS>::CreateObject(lightName);

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
		IFactory<IPointLightS>::DestroyObject(lightName);
		return nullptr;
	}
}

ISpotLightS * ILightManager::CreateStaticSpotLight(N_UID lightName, const N_SpotLightDesc& desc)
{
	ISpotLightS* pLight = IFactory<ISpotLightS>::CreateObject(lightName);

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
		IFactory<ISpotLightS>::DestroyObject(lightName);
		return nullptr;
	}
}

//-----------Interface GETTER----------------------------
IDirLightD * ILightManager::GetDirLightD(N_UID lightName)
{
	return IFactory<IDirLightD>::GetObjectPtr(lightName);
}

IDirLightD * Noise3D::ILightManager::GetDirLightD(UINT index)
{
	return IFactory<IDirLightD>::GetObjectPtr(index);
}

IPointLightD * ILightManager::GetPointLightD(N_UID lightName)
{
	return  IFactory<IPointLightD>::GetObjectPtr(lightName);
}

IPointLightD * ILightManager::GetPointLightD(UINT index)
{
	return IFactory<IPointLightD>::GetObjectPtr(index);
}

ISpotLightD * Noise3D::ILightManager::GetSpotLightD(N_UID lightName)
{
	return IFactory<ISpotLightD>::GetObjectPtr(lightName);
}

ISpotLightD * ILightManager::GetSpotLightD(UINT index)
{
	return IFactory<ISpotLightD>::GetObjectPtr(index);
}

IDirLightS * ILightManager::GetDirLightS(N_UID lightName)
{
	return IFactory<IDirLightS>::GetObjectPtr(lightName);
}

IDirLightS * ILightManager::GetDirLightS(UINT index)
{
	return IFactory<IDirLightS>::GetObjectPtr(index);
}

IPointLightS * ILightManager::GetPointLightS(N_UID lightName)
{
	return IFactory<IPointLightS>::GetObjectPtr(lightName);
}

IPointLightS * ILightManager::GetPointLightS(UINT index)
{
	return IFactory<IPointLightS>::GetObjectPtr(index);
}

ISpotLightS * ILightManager::GetSpotLightS(N_UID lightName)
{
	return IFactory<ISpotLightS>::GetObjectPtr(lightName);
}

ISpotLightS * ILightManager::GetSpotLightS(UINT index)
{
	return  IFactory<ISpotLightS>::GetObjectPtr(index);
}

//-----------Dynamic Light Deletion-------------
bool ILightManager::DeleteDirLightD(N_UID lightName)
{
	return IFactory<IDirLightD>::DestroyObject(lightName);
}

bool ILightManager::DeleteDirLightD(IDirLightD * pLight)
{
	return IFactory<IDirLightD>::DestroyObject(pLight);
}

bool ILightManager::DeletePointLightD(N_UID lightName)
{
	return IFactory<IPointLightD>::DestroyObject(lightName);
}

bool ILightManager::DeletePointLightD(IPointLightD * pLight)
{
	return IFactory<IPointLightD>::DestroyObject(pLight);
}

bool ILightManager::DeleteSpotLightD(N_UID lightName)
{
	return IFactory<ISpotLightD>::DestroyObject(lightName);
}

bool ILightManager::DeleteSpotLightD(ISpotLightD * pLight)
{
	return IFactory<ISpotLightD>::DestroyObject(pLight);
}


//---------Static Light Deletion---------------
bool ILightManager::DeleteDirLightS(N_UID lightName)
{
	mCanUpdateStaticLights = true;
	return IFactory<IDirLightS>::DestroyObject(lightName);
}

bool  ILightManager::DeleteDirLightS(IDirLightS * pLight)
{
	mCanUpdateStaticLights = true;
	return IFactory<IDirLightS>::DestroyObject(pLight);
}

bool ILightManager::DeletePointLightS(N_UID lightName)
{
	mCanUpdateStaticLights = true;
	return IFactory<IPointLightS>::DestroyObject(lightName);
}

bool ILightManager::DeletePointLightS(IPointLightS * pLight)
{
	mCanUpdateStaticLights = true;
	return IFactory<IPointLightS>::DestroyObject(pLight);
}

bool ILightManager::DeleteSpotLightS(N_UID lightName)
{
	mCanUpdateStaticLights = true;
	return IFactory<ISpotLightS>::DestroyObject(lightName);
}

bool ILightManager::DeleteSpotLightS(ISpotLightS * pLight)
{
	mCanUpdateStaticLights = true;
	return IFactory<ISpotLightS>::DestroyObject(pLight);
}


void	ILightManager::SetDynamicLightingEnabled(bool isEnabled)
{
	mIsDynamicLightingEnabled = isEnabled;
};


bool ILightManager::IsDynamicLightingEnabled()
{
	return mIsDynamicLightingEnabled;
}



UINT	ILightManager::GetLightCount(NOISE_LIGHT_TYPE lightType)
{
	switch(lightType)
	{
	case NOISE_LIGHT_TYPE_DYNAMIC_DIR :
		return IFactory<IDirLightD>::GetObjectCount();
		break;
	
	case NOISE_LIGHT_TYPE_STATIC_DIR:
		return IFactory<IDirLightS>::GetObjectCount();
		break;

	case NOISE_LIGHT_TYPE_DYNAMIC_POINT :
		return IFactory<IPointLightD>::GetObjectCount();
		break;

	case NOISE_LIGHT_TYPE_STATIC_POINT:
		return IFactory<IPointLightS>::GetObjectCount();
		break;
	
	case NOISE_LIGHT_TYPE_DYNAMIC_SPOT :
		return IFactory<ISpotLightD>::GetObjectCount(); 
		break;

	case NOISE_LIGHT_TYPE_STATIC_SPOT:
		return IFactory<ISpotLightS>::GetObjectCount();
		break;

	default:
		return 0;
	}
	return 0;
}

UINT	ILightManager::GetDynamicLightCount()
{
	return IFactory<IDirLightD>::GetObjectCount() + IFactory<IPointLightD>::GetObjectCount() + IFactory<ISpotLightD>::GetObjectCount();
};

UINT	ILightManager::GetStaticLightCount()
{
	return IFactory<IDirLightS>::GetObjectCount() + IFactory<IPointLightS>::GetObjectCount() + IFactory<ISpotLightS>::GetObjectCount();
};

UINT	ILightManager::GetTotalLightCount()
{
	return GetDynamicLightCount() + GetStaticLightCount();
};


/***********************************************************************
								PRIVATE					                    
***********************************************************************/
