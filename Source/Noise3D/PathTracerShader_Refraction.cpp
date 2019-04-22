/***********************************************************

				Path Tracer Shader demo:  Refraction (glass ball?)

***********************************************************/
#include "Noise3D.h"
#include "Noise3D_InDevHeader.h"

using namespace Noise3D;

void Noise3D::GI::PathTracerShader_RefractionDemo::SetSkyTexture(Texture2D * pTex)
{
	m_pSkyDomeTexture = pTex;
}

void Noise3D::GI::PathTracerShader_RefractionDemo::ClosestHit(const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, N_TraceRayPayload & in_out_payload)
{
	//refract: transmit_dir = (n_i/n_t) (- in_dir) + [ (n_i /n _t)(in_dir dot n) - cos theta_t ] n
	GI::Radiance finalTransmittedColor;
	GI::AdvancedGiMaterial* pMat = hitInfo.pHitObj->GetGiMaterial();
	float ior_object_over_air = pMat->GetDesc().ior;

	//ray is traced object-to-air
	if (param.isInsideObject)
	{
		//n_obj sin_obj = n_air sin_air

		//inside-out, normal's vector should be reversed
		Vec3 refractedDir = XMVector3Refract(param.ray.dir, -hitInfo.normal, ior_object_over_air);
		N_TraceRayPayload payload;
		N_TraceRayParam newParam = param;

		//if total internal reflection, (0,0,0) will be returned by XMVec3Refract
		if (refractedDir == Vec3(0, 0, 0))
		{
			//total internal reflection
			Vec3 newTraceDir = XMVector3Reflect(param.ray.dir, -hitInfo.normal);
			newParam.specularReflectionBounces = param.specularReflectionBounces + 1;
			newParam.isInsideObject = true;
			newParam.ray = N_Ray(hitInfo.pos, newTraceDir);
		}
		else
		{
			//refract from object to air
			Vec3 newTraceDir = refractedDir;
			newParam.refractionBounces = param.refractionBounces + 1;
			newParam.isInsideObject = false;
			newParam.ray = N_Ray(hitInfo.pos, newTraceDir);
		}

		IPathTracerSoftShader::_TraceRay(newParam, payload);
		finalTransmittedColor = payload.radiance;
	}
	else
	{
		//ray is traced air-to-object

		//n1 sin1 = n2 sin2
		Vec3 refractedDir = XMVector3Refract(param.ray.dir ,hitInfo.normal, 1.0f / ior_object_over_air);
		N_Ray refractedRay = N_Ray(hitInfo.pos, refractedDir);

		//travelled distance is added automatically
		N_TraceRayPayload payload;
		N_TraceRayParam newParam = param;
		newParam.refractionBounces = param.refractionBounces + 1;
		newParam.ray = refractedRay;
		newParam.isInsideObject = true;//new ray travel inside object
		IPathTracerSoftShader::_TraceRay(newParam, payload);

		finalTransmittedColor = payload.radiance;
	}

	in_out_payload.radiance = finalTransmittedColor;
}

void Noise3D::GI::PathTracerShader_RefractionDemo::Miss(N_Ray ray, N_TraceRayPayload & in_out_payload)
{
	Texture2dSampler_Spherical sampler;
	sampler.SetTexturePtr(m_pSkyDomeTexture);
	Color4f skyColor = sampler.Eval(ray.dir);
	//Color4f skyColor = Color4f(0.6f, 0.8f, 1.0f, 1.0f);
	in_out_payload.radiance = GI::Radiance(skyColor.x, skyColor.y, skyColor.z);
}