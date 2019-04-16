/***********************************************************

					Path Tracer Shader:  Sky & Reflection

			Noise3D::PathTracer's soft shader "hello world", 
			for demonstration and test usage

***********************************************************/
#include "Noise3D.h"

using namespace Noise3D;

void Noise3D::GI::PathTracerShader_ReflectionDemo::SetSkyTexture(Texture2D * pTex)
{
	m_pSkyDomeTexture = pTex;
}

void Noise3D::GI::PathTracerShader_ReflectionDemo::ClosestHit(const N_TraceRayParam& param, const N_RayHitInfoForPathTracer & hitInfo, N_TraceRayPayload & in_out_payload)
{
	//Texture2dSampler_Spherical sampler;
	//sampler.SetTexturePtr(m_pSkyDomeTexture);
	Vec3 reflectedDir = Vec3::Reflect(param.ray.dir, hitInfo.normal);
	reflectedDir.Normalize();
	N_Ray reflectedRay = N_Ray(hitInfo.pos, reflectedDir);
	//travelled distance is added automatically
	N_TraceRayPayload payload;
	N_TraceRayParam newParam = param;
	newParam.specularReflectionBounces = param.specularReflectionBounces + 1;
	newParam.ray = reflectedRay;
	IPathTracerSoftShader::_TraceRay(newParam, payload);
	GI::Radiance reflectedColor = payload.radiance;
	in_out_payload.radiance = reflectedColor;
}

void Noise3D::GI::PathTracerShader_ReflectionDemo::Miss(N_Ray ray, N_TraceRayPayload & in_out_payload)
{
	Texture2dSampler_Spherical sampler;
	sampler.SetTexturePtr(m_pSkyDomeTexture);
	Color4f skyColor = sampler.Eval(ray.dir);
	in_out_payload.radiance = GI::Radiance(skyColor.x, skyColor.y, skyColor.z);
}
