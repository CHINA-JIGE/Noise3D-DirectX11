/***********************************************************************

                           h£ºNoiseLightManager

************************************************************************/

#pragma once

namespace Noise3D
{
	class DirLight;
	class PointLight;
	class SpotLight;

	class /*_declspec(dllexport)*/ LightManager:
		public IFactoryEx<DirLight, PointLight, SpotLight>
	{
	public:

		DirLight*		CreateDynamicDirLight(SceneNode* pAttachedNode, N_UID lightName, N_SHADOW_MAPPING_PARAM smParam = N_SHADOW_MAPPING_PARAM());

		PointLight*	CreateDynamicPointLight(SceneNode* pAttachedNode, N_UID lightName);

		SpotLight*	CreateDynamicSpotLight(SceneNode* pAttachedNode, N_UID lightName);

		void		SetDynamicLightingEnabled(bool isEnabled);

		bool		IsDynamicLightingEnabled();

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