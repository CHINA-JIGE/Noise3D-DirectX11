
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

/*****************************************************
								PRIVATE					                    
******************************************************/

void Noise3D::LightManager::mFunction_GetShadowMapRenderTaskList()
{
	ERROR_MSG("Not Implemented!");
}
