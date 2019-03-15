/***********************************************************************

                           h£ºNoiseLightManager

************************************************************************/

#pragma once

namespace Noise3D
{

	class /*_declspec(dllexport)*/ LightManager:
		protected IFactory<DirLight>,
		protected IFactory<PointLight>,
		protected IFactory<SpotLight>
	{
	public:

		DirLight*			CreateDynamicDirLight(SceneNode* pAttachedNode, N_UID lightName, N_SHADOW_MAPPING_PARAM smParam = N_SHADOW_MAPPING_PARAM());

		PointLight*		CreateDynamicPointLight(SceneNode* pAttachedNode, N_UID lightName);

		SpotLight*		CreateDynamicSpotLight(SceneNode* pAttachedNode, N_UID lightName);

		//supported template param: DirLight, PointLight, SpotLight
		template <typename T> T* GetLight(N_UID lightName);

		//supported template param: DirLight, PointLight, SpotLight
		template <typename T> T* GetLight(uint32_t index);

		//supported template param: DirLight, PointLight, SpotLight
		template <typename T> bool DeleteLight(N_UID lightName);

		//supported template param: DirLight, PointLight, SpotLight
		template <typename T> bool DeleteLight(T* pLight);

		void		SetDynamicLightingEnabled(bool isEnabled);

		bool		IsDynamicLightingEnabled();

		//supported template param: DirLight, PointLight, SpotLight
		template <typename T> uint32_t GetLightCount();

		UINT	GetTotalLightCount();


	private:

		friend  Renderer;//access to 'CanUpdateStaticLights'

		friend IFactory<LightManager>;

		LightManager();

		~LightManager();

		//iterate all ShadowCaster base classes of Lights
		//and pass/delegate them to the Renderer (?) to do d3d-based render stuffs?
		void	mFunction_GetShadowMapRenderTaskList();

		bool		mIsDynamicLightingEnabled;
		bool		mCanUpdateStaticLights;

	};
}