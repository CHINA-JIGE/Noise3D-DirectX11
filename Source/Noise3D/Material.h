/***********************************************************************

                           h£ºIMaterial & Desc

************************************************************************/
#pragma once

namespace Noise3D
{
	//basic Desc (that can be upload to CONSTANT BUFFER
	struct N_BasicMaterialDesc
	{
		N_BasicMaterialDesc()
		{
			ZeroMemory(this, sizeof(*this));
			mAmbientColor = NVECTOR3(0, 0, 0);
			mDiffuseColor = NVECTOR3(0.1f, 0.1f, 0.1f);
			mSpecularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
			mSpecularSmoothLevel = 10;
			mNormalMapBumpIntensity = 0.1f;
			mEnvironmentMapTransparency = 0.1f;
		}

		//base attribute offset that will be added to final color
		NVECTOR3	mAmbientColor;		INT32	mSpecularSmoothLevel;
		NVECTOR3	mDiffuseColor;		float		mNormalMapBumpIntensity;
		NVECTOR3	mSpecularColor;		float		mEnvironmentMapTransparency;

	};

	//Material Description including texture info (texture part will be upload to GraphicMem in the
	//form of shader resource
	struct N_MaterialDesc :
		public N_BasicMaterialDesc
	{
		N_MaterialDesc() :
			diffuseMapName(""),
			normalMapName(""),
			specularMapName(""),
			environmentMapName("")
		{ };

		N_UID diffuseMapName;
		N_UID normalMapName;
		N_UID specularMapName;
		N_UID environmentMapName;
	};



	class IMaterial
	{
	public:

		//BASE means that these constant color will be added to texture-based color
		void	SetBaseAmbientColor(const NVECTOR3& color);

		void	SetBaseDiffuseColor(const NVECTOR3& color);

		void	SetBaseSpecularColor(const NVECTOR3& color);

		//0 to 100
		void	SetSpecularSmoothLevel(int level);

		//0 to 1
		void	SetNormalMapBumpIntensity(float normalized_intensity);

		//0 to 1
		void	SetEnvironmentMappingTransparency(float transparency);

		void	SetDiffuseMap(const N_UID& diffMapName);

		void SetNormalMap(const N_UID& normMapName);

		void	SetSpecularMap(const N_UID& specMapName);

		void SetEnvMap(const N_UID& environmentMapName);

		void SetDesc(const N_MaterialDesc& desc);

		void	GetDesc(N_MaterialDesc& outDesc);

	private:

		friend class IFactory<IMaterial>;

		IMaterial();

		~IMaterial();

		N_MaterialDesc mMatDesc;
	};


};