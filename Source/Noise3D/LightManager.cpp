
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
	mIsDynamicLightingEnabled = TRUE;
	mIsStaticLightingEnabled = TRUE;
	mCanUpdateStaticLights = FALSE;
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
	BOOL isSucceeded  = pLight->mFunction_Init(desc);
	
	if (isSucceeded)
	{
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
	BOOL isSucceeded = pLight->mFunction_Init(desc);

	if (isSucceeded)
	{
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
	BOOL isSucceeded = pLight->mFunction_Init(desc);

	if (isSucceeded)
	{
		return pLight;
	}
	else
	{
		//clear invalid ptr
		IFactory<ISpotLightS>::DestroyObject(lightName);
		return nullptr;
	}
}

//--------Dynamic Light Deletion------------
BOOL ILightManager::DeleteDirLightD(N_UID lightName)
{
	return IFactory<IDirLightD>::DestroyObject(lightName);
}

BOOL ILightManager::DeleteDirLightD(IDirLightD * pLight)
{
	return IFactory<IDirLightD>::DestroyObject(pLight);
}

BOOL ILightManager::DeletePointLightD(N_UID lightName)
{
	return IFactory<IPointLightD>::DestroyObject(lightName);
}

BOOL ILightManager::DeletePointLightD(IPointLightD * pLight)
{
	return IFactory<IPointLightD>::DestroyObject(pLight);
}

BOOL ILightManager::DeleteSpotLightD(N_UID lightName)
{
	return IFactory<ISpotLightD>::DestroyObject(lightName);
}

BOOL ILightManager::DeleteSpotLightD(ISpotLightD * pLight)
{
	return IFactory<ISpotLightD>::DestroyObject(pLight);
}


//---------Static Light Deletion---------------
BOOL ILightManager::DeleteDirLightS(N_UID lightName)
{
	mCanUpdateStaticLights = TRUE;
	return IFactory<IDirLightS>::DestroyObject(lightName);
}

BOOL  ILightManager::DeleteDirLightS(IDirLightS * pLight)
{
	mCanUpdateStaticLights = TRUE;
	return IFactory<IDirLightS>::DestroyObject(pLight);
}

BOOL ILightManager::DeletePointLightS(N_UID lightName)
{
	mCanUpdateStaticLights = TRUE;
	return IFactory<IPointLightS>::DestroyObject(lightName);
}

BOOL ILightManager::DeletePointLightS(IPointLightS * pLight)
{
	mCanUpdateStaticLights = TRUE;
	return IFactory<IPointLightS>::DestroyObject(pLight);
}

BOOL ILightManager::DeleteSpotLightS(N_UID lightName)
{
	mCanUpdateStaticLights = TRUE;
	return IFactory<ISpotLightS>::DestroyObject(lightName);
}

BOOL ILightManager::DeleteSpotLightS(ISpotLightS * pLight)
{
	mCanUpdateStaticLights = TRUE;
	return IFactory<ISpotLightS>::DestroyObject(pLight);
}


void	ILightManager::SetDynamicLightingEnabled(BOOL isEnabled)
{
	mIsDynamicLightingEnabled = isEnabled;
};

void	ILightManager::SetStaticLightingEnabled(BOOL isEnabled)
{
	mIsStaticLightingEnabled = isEnabled;
};


UINT	ILightManager::GetLightCount(NOISE_LIGHT_TYPE lightType)
{
	switch(lightType)
	{
	case NOISE_LIGHT_TYPE_DIRECTIONAL :
		return (IFactory<IDirLightD>::GetObjectCount()+ IFactory<IDirLightS>::GetObjectCount());
		break;
	case NOISE_LIGHT_TYPE_POINT :
		return (IFactory<IPointLightD>::GetObjectCount() + IFactory<IPointLightS>::GetObjectCount());
		break;
	case NOISE_LIGHT_TYPE_SPOT :
		return (IFactory<ISpotLightD>::GetObjectCount() + IFactory<ISpotLightS>::GetObjectCount());
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
