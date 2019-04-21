
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

			void SetSkyType(NOISE_ATMOSPHERE_SKYTYPE type);

			void SetSkyDomeTexture(Texture2D* pTex, bool computeSH16 =true);

			void SetSkyBoxTexture(TextureCubeMap* pTex, bool computeSH16 = true);

			virtual void ClosestHit(const N_TraceRayParam& param, const N_RayHitInfoForPathTracer& hitInfo, N_TraceRayPayload& in_out_payload) override;

			virtual void Miss(N_Ray ray, N_TraceRayPayload& in_out_payload) override;

		private:

			//final integration of all results of integration (blend 3 BxDF according to the ratio of sample count/variance)
			GI::Radiance _FinalIntegration();

			//integration of  mul(light sources direct light,  complete BSDF)
			GI::Radiance _IntegrateOverLightSources(int samplesPerLight);

			 //integration of mul(non-light source indirect ligh, reflection BRDF+refraction BTDF)
			//(additional sample for indirect lighting, less sample; might have importance sampling)
			void _IntegrateSpecularBsdf(int samplesCount, GI::Radiance& outReflection, GI::Radiance& outTransmission);

			//integration of mul(non-light source indirect light, diffuse BRDF)
			//(additional sample for indirect lighting of diffusion, SH vector will be used. )
			GI::Radiance _IntegrateDiffuseBrdf(int samplesCount);

			Vec3 _CompleteBsdf(const N_Ray& newSampleRay, const N_TraceRayParam& hitRayParam, const N_RayHitInfoForPathTracer & hitInfo);

			Vec3 _DiffuseBrdf(Color4f albedo, Vec3 l, Vec3 v, Vec3 n, float alpha);

			void _SpecularBsdf(Vec3 l, Vec3 v, Vec3 n, Vec3& outFresnel, Vec3& outReflectionBrdf, Vec3& outTransmissionBtdf);//microfacet reflection

			//Vec3 _SpecularTransmissionBtdf(Vec3 l, Vec3 v, Vec3 n, float D, float G, Vec3 F);//microfacet refraction/transmission

			NOISE_ATMOSPHERE_SKYTYPE mSkyType;

			Texture2D* m_pSkyDomeTex;

			TextureCubeMap* m_pSkyBoxTex;

			SHVector mShVecSky;//SH vector of Env Lighting (env diffuse)
			

		};
	}
}
