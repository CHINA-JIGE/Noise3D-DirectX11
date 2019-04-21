/***********************************************************

			Path Tracer Shader: Standard Shader

			full support for Advanced GI Material 
			and an accomplished BSDF

***********************************************************/
#include "Noise3D.h"

using namespace Noise3D;

void Noise3D::GI::PathTracerStandardShader::SetSkyType(NOISE_ATMOSPHERE_SKYTYPE type)
{
	mSkyType = type;
}

void Noise3D::GI::PathTracerStandardShader::SetSkyDomeTexture(Texture2D * pTex, bool computeSH16)
{
	if (pTex != nullptr)
	{
		m_pSkyDomeTex = pTex;
		if (computeSH16)
		{
			GI::Texture2dSampler_Spherical sampler;
			mShVecSky.Project(3, 10000, &sampler);
		}
	}
}

void Noise3D::GI::PathTracerStandardShader::SetSkyBoxTexture(TextureCubeMap * pTex, bool computeSH16)
{
	if (pTex != nullptr)
	{
		m_pSkyBoxTex = pTex;
		if (computeSH16)
		{
			GI::CubeMapSampler sampler;
			mShVecSky.Project(3, 10000, &sampler);
		}
	}
}

void Noise3D::GI::PathTracerStandardShader::ClosestHit(const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, N_TraceRayPayload & in_out_payload)
{
}

void Noise3D::GI::PathTracerStandardShader::Miss(N_Ray ray, N_TraceRayPayload & in_out_payload)
{
}

/***************************************

							PRIVATE

***************************************/

Vec3 Noise3D::GI::PathTracerStandardShader::_CompleteBsdf(const N_Ray& newSampleRay,const N_TraceRayParam& hitRayParam, const N_RayHitInfoForPathTracer & hitInfo)
{
	float k_d = 0.0f;//difuse energy ratio
	float k_s = 0.0f;//(microfacet-based) specular reflection
	float k_t = 0.0f;//(microfacet-based) specular refraction

	float F = 0.0f;//fresnel
	Vec3 l = param.ray.dir;//light vec
	Vec3 v = hitInfo.
	//..
	_SpecularBsdf(param.ray.dir, )

	return Vec3();
}
