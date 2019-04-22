/***********************************************************************

                           h: Advanced material for GI Renderer

************************************************************************/
#pragma once

namespace Noise3D
{
	namespace GI
	{
		struct N_AdvancedMatDesc
		{
			N_AdvancedMatDesc() :
				albedo(1.0f, 1.0f, 1.0f, 1.0),
				roughness(0.0f),
				metallicity(0.0f),
				transparency(0.0f),
				ior(1.1f),
				metal_F0(0.04f,0.04f,0.04f),
				emission(0, 0, 0) {};

			//(2019.4.14)metalness, transparency shall be 3D parameters that affect transimission ray throughout the object
			//but for simplicity, we only use 2D texture to modulate material param ON THE SURFACE
			//(just modulate the surface). if we want to modulate the whole object volume, 3D Texture and 
			//volume render wil be required, which is far too complicated.
			//so we now assume: many

			//surface(?) param, describing object's diffuse(local SSS) absorbance of dielectric part
			Color4f albedo;

			//surface param, Cook-Torrance's microfacet model's param of D & G 
			float roughness;

			//volume param, describing object's internal dielectric volume absorbance
			//(partially absorb what's left when taken away diffuse and reflection)
			float transparency;

			//volume param, describing object's percentage of 'being metal'(otherwise dielectric)
			float metallicity;

			//index of refraction (related to F0)
			float ior;

			//Fresnel Reflectance of normal incident, F0 (related to ior)
			Vec3 metal_F0;

			//surface param, color and intensity of emission (HDR)
			Vec3 emission;

			//(2019.4.13) texture maps will implemented later
			//Texture2D* pAlbedoMap;
			//Texture2D* pRoughnessMap;
			//Texture2D* pMetallicityMap;//if transmission is enabled, this will be disabled.
			//Texture2D* pEmissiveMap;

		};

		//material specifically for PathTracer
		class AdvancedGiMaterial
		{
		public:

			//optimization will be made for PathTracer, in which texture ptr are access frequently.
			//textures are access directly throught ptr.

			//(2019.4.5)one day, reference counting of Texture/Material should be implemented.
			//and those who owns resource X will be inherited from sth like IResourceOwner<X>
			void SetAlbedo(Color4f albedo);

			void SetRoughness(float r);

			void SetOpacity(float o);

			void SetMetallicity(float m);

			void SetEmission(Vec3 hdrEmission);

			void SetMetalF0(Vec3 F0);

			void SetRefractiveIndex(float ior);//also update F0

			bool IsTransmissionEnabled();

			bool IsEmissionEnabled();//true for light source

			void SetDesc(const N_AdvancedMatDesc& desc);

			const N_AdvancedMatDesc& GetDesc();

			//preset material
			void Preset_PerfectGlass(float ior=2.0f);

		private:

			float mFunc_IorToF0(float ior);

			N_AdvancedMatDesc mMatDesc;
		};

		//ut interface for path tracer
		class IAdvancedGiMaterialOwner
		{
		public:

			//(2019.4.5)material for GI renderer, for now doesn't support multiple GI mat for one mesh
			void	SetGiMaterial(GI::AdvancedGiMaterial* pMat);

			GI::AdvancedGiMaterial* GetGiMaterial();

		protected:

			IAdvancedGiMaterialOwner();

			~IAdvancedGiMaterialOwner();


		private:

			GI::AdvancedGiMaterial* m_pGiMat;

		};

	}
}