
/***********************************************************

			GI: path tracer's STANDARD soft shader

**************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		class PathTracerStandardShader :
			public IPathTracerSoftShader
		{
		public:

			PathTracerStandardShader();

			void SetSkyLightMultiplier(float multiplier);

			void SetSkyLightType(NOISE_PATH_TRACER_SKYLIGHT_TYPE type);

			void SetSkyDomeTexture(Texture2D* pTex, bool computeSH16 =true);

			void SetSkyBoxTexture(TextureCubeMap* pTex, bool computeSH16 = true);

			virtual void ClosestHit(const N_TraceRayParam& param, const N_RayHitInfoForPathTracer& hitInfo, N_TraceRayPayload& in_out_payload) override;

			virtual void Miss(const N_TraceRayParam & param, N_TraceRayPayload& in_out_payload) override;

		private:
			enum
			{
				BxDF_LightTransfer_Diffuse = 0x00000001,
				BxDF_LightTransfer_Specular = 0x00000002,
				BxDF_LightTransfer_Transmission = 0x00000004
			};

			struct BxdfInfo
			{
				//vary over samples
				Vec3 k_d;//BxDF ratio is related to wave length
				Vec3 k_s;
				Vec3 k_t;
				Vec3 diffuseBRDF;
				float reflectionBRDF;//k_s/ Fresnel will be multiplied later
				//float reflectionBrdfDividedByD;//cancelled D with GGX importance sampling's pdf's D term
				float transmissionBTDF;
			};

			//final integration of all results of integration (blend 3 BxDF according to the ratio of sample count/variance)
			GI::Radiance _FinalIntegration(const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo);

			//integration of  mul(light sources direct light,  diffuse BRDF)
			//void _IntegrateDiffuseDirectLighting(int samplesPerLight, const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, GI::Radiance& outDiffuse);

			//integration of  mul(non light sources indirect light,  diffuse BRDF)
			//(additional sample for indirect lighting, less & sparser samples ; SH vector will be used in indirect diffusion )
			void _IntegrateDiffuse(int sampleCount, const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, GI::Radiance& outDiffuse);

			 //integration of mul(direct& indirect lights together ,refraction BTDF)
			//(samples for refraction/transmission; might have importance sampling)
			void _IntegrateTransmission(int samplesCount, const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, GI::Radiance& outTransmission);

			//integration of mul(direct& indirect lights together, specular reflection)
			//(additional samples for reflections; might have importance sampling)
			void _IntegrateSpecular(int samplesCount, const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, GI::Radiance& outReflection);

			//eval a BxDF and its coefficients given light transfer type
			void _CalculateBxDF(uint32_t lightTransferType, Vec3 lightDir, Vec3 viewDir, const N_RayHitInfoForPathTracer & hitInfo, BxdfInfo& outBxdfInfo);

			Vec3 _DiffuseBRDF(Vec3 albedo, Vec3 l, Vec3 v, Vec3 n, float alpha);

			//Fresnel term is not involved here. it's in k_s
			float _SpecularReflectionBRDF(Vec3 l, Vec3 v, Vec3 n, float D, float G);//microfacet reflection

			float _SpecularReflectionBrdfDividedByD(Vec3 l, Vec3 v, Vec3 n, float G);

			float _SpecularTransmissionBTDF(Vec3 l, Vec3 v, Vec3 n, float D, float G);//microfacet refraction/transmission

			float _RoughnessToAlpha(float r);

			NOISE_PATH_TRACER_SKYLIGHT_TYPE mSkyLightType;

			Texture2D* m_pSkyDomeTex;

			TextureCubeMap* m_pSkyBoxTex;

			SHVector mShVecSky;//SH vector of Env Lighting (env diffuse)
			
			float mSkyLightMultiplier;//radiance scale factor
		};
	}
}
