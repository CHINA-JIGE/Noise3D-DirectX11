/***********************************************************************

                           h£ºILight interafces && lightDesc 

************************************************************************/
#pragma once

namespace Noise3D
{
	//LIGHT description : just combination of data
	//LIGHT interfaces : provide interfaces for the user to interact


	struct N_CommonLightDesc
	{
		N_CommonLightDesc() { ZeroMemory(this, sizeof(*this)); }
		NVECTOR3	ambientColor;		float				specularIntensity;
		NVECTOR3	diffuseColor;		float				diffuseIntensity;
		NVECTOR3	specularColor;	//4 bytes left to pad to fulfill 128 bytes alignment
	};


	//don't forget graphic memory's 128 bit alignment
	struct N_DirLightDesc
		:public N_CommonLightDesc
	{
		N_DirLightDesc() { ZeroMemory(this, sizeof(*this)); };

		/*NVECTOR3	ambientColor;		float				specularIntensity;
		NVECTOR3	diffuseColor;				float				diffuseIntensity;
		NVECTOR3	specularColor;*/		float		mPad2;
		NVECTOR3 direction;				float		mPad3;
	};


	//don't forget graphic memory's 128 bit alignment
	struct N_PointLightDesc 
		:public N_CommonLightDesc
	{
		N_PointLightDesc() {ZeroMemory(this, sizeof(*this));}

		/*NVECTOR3	ambientColor;		float				specularIntensity;
		NVECTOR3	diffuseColor;				float				diffuseIntensity;
		NVECTOR3	specularColor;*/		float		mAttenuationFactor;
		NVECTOR3 mPosition;						float		mLightingRange;

	};


	//don't forget graphic memory's 128 bit alignment
	struct N_SpotLightDesc
		:public N_CommonLightDesc
	{
		N_SpotLightDesc(){ZeroMemory(this, sizeof(*this));}

		/*NVECTOR3 ambientColor;		float specularIntensity;
		NVECTOR3 diffuseColor;			float diffuseIntensity;
		NVECTOR3 specularColor;*/	float mAttenuationFactor;
		NVECTOR3 mLitAt;					float mLightingAngle;
		NVECTOR3 mPosition;			float mLightingRange;
	};


	//----------------------BASE LIGHT-------------------------------
	class IBaseLight
	{
	public:

		IBaseLight();

		void SetAmbientColor(const NVECTOR3& color);

		void SetDiffuseColor(const NVECTOR3& color);

		void SetSpecularColor(const NVECTOR3& color);

		void SetSpecularIntensity(float specInt);

		void SetDiffuseIntensity(float diffInt);


	protected:

		//invoked by derived Light,not by user
		void	SetDesc(const N_CommonLightDesc& desc);

		//invoked by derived Light,not by user
		void GetDesc(N_CommonLightDesc& outDesc);

	private:
		N_CommonLightDesc mBaseLightDesc;

	};


	//---------------------Dynamic Directional Light------------------
	class DirLight : 
		public IBaseLight,
		public IShadowCaster//container of DSV of shadow map
	{
	public:

		void	SetDirection(const NVECTOR3& dir);

		void SetDesc(const N_DirLightDesc& desc);//many CLAMP op happens in this

		N_DirLightDesc GetDesc();

	protected:

		//override SM init function. invoked by LightManager
		virtual bool mFunction_InitShadowMap(SHADOW_MAP_PROJECTION_TYPE type, N_SHADOW_MAPPING_PARAM smParam) override;

	private:

		friend class LightManager;//to init
		friend IFactory<DirLight>;

		DirLight();

		~DirLight();

		N_DirLightDesc mLightDesc;


	};


	//-----------------------Dynamic Point Light--------------------
	class PointLight : 
		public IBaseLight
		//shadow map init not implemented
	{
	public:

		void SetPosition(const NVECTOR3& pos);

		void SetAttenuationFactor(float attFactor);

		void	SetLightingRange(float range);

		void SetDesc(const N_PointLightDesc& desc);//many CLAMP op happens in this

		N_PointLightDesc GetDesc();

	private:

		friend IFactory<PointLight>;

		PointLight();

		~PointLight();

		N_PointLightDesc mLightDesc;
	};


	//-----------------------Dynamic Spot Light------------------
	class SpotLight:
		public IBaseLight
	{
	public:

		void SetPosition(const NVECTOR3& pos);

		void SetAttenuationFactor(float attFactor);

		void	SetLitAt(const NVECTOR3& vLitAt);

		void	SetLightingAngle(float coneAngle_Rad);

		void	SetLightingRange(float range);

		void SetDesc(const N_SpotLightDesc& desc);//many CLAMP op happens in this

		N_SpotLightDesc GetDesc();

	private:

		friend IFactory<SpotLight>;

		SpotLight();

		~SpotLight();

		N_SpotLightDesc mLightDesc;

	};


};