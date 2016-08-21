/***********************************************************************

                           h£ºNoiseLightManager

************************************************************************/

#pragma once

namespace Noise3D
{

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

		ISpotLightS*			CreateStaticSpotLight(N_UID lightName, const N_SpotLightDesc& desc);

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



		BOOL		DeleteDirLightD(N_UID lightName);

		BOOL		DeleteDirLightD(IDirLightD* pLight);

		BOOL		DeletePointLightD(N_UID lightName);

		BOOL		DeletePointLightD(IPointLightD* pLight);

		BOOL		DeleteSpotLightD(N_UID lightName);

		BOOL		DeleteSpotLightD(ISpotLightD* pLight);

		NOISE_LIGHT_TYPE a;
		BOOL		DeleteDirLightS(N_UID lightName);

		BOOL		DeleteDirLightS(IDirLightS* pLight);

		BOOL		DeletePointLightS(N_UID lightName);

		BOOL		DeletePointLightS(IPointLightS* pLight);

		BOOL		DeleteSpotLightS(N_UID lightName);

		BOOL		DeleteSpotLightS(ISpotLightS* pLight);


		void			SetDynamicLightingEnabled(BOOL isEnabled);

		void			SetStaticLightingEnabled(BOOL isEnabled);

		UINT		GetLightCount(NOISE_LIGHT_TYPE lightType);

		UINT		GetDynamicLightCount();

		UINT		GetStaticLightCount();

		UINT		GetTotalLightCount();


	private:

		friend  IRenderer;//access to 'CanUpdateStaticLights'

		friend IFactory<ILightManager>;

		//¹¹Ôìº¯Êý
		ILightManager();

		~ILightManager();

		BOOL		mIsDynamicLightingEnabled;
		BOOL		mIsStaticLightingEnabled;
		BOOL		mCanUpdateStaticLights;

	};
}