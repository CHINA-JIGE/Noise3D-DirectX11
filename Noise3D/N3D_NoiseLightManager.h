/***********************************************************************

                           h£ºNoiseLightManager

************************************************************************/

#pragma once


public class _declspec(dllexport) NoiseLightManager
{
public:
	friend NoiseScene;
	friend NoiseRenderer;

	//¹¹Ôìº¯Êý
	NoiseLightManager();

	void		SelfDestruction();

	void		AddDynamicDirLight(N_DirectionalLight* pLight);

	void		AddDynamicPointLight(N_PointLight* pLight);

	void		AddDynamicSpotLight(N_SpotLight* pLight);

	void		RemoveDynamicDirLight(N_DirectionalLight* pLight);

	void		RemoveDynamicPointLight(N_PointLight* pLight);

	void		RemoveDynamicSpotLight(N_SpotLight* pLight);

	void		SetDynamicLightingEnabled(BOOL isEnabled);


	void		AddStaticDirLight(N_DirectionalLight iLight);

	void		AddStaticPointLight(N_PointLight iLight);

	void		AddStaticSpotLight(N_SpotLight iLight);

	void		RemoveStaticDirLight(UINT iLight_Index);

	void		RemoveStaticPointLight(UINT iLight_Index);

	void		RemoveStaticSpotLight(UINT iLight_Index);

	void		SetStaticLightingEnabled(BOOL isEnabled);

	UINT		GetLightCount(NOISE_LIGHT_TYPE lightType);

	UINT		GetTotalLightCount();


private:
	template <typename T> 
	void mFunction_AddLight(std::vector<T>* pList, T iLight,UINT maxElementCount);

	template <typename T> 
	void mFunction_RemoveLight_ByAddr(std::vector<T>* pList, T DelLight);

	template <typename T> 
	void mFunction_RemoveLight_ByIndex(std::vector<T>* pList, UINT lightIndex);


private:
	NoiseScene*		m_pFatherScene;

	std::vector<N_DirectionalLight*>*	m_pLightList_Dir_Dynamic;
	std::vector<N_PointLight*>*			m_pLightList_Point_Dynamic;
	std::vector<N_SpotLight*>*			m_pLightList_Spot_Dynamic;
	BOOL		mIsDynamicLightingEnabled;

	std::vector<N_DirectionalLight>*	m_pLightList_Dir_Static;
	std::vector<N_PointLight>*			m_pLightList_Point_Static;
	std::vector<N_SpotLight>*			m_pLightList_Spot_Static;
	BOOL		mIsStaticLightingEnabled;
	BOOL		mCanUpdateStaticLights;

};
