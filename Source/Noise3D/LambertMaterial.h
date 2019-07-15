/***********************************************************************

                           h£ºMaterial & Desc

************************************************************************/
#pragma once

namespace Noise3D
{
	//When structure of material is modified, several things should be carefully
	//dealt with:
	//1, Set methods of Material
	//2, SetDesc method of Material
	//3, N_CbPerSubset
	//4, structure in shaders

	//basic Desc (that can be upload to CONSTANT BUFFER
	struct N_BasicLambertMaterialDesc
	{
		N_BasicLambertMaterialDesc()
		{
			ambientColor = Vec3(0, 0, 0);
			diffuseColor = Vec3(0.1f, 0.1f, 0.1f);
			specularColor = Vec3(1.0f, 1.0f, 1.0f);
			specularSmoothLevel = 10;
			normalMapBumpIntensity = 0.1f;
			environmentMapTransparency = 0.1f;
			transparency = 1.0f;
		}
		
		//base attribute offset that will be added to final color
		Vec3	ambientColor;		int32_t	specularSmoothLevel;
		Vec3	diffuseColor;		float		normalMapBumpIntensity;
		Vec3	specularColor;		float		environmentMapTransparency;
		float	transparency;		int32_t	pad1; int32_t pad2; int32_t pad3;

	};

	//Material Description including texture info (texture part will be upload to GraphicMem in the
	//form of shader resource
	struct N_LambertMaterialDesc :
		public N_BasicLambertMaterialDesc
	{
		N_LambertMaterialDesc() :
			diffuseMapName(""),
			normalMapName(""),
			specularMapName(""),
			emissiveMapName(""),
			environmentMapName("")
		{ };

		N_LambertMaterialDesc(const N_BasicLambertMaterialDesc& basicDesc) :
			N_BasicLambertMaterialDesc(basicDesc),
			diffuseMapName(""),
			normalMapName(""),
			specularMapName(""),
			emissiveMapName(""),
			environmentMapName("")
		{
		};

		N_UID diffuseMapName;
		N_UID normalMapName;
		N_UID specularMapName;
		N_UID emissiveMapName;//(2018.1.20)not supported yet
		N_UID environmentMapName;
	};

	class LambertMaterial
	{
	public:

		//name
		void	SetAmbientColor(const Vec3& color);

		//name
		void	SetDiffuseColor(const Vec3& color);

		//name
		void	SetSpecularColor(const Vec3& color);

		//0 to 100
		void	SetSpecularSmoothLevel(int level);

		//0 to 1
		void	SetNormalMapBumpIntensity(float normalized_intensity);

		//0 to 1
		void	SetEnvironmentMappingTransparency(float transparency);

		//0 to 1
		void SetTransparency(float transparency);

		//string
		void	SetDiffuseMap(const N_UID& diffMapName);

		//string
		void SetNormalMap(const N_UID& normMapName);

		//string
		void	SetSpecularMap(const N_UID& specMapName);

		//string
		void SetEnvMap(const N_UID& environmentMapName);

		void SetDesc(const N_LambertMaterialDesc& desc);

		void	GetDesc(N_LambertMaterialDesc& outDesc);

	private:

		friend class IFactory<LambertMaterial>;

		LambertMaterial();

		~LambertMaterial();

		N_LambertMaterialDesc mMatDesc;
	};

};