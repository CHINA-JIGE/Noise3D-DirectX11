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
				opacity(0.0f),
				emission(0, 0, 0) {};

			//(2019.4.14)metalness, opacity shall be 3D parameters that affect transimission ray throughout the object
			//but for simplicity, we only use 2D texture to modulate material param ON THE SURFACE
			//(just modulate the surface). if we want to modulate the whole object volume, 3D Texture and 
			//volume render wil be required, which is far too complicated.
			//so we now assume: many

			//surface(?) param, describing object's diffuse(local SSS) absorbance
			Color4f albedo;

			//surface param, Cook-Torrance's microfacet model's param of D & G 
			float roughness;

			//volume param, describing object's internal volume absorbance
			//(partially absorb what's left when taken away diffuse and reflection)
			float opacity;

			//volume param, describing object's percentage of 'being metal'(otherwise dielectric)
			float metallicity;

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

			bool IsTransmissionEnabled();

			bool IsEmissionEnabled();//true for light source

			void SetDesc(const N_AdvancedMatDesc& desc);

			N_AdvancedMatDesc& GetDesc();

		private:

			N_AdvancedMatDesc mMatDesc;
		};
	}
}