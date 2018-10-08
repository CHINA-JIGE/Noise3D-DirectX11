/***********************************************************************

                           h：NoiseLightManager

************************************************************************/

#pragma once

namespace Noise3D
{
	//灯光类型
	enum NOISE_LIGHT_TYPE
	{
		NOISE_LIGHT_TYPE_DYNAMIC_DIR = 0,
		NOISE_LIGHT_TYPE_DYNAMIC_POINT = 1,
		NOISE_LIGHT_TYPE_DYNAMIC_SPOT = 2,
		NOISE_LIGHT_TYPE_STATIC_DIR = 3,
		NOISE_LIGHT_TYPE_STATIC_POINT = 4,
		NOISE_LIGHT_TYPE_STATIC_SPOT = 5
	};

	class /*_declspec(dllexport)*/ LightManager:
		IFactory<DirLightD>,
		IFactory<PointLightD>,
		IFactory<SpotLightD>,
		IFactory<DirLightS>,
		IFactory<PointLightS>,
		IFactory<SpotLightS>
	{
	public:

		DirLightD*			CreateDynamicDirLight(N_UID lightName);

		PointLightD*		CreateDynamicPointLight(N_UID lightName);

		SpotLightD*		CreateDynamicSpotLight(N_UID lightName);

		DirLightS*			CreateStaticDirLight(N_UID lightName,const N_DirLightDesc& desc);

		PointLightS*		CreateStaticPointLight(N_UID lightName, const N_PointLightDesc& desc);

		SpotLightS*		CreateStaticSpotLight(N_UID lightName, const N_SpotLightDesc& desc);

		DirLightD*			GetDirLightD(N_UID lightName);

		DirLightD*			GetDirLightD(UINT index);

		PointLightD*		GetPointLightD(N_UID lightName);
		
		PointLightD*		GetPointLightD(UINT index);

		SpotLightD*		GetSpotLightD(N_UID lightName);
		
		SpotLightD*		GetSpotLightD(UINT index);

		DirLightS*			GetDirLightS(N_UID lightName);
		
		DirLightS*			GetDirLightS(UINT index);

		PointLightS*		GetPointLightS(N_UID lightName);
		
		PointLightS*		GetPointLightS(UINT index);

		SpotLightS*		GetSpotLightS(N_UID lightName);
		
		SpotLightS*		GetSpotLightS(UINT index);



		bool		DeleteDirLightD(N_UID lightName);

		bool		DeleteDirLightD(DirLightD* pLight);

		bool		DeletePointLightD(N_UID lightName);

		bool		DeletePointLightD(PointLightD* pLight);

		bool		DeleteSpotLightD(N_UID lightName);

		bool		DeleteSpotLightD(SpotLightD* pLight);

		bool		DeleteDirLightS(N_UID lightName);

		bool		DeleteDirLightS(DirLightS* pLight);

		bool		DeletePointLightS(N_UID lightName);

		bool		DeletePointLightS(PointLightS* pLight);

		bool		DeleteSpotLightS(N_UID lightName);

		bool		DeleteSpotLightS(SpotLightS* pLight);


		void			SetDynamicLightingEnabled(bool isEnabled);

		bool			IsDynamicLightingEnabled();

		UINT		GetLightCount(NOISE_LIGHT_TYPE lightType);

		UINT		GetDynamicLightCount();

		UINT		GetStaticLightCount();

		UINT		GetTotalLightCount();


	private:

		friend  Renderer;//access to 'CanUpdateStaticLights'

		friend IFactory<LightManager>;

		//构造函数
		LightManager();

		~LightManager();

		bool		mIsDynamicLightingEnabled;
		bool		mIsStaticLightingEnabled;
		bool		mCanUpdateStaticLights;

	};
}