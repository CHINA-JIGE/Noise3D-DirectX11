
/***********************************************************************

								class£ºLight Mgr

					desc: store light resources data

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

LightManager::LightManager():
	IFactoryEx<DirLight, PointLight, SpotLight>({ 5, 5, 5 }),
	mIsDynamicLightingEnabled(true),
	mCanUpdateStaticLights(false)
{
}

LightManager::~LightManager()
{
}

//-----------CREATION-----------
DirLight * LightManager::CreateDynamicDirLight(SceneNode* pFatherNode, N_UID lightName, N_SHADOW_MAPPING_PARAM smParam)
{
	DirLight* pLight = IFactory<DirLight>::CreateObject(lightName);

	//init of shaders/RV/states/....
	bool isSucceeded = pLight->mFunction_InitShadowMap(smParam);
	if (isSucceeded)
	{
		//init scene object info(necessary for class derived from ISceneObject)
		pLight->ISceneObject::mFunc_InitSceneObject(lightName, pFatherNode);
		return pLight;
	}
	else
	{
		IFactory<DirLight>::DestroyObject(lightName);
		ERROR_MSG("LightManager: Dir light's shadow map init failed. uid: " + lightName);
		return nullptr;
	}

}

PointLight* LightManager::CreateDynamicPointLight(SceneNode* pFatherNode, N_UID lightName)
{
	PointLight* pLight =  IFactory<PointLight>::CreateObject(lightName);
	//init scene object info(necessary for class derived from ISceneObject)
	pLight->ISceneObject::mFunc_InitSceneObject(lightName, pFatherNode);
	return pLight;
}

SpotLight* LightManager::CreateDynamicSpotLight(SceneNode* pFatherNode, N_UID lightName)
{
	SpotLight* pLight =  IFactory<SpotLight>::CreateObject(lightName);
	//init scene object info(necessary for class derived from ISceneObject)
	pLight->ISceneObject::mFunc_InitSceneObject(lightName, pFatherNode);
	return pLight;
}


//-----------Interface GETTER----------------------------
/*
template<typename T>
T * Noise3D::LightManager::GetLight(N_UID lightName)
{
	static_assert(false, "LightMananger: GetLight() are only supported for DirLight/PointLight/SpotLight");
}


template<>
DirLight * Noise3D::LightManager::GetLight(N_UID lightName)
{
	return IFactory<DirLight>::GetObjectPtr(lightName);
}

template<>
PointLight * Noise3D::LightManager::GetLight(N_UID lightName)
{
	return IFactory<PointLight>::GetObjectPtr(lightName);
}

template<>
SpotLight * Noise3D::LightManager::GetLight(N_UID lightName)
{
	return IFactory<SpotLight>::GetObjectPtr(lightName);
}


template<typename T>
T * Noise3D::LightManager::GetLight(uint32_t index)
{
	static_assert(false, "LightMananger: GetLight() are only supported for DirLight/PointLight/SpotLight");
}

template<>
DirLight * Noise3D::LightManager::GetLight(uint32_t index)
{
	return IFactory<DirLight>::GetObjectPtr(index);
}

template<>
PointLight * Noise3D::LightManager::GetLight(uint32_t index)
{
	return IFactory<PointLight>::GetObjectPtr(index);
}

template<>
SpotLight * Noise3D::LightManager::GetLight(uint32_t index)
{
	return IFactory<SpotLight>::GetObjectPtr(index);
}

//-----------Dynamic Light Deletion-------------
template<typename T>
bool LightManager::DeleteLight(N_UID lightName)
{
	static_assert(false, "LightMananger: DeleteLight() are only supported for DirLight/PointLight/SpotLight");
}

template <>
bool LightManager::DeleteLight<DirLight>(N_UID lightName)
{
	return IFactory<DirLight>::DestroyObject(lightName);
}

template <>
bool LightManager::DeleteLight<PointLight>(N_UID lightName)
{
	return IFactory<PointLight>::DestroyObject(lightName);
}

template <>
bool LightManager::DeleteLight<SpotLight>(N_UID lightName)
{
	return IFactory<SpotLight>::DestroyObject(lightName);
}

//IFactory operation only support DirLight/PointLight/SpotLight
template <typename T>
bool LightManager::DeleteLight(T* pLight)
{
	static_assert(false, "LightMananger: DeleteLight() are only supported for DirLight/PointLight/SpotLight");
}

template <>
bool LightManager::DeleteLight<DirLight>(DirLight * pLight)
{
	return IFactory<DirLight>::DestroyObject(pLight);
}

template <>
bool LightManager::DeleteLight<PointLight>(PointLight * pLight)
{
	return IFactory<PointLight>::DestroyObject(pLight);
}

template <>
bool LightManager::DeleteLight<SpotLight>(SpotLight * pLight)
{
	return IFactory<SpotLight>::DestroyObject(pLight);
}

//---------------Light Count-----------
template <typename T> 
uint32_t GetLightCount()
{
	static_assert(false, "LightMananger: GetLightCount() are only supported for DirLight/PointLight/SpotLight");
}

template <>
uint32_t LightManager::GetLightCount<DirLight>()
{
	  return IFactory<DirLight>::GetObjectCount();
}

template <>
uint32_t LightManager::GetLightCount<PointLight>()
{
	return IFactory<PointLight>::GetObjectCount();
}

template <>
uint32_t LightManager::GetLightCount<SpotLight>()
{
	return IFactory<SpotLight>::GetObjectCount();
}*/

//--------------
void	LightManager::SetDynamicLightingEnabled(bool isEnabled)
{
	mIsDynamicLightingEnabled = isEnabled;
};

bool LightManager::IsDynamicLightingEnabled()
{
	return mIsDynamicLightingEnabled;
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
