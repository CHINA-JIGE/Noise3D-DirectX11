/***********************************************************

					Path Tracer Shader: Diffuse

								diffuse demo

***********************************************************/
#include "Noise3D.h"

using namespace Noise3D;


void Noise3D::GI::PathTracerShader_Diffuse::SetSkyTexture(Texture2D * pTex)
{
	m_pSkyDomeTexture = pTex;
}

void Noise3D::GI::PathTracerShader_Diffuse::ClosestHit(const N_TraceRayParam& param, const N_RayHitInfoForPathTracer & hitInfo, N_TraceRayPayload & in_out_payload)
{
	GI::RandomSampleGenerator g;
	GI::Radiance diffuseRadiance;
	Vec3 albedo = Vec3(1.0f, 1.0f, 1.0f);

	uint32_t diffSampleCount = IPathTracerSoftShader::_MaxDiffuseSample();
	std::vector<Vec3> dirList;
	std::vector<float> pdfList;
	g.UniformSphericalVec_Hemisphere(hitInfo.normal, diffSampleCount, dirList);
	//g.CosinePdfSphericalVec_Cone(hitInfo.normal, Ut::PI / 2.0f, diffSampleCount, dirList, pdfList);

	for (int i = 0; i < diffSampleCount; ++i)
	{
		//gen random ray for diff sample
		Vec3 diffSampleDir = dirList.at(i);
		N_Ray diffSampleRay = N_Ray(hitInfo.pos, diffSampleDir);
		N_TraceRayPayload payload;
		N_TraceRayParam newParam = param;
		newParam.bounces = param.bounces + 1;
		newParam.ray = diffSampleRay;
		IPathTracerSoftShader::_TraceRay(newParam, payload);
		
		//lambert surface/ or disney diffuse
		float LdotN = diffSampleDir.Dot(hitInfo.normal);
		Vec3 viewVec = -param.ray.dir;
		Vec3 lightVec = diffSampleDir;
		Vec3 halfVector = (viewVec + lightVec);// v + h
		halfVector.Normalize();
		Color4f albedo = Color4f(1.0f, 1.0f, 1.0f, 1.0f);

		if (LdotN > 0.0f) 
		{ 
			//GI::Radiance deltaRadiance = payload.radiance * LdotN * Ut::INV_PI;
			Color4f disneyDiffuseBrdf = BxdfUt::DisneyDiffuse(albedo, viewVec, lightVec, hitInfo.normal, halfVector, 1.0f);
			GI::Radiance deltaRadiance = payload.radiance * disneyDiffuseBrdf;

			//deltaRadiance /= pdfList.at(i);//monte carlo
			float pdf = 2.0f * Ut::PI;
			deltaRadiance *= pdf;
			diffuseRadiance += deltaRadiance;
		}
	}
	// estimated = sum/ (pdf*count), pdf = p_hemispherical = 1/ (1/2pi) = 2pi 
	float monteCarloScaleFactor =1.0f / float(diffSampleCount);
	diffuseRadiance *= (monteCarloScaleFactor * albedo);
	diffuseRadiance /= 4.0f;//avoid hdr

	in_out_payload.radiance = diffuseRadiance;
}

void Noise3D::GI::PathTracerShader_Diffuse::Miss(N_Ray ray, N_TraceRayPayload & in_out_payload)
{
	Texture2dSampler_Spherical sampler;
	sampler.SetTexturePtr(m_pSkyDomeTexture);
	Color4f skyColor = sampler.Eval(ray.dir);
	//Color4f skyColor = Color4f(0.6f, 0.8f, 1.0f, 1.0f);
	in_out_payload.radiance = GI::Radiance(skyColor.x, skyColor.y, skyColor.z);
}
