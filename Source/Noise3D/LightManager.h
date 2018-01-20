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

	class /*_declspec(dllexport)*/ ILightManager:
		IFactory<IDirLightD>,
		IFactory<IPointLightD>,
		IFactory<ISpotLightD>,
		IFactory<IDirLightS>,
		IFactory<IPointLightS>,
		IFactory<ISpotLightS>
	{
	public:

		IDirLightD*			CreateDynamicDirLight(N_UID lightName);

		IPointLightD*		CreateDynamicPointLight(N_UID lightName);

		ISpotLightD*		CreateDynamicSpotLight(N_UID lightName);

		IDirLightS*			CreateStaticDirLight(N_UID lightName,const N_DirLightDesc& desc);

		IPointLightS*		CreateStaticPointLight(N_UID lightName, const N_PointLightDesc& desc);

		ISpotLightS*		CreateStaticSpotLight(N_UID lightName, const N_SpotLightDesc& desc);

		IDirLightD*			GetDirLightD(N_UID lightName);

		IDirLightD*			GetDirLightD(UINT index);

		IPointLightD*		GetPointLightD(N_UID lightName);
		
		IPointLightD*		GetPointLightD(UINT index);

		ISpotLightD*		GetSpotLightD(N_UID lightName);
		
		ISpotLightD*		GetSpotLightD(UINT index);

		IDirLightS*			GetDirLightS(N_UID lightName);
		
		IDirLightS*			GetDirLightS(UINT index);

		IPointLightS*		GetPointLightS(N_UID lightName);
		
		IPointLightS*		GetPointLightS(UINT index);

		ISpotLightS*		GetSpotLightS(N_UID lightName);
		
		ISpotLightS*		GetSpotLightS(UINT index);



		bool		DeleteDirLightD(N_UID lightName);

		bool		DeleteDirLightD(IDirLightD* pLight);

		bool		DeletePointLightD(N_UID lightName);

		bool		DeletePointLightD(IPointLightD* pLight);

		bool		DeleteSpotLightD(N_UID lightName);

		bool		DeleteSpotLightD(ISpotLightD* pLight);

		bool		DeleteDirLightS(N_UID lightName);

		bool		DeleteDirLightS(IDirLightS* pLight);

		bool		DeletePointLightS(N_UID lightName);

		bool		DeletePointLightS(IPointLightS* pLight);

		bool		DeleteSpotLightS(N_UID lightName);

		bool		DeleteSpotLightS(ISpotLightS* pLight);


		void			SetDynamicLightingEnabled(bool isEnabled);

		//void		SetStaticLightingEnabled(bool isEnabled);

		bool			IsDynamicLightingEnabled();

		//bool		IsStaticLightingEnabled();

		UINT		GetLightCount(NOISE_LIGHT_TYPE lightType);

		UINT		GetDynamicLightCount();

		UINT		GetStaticLightCount();

		UINT		GetTotalLightCount();


	private:

		friend  IRenderer;//access to 'CanUpdateStaticLights'

		friend IFactory<ILightManager>;

		//构造函数
		ILightManager();

		~ILightManager();

		bool		mIsDynamicLightingEnabled;
		bool		mIsStaticLightingEnabled;
		bool		mCanUpdateStaticLights;

	};
}