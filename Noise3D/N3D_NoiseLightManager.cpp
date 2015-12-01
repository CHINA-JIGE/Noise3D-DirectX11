
/***********************************************************************

                           类：NOISE Light Engine

			简述：主要负责管理场景的灯光，但不负责更新到GPU

************************************************************************/

#include "Noise3D.h"

NoiseLightManager::NoiseLightManager()
{
	m_pLightList_Dir_Dynamic	= new std::vector<N_DirectionalLight*>;
	m_pLightList_Point_Dynamic	= new std::vector<N_PointLight*>;
	m_pLightList_Spot_Dynamic		=	new std::vector<N_SpotLight*>;
	mIsDynamicLightingEnabled = TRUE;

	m_pLightList_Dir_Static		= new std::vector<N_DirectionalLight>;
	m_pLightList_Point_Static	= new std::vector<N_PointLight>;
	m_pLightList_Spot_Static	=	new std::vector<N_SpotLight>;
	mIsStaticLightingEnabled = TRUE;
	mCanUpdateStaticLights = FALSE;
}

void NoiseLightManager::Destroy()
{
	m_pFatherScene = nullptr;

};

//这些烂鬼add remove 就用macro吧
void	NoiseLightManager::AddDynamicDirLight(N_DirectionalLight& refLight)
{
	mFunction_AddLight<N_DirectionalLight*>(m_pLightList_Dir_Dynamic, &refLight,10);
};

void	NoiseLightManager::AddDynamicPointLight(N_PointLight& refLight)
{
	mFunction_AddLight<N_PointLight*>(m_pLightList_Point_Dynamic,&refLight,10);
};          

void NoiseLightManager::AddDynamicSpotLight(N_SpotLight& refLight)
{
	mFunction_AddLight<N_SpotLight*>(m_pLightList_Spot_Dynamic, &refLight,10);
};

void	NoiseLightManager::RemoveDynamicDirLight(N_DirectionalLight& refLight)
{
	mFunction_RemoveLight_ByAddr<N_DirectionalLight*>(m_pLightList_Dir_Dynamic, &refLight);
};

void	NoiseLightManager::RemoveDynamicPointLight(N_PointLight& refLight)
{
	mFunction_RemoveLight_ByAddr<N_PointLight*>(m_pLightList_Point_Dynamic, &refLight);
};

void	NoiseLightManager::RemoveDynamicSpotLight(N_SpotLight& refLight)
{	
	mFunction_RemoveLight_ByAddr<N_SpotLight*>(m_pLightList_Spot_Dynamic, &refLight);
};

void	NoiseLightManager::SetDynamicLightingEnabled(BOOL isEnabled)
{
	mIsDynamicLightingEnabled = isEnabled;
};



void	NoiseLightManager::AddStaticDirLight(N_DirectionalLight iLight)
{
	mFunction_AddLight<N_DirectionalLight>(m_pLightList_Dir_Static,iLight,50);
	mCanUpdateStaticLights = TRUE; 
};

void	NoiseLightManager::AddStaticPointLight(N_PointLight iLight)
{
	mFunction_AddLight<N_PointLight>(m_pLightList_Point_Static,iLight,50);
	mCanUpdateStaticLights = TRUE; 
};

void	NoiseLightManager::AddStaticSpotLight(N_SpotLight iLight)
{
	mFunction_AddLight<N_SpotLight>(m_pLightList_Spot_Static,iLight,50);
	mCanUpdateStaticLights = TRUE; 
};

void	NoiseLightManager::RemoveStaticDirLight(UINT iLight_Index)
{
	mFunction_RemoveLight_ByIndex<N_DirectionalLight>(m_pLightList_Dir_Static,iLight_Index);
	mCanUpdateStaticLights = TRUE; 
};

void	NoiseLightManager::RemoveStaticPointLight(UINT iLight_Index)
{
	mFunction_RemoveLight_ByIndex<N_PointLight>(m_pLightList_Point_Static,iLight_Index);
	mCanUpdateStaticLights = TRUE; 
};

void	NoiseLightManager::RemoveStaticSpotLight(UINT iLight_Index)
{
	mFunction_RemoveLight_ByIndex<N_SpotLight>(m_pLightList_Spot_Static,iLight_Index);
	mCanUpdateStaticLights = TRUE; 
};

void	NoiseLightManager::SetStaticLightingEnabled(BOOL isEnabled)
{
	mIsStaticLightingEnabled = isEnabled;
};


UINT	 NoiseLightManager::GetLightCount(NOISE_LIGHT_TYPE lightType)
{
	switch(lightType)
	{
	case NOISE_LIGHT_TYPE_DIRECTIONAL :
		return (m_pLightList_Dir_Dynamic->size()+m_pLightList_Dir_Static->size());		
		break;
	case NOISE_LIGHT_TYPE_POINT :
		return (m_pLightList_Point_Dynamic->size()+m_pLightList_Point_Static->size());		
		break;
	case NOISE_LIGHT_TYPE_SPOT :
		return (m_pLightList_Spot_Dynamic->size()+m_pLightList_Spot_Static->size());		
		break;
	}
	return 0;
};

UINT	 NoiseLightManager::GetTotalLightCount()
{
	return m_pLightList_Dir_Dynamic->size()+m_pLightList_Dir_Static->size()+
			m_pLightList_Point_Dynamic->size()+m_pLightList_Point_Static->size() + 
			m_pLightList_Spot_Dynamic->size()+m_pLightList_Spot_Static->size();
};


/***********************************************************************
								PRIVATE					                    
***********************************************************************/
template <typename T> 
void NoiseLightManager::mFunction_AddLight(std::vector<T>* pList, T iLight,UINT maxElementCount)
{
	std::vector<T>::iterator tmp_iter; 
		for(tmp_iter = pList->begin();	tmp_iter != pList->end(); tmp_iter++) 
		{ 
		//找到重复的灯光就不add了
		if(*tmp_iter == iLight) return; 
		}; 
		if ( pList->size() < maxElementCount) pList->push_back(iLight); 
};

template <typename T> 
void NoiseLightManager::mFunction_RemoveLight_ByAddr(std::vector<T>* pList, T DelLight)
{
	std::vector<T>::iterator tmp_iter; 
	for(tmp_iter = pList->begin();	tmp_iter != pList->end(); tmp_iter++) 
	{ 
	//把vector当前位置的空位和内容都抹掉
	if(*tmp_iter == DelLight){tmp_iter = pList->erase(tmp_iter);break;} 
	}; 
};

template <typename T> 
void NoiseLightManager::mFunction_RemoveLight_ByIndex(std::vector<T>* pList, UINT lightIndex)
{
	std::vector<T>::iterator tmp_iter = pList->begin(); 
		if(pList->size() > lightIndex) 
		{ 
			tmp_iter += lightIndex; 
			pList->erase(tmp_iter);  
		} 
};
