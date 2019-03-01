
/***********************************************************************

								class£ºLight Mgr

					desc: store light resources data

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

LightManager::LightManager():
	IFactory<DirLight>(10),
	IFactory<PointLight>(10),
	IFactory<SpotLight>(10),
	mIsDynamicLightingEnabled(true),
	mCanUpdateStaticLights(false)
{
}

LightManager::~LightManager()
{
}

//-----------CREATION-----------
DirLight * LightManager::CreateDynamicDirLight(N_UID lightName, N_SHADOW_MAPPING_PARAM smParam)
{
	DirLight* pLight = IFactory<DirLight>::CreateObject(lightName);

	//init of shaders/RV/states/....
	bool isSucceeded = pLight->mFunction_InitShadowMap(smParam);
	if (isSucceeded)
	{
		//init name of SceneObject
		pLight->ISceneObject::mUid = lightName;
		return pLight;
	}
	else
	{
		IFactory<DirLight>::DestroyObject(lightName);
		ERROR_MSG("LightManager: Dir light's shadow map init failed. uid: " + lightName);
		return nullptr;
	}

}

PointLight* LightManager::CreateDynamicPointLight(N_UID lightName)
{
	return IFactory<PointLight>::CreateObject(lightName);
}

SpotLight* LightManager::CreateDynamicSpotLight(N_UID lightName)
{
	return IFactory<SpotLight>::CreateObject(lightName);
}


//-----------Interface GETTER----------------------------
DirLight * LightManager::GetDirLight(N_UID lightName)
{
	return IFactory<DirLight>::GetObjectPtr(lightName);
}

DirLight * Noise3D::LightManager::GetDirLight(UINT index)
{
	return IFactory<DirLight>::GetObjectPtr(index);
}

PointLight * LightManager::GetPointLight(N_UID lightName)
{
	return  IFactory<PointLight>::GetObjectPtr(lightName);
}

PointLight * LightManager::GetPointLight(UINT index)
{
	return IFactory<PointLight>::GetObjectPtr(index);
}

SpotLight * Noise3D::LightManager::GetSpotLight(N_UID lightName)
{
	return IFactory<SpotLight>::GetObjectPtr(lightName);
}

SpotLight * LightManager::GetSpotLight(UINT index)
{
	return IFactory<SpotLight>::GetObjectPtr(index);
}

//-----------Dynamic Light Deletion-------------
bool LightManager::DeleteDirLight(N_UID lightName)
{
	return IFactory<DirLight>::DestroyObject(lightName);
}

bool LightManager::DeleteDirLight(DirLight * pLight)
{
	return IFactory<DirLight>::DestroyObject(pLight);
}

bool LightManager::DeletePointLight(N_UID lightName)
{
	return IFactory<PointLight>::DestroyObject(lightName);
}

bool LightManager::DeletePointLight(PointLight * pLight)
{
	return IFactory<PointLight>::DestroyObject(pLight);
}

bool LightManager::DeleteSpotLight(N_UID lightName)
{
	return IFactory<SpotLight>::DestroyObject(lightName);
}

bool LightManager::DeleteSpotLight(SpotLight * pLight)
{
	return IFactory<SpotLight>::DestroyObject(pLight);
}


//---------Static Light Deletion---------------
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
		return IFactory<DirLight>::GetObjectCount();
		break;
	
	case NOISE_LIGHT_TYPE_DYNAMIC_POINT :
		return IFactory<PointLight>::GetObjectCount();
		break;

	case NOISE_LIGHT_TYPE_DYNAMIC_SPOT :
		return IFactory<SpotLight>::GetObjectCount();
		break;

	default:
		return 0;
	}
	return 0;
}

UINT	LightManager::GetTotalLightCount()
{
	return IFactory<DirLight>::GetObjectCount() + IFactory<PointLight>::GetObjectCount() + IFactory<SpotLight>::GetObjectCount();
};


/***********************************************************************
								PRIVATE					                    
***********************************************************************/

void Noise3D::LightManager::mFunction_GetShadowMapRenderTaskList()
{
	ERROR_MSG("Not Implemented!");
}
