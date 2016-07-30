
/***********************************************************************

                           ¿‡£∫ Light Mgr

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

IDirLightS * ILightManager::CreateStaticDirLight(N_UID lightName)
{
	return IFactory<IDirLightS>::CreateObject(lightName);
}

IPointLightS * ILightManager::CreateStaticPointLight(N_UID lightName)
{
	return IFactory<IPointLightS>::CreateObject(lightName);
}

ISpotLightS * ILightManager::CreateStaticSpotLight(N_UID lightName)
{
	return IFactory<ISpotLightS>::CreateObject(lightName);
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
	return IFactory<IDirLightS>::DestroyObject(lightName);
}

BOOL  ILightManager::DeleteDirLightS(IDirLightS * pLight)
{
	return IFactory<IDirLightS>::DestroyObject(pLight);
}

BOOL ILightManager::DeletePointLightS(N_UID lightName)
{
	return IFactory<IPointLightS>::DestroyObject(lightName);
}

BOOL ILightManager::DeletePointLightS(IPointLightS * pLight)
{
	return IFactory<IPointLightS>::DestroyObject(pLight);
}

BOOL ILightManager::DeleteSpotLightS(N_UID lightName)
{
	return IFactory<ISpotLightS>::DestroyObject(lightName);
}

BOOL ILightManager::DeleteSpotLightS(ISpotLightS * pLight)
{
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


UINT	 ILightManager::GetLightCount(NOISE_LIGHT_TYPE lightType)
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

UINT ILightManager::GetDynamicLightCount()
{
	return IFactory<IDirLightD>::GetObjectCount() + IFactory<IPointLightD>::GetObjectCount() + IFactory<ISpotLightD>::GetObjectCount();
};

UINT Noise3D::ILightManager::GetStaticLightCount()
{
	return IFactory<IDirLightS>::GetObjectCount() + IFactory<IPointLightS>::GetObjectCount() + IFactory<ISpotLightS>::GetObjectCount();
};


UINT	 ILightManager::GetTotalLightCount()
{
	return GetDynamicLightCount() + GetStaticLightCount();
};


/***********************************************************************
								PRIVATE					                    
***********************************************************************/
