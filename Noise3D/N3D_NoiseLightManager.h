/***********************************************************************

                           h：NoiseLightManager

************************************************************************/

#pragma once


struct N_DirectionalLight
{
	N_DirectionalLight()
	{
		ZeroMemory(this, sizeof(*this));
		mSpecularIntensity = 1.0f;
		mDirection = NVECTOR3(1.0f, 0, 0);
		mDiffuseIntensity = 0.5;
	}
	BOOL operator==(N_DirectionalLight& Light)
	{
		//two memory fragments are identical
		if (memcmp(this, &Light, sizeof(Light)) == 0)
		{
			return TRUE;
		}
		return FALSE;
	}

	NVECTOR3 mAmbientColor;	 float		mSpecularIntensity;
	NVECTOR3 mDiffuseColor;	float			mDiffuseIntensity;
	NVECTOR3 mSpecularColor;	 float		mPad2;//用于内存对齐
	NVECTOR3 mDirection;			 float		mPad3;//用于内存对齐
};

struct N_PointLight
{
	N_PointLight() {
		ZeroMemory(this, sizeof(*this));
		mSpecularIntensity = 1.0f;
		mAttenuationFactor = 0.05f;
		mLightingRange = 100.0f;
		mDiffuseIntensity = 0.5;
	}

	BOOL operator==(N_PointLight& Light)
	{
		//two memory fragments are identical
		if (memcmp(this, &Light, sizeof(Light)) == 0)
		{
			return TRUE;
		}
		return FALSE;
	}

	NVECTOR3 mAmbientColor;		float mSpecularIntensity;
	NVECTOR3 mDiffuseColor;		float mLightingRange;
	NVECTOR3 mSpecularColor;		float mAttenuationFactor;
	NVECTOR3 mPosition;				float mDiffuseIntensity;//memory alignment

};

struct N_SpotLight
{
	N_SpotLight()
	{
		ZeroMemory(this, sizeof(*this));
		mSpecularIntensity = 1.0f;
		mAttenuationFactor = 1.0f;
		mLightingRange = 100.0f;
		mLightingAngle = MATH_PI / 4;
		mDiffuseIntensity = 0.5;
		mLitAt = NVECTOR3(0, 0, 0);
	}

	BOOL operator==(N_SpotLight& Light)
	{
		//two memory fragments are identical
		if (memcmp(this, &Light, sizeof(Light)) == 0)
		{
			return TRUE;
		}
		return FALSE;
	}


	NVECTOR3 mAmbientColor;		float mSpecularIntensity;
	NVECTOR3 mDiffuseColor;		float mLightingRange;
	NVECTOR3 mSpecularColor;		float mAttenuationFactor;
	NVECTOR3 mLitAt;					float mLightingAngle;
	NVECTOR3 mPosition;				float mDiffuseIntensity;
};


class _declspec(dllexport) NoiseLightManager:public NoiseClassLifeCycle
{
public:
	friend NoiseScene;
	friend NoiseRenderer;

	//构造函数
	NoiseLightManager();

	void		AddDynamicDirLight(N_DirectionalLight& refLight);

	void		AddDynamicPointLight(N_PointLight& refLight);

	void		AddDynamicSpotLight(N_SpotLight& refLight);

	void		RemoveDynamicDirLight(N_DirectionalLight& refLight);

	void		RemoveDynamicPointLight(N_PointLight& refLight);

	void		RemoveDynamicSpotLight(N_SpotLight& refLight);

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

	void		Destroy();

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
