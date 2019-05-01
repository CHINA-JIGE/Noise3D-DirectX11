/***********************************************************

					Path Tracer Shader: Area Lighting

					area light/local lighting demo

***********************************************************/
#include "Noise3D.h"

using namespace Noise3D;

/***********************************************************

			Path Tracer Shader: Area Lighting

***********************************************************/
#include "Noise3D.h"

using namespace Noise3D;

void Noise3D::GI::PathTracerShader_AreaLightingDemo::ClosestHit(const N_TraceRayParam& param, const N_RayHitInfoForPathTracer & hitInfo, N_TraceRayPayload & in_out_payload)
{
	GI::PbrtMaterial* pMat = hitInfo.pHitObj->GetPbrtMaterial();

	GI::Radiance outEmission;

	/*if (param.isShadowRay)
	{
		if (hitInfo.pHitObj != mLightSourceList.at(param.shadowRayLightSourceId))return;

		float invDist = 1.0f / (param.ray.Distance(hitInfo.t) + 1.0f);
		//float logExposureInvDist = 1.0f / (log(dist) + 0.1f);
		outEmission = pMat->GetDesc().emission* invDist * invDist;//_EvalEmission(param.ray, param.shadowRayLightSourceId)* invDist * invDist;
		if (outEmission.Length() > 2.0f)
		{
			in_out_payload.radiance = outEmission;
			return;
		}
	}
	else
	{
		//primary ray
		if (pMat->IsEmissionEnabled())
		{
			float invDist = 1.0f / (param.ray.Distance(hitInfo.t) + 1.0f);
			//float logExposureInvDist = 1.0f/(log(dist)+0.1f);
			outEmission = pMat->GetDesc().emission * invDist * invDist;
			in_out_payload.radiance = outEmission;
			return;
		}
	}*/
	if (param.isShadowRay)
	{
		if (hitInfo.pHitObj != mLightSourceList.at(param.shadowRayLightSourceId))return;
	}
	if (pMat->IsEmissionEnabled())
	{
		float invDist = 1.0f / (param.ray.Distance(hitInfo.t) + 1.0f);
		//float logExposureInvDist = 1.0f/(log(dist)+0.1f);
		outEmission = pMat->GetDesc().emission * invDist * invDist;
		in_out_payload.radiance = outEmission;
		return;
	}

	GI::RandomSampleGenerator g;
	GI::Radiance accumulatedDiffuseRadiance;
	Vec3 albedo = pMat->GetDesc().albedo;

	//g.UniformSphericalVec_Hemisphere(hitInfo.normal, diffSampleCount, dirList);
	//g.CosinePdfSphericalVec_Cone(hitInfo.normal, Ut::PI / 2.0f, diffSampleCount, dirList, pdfList);
	for (uint32_t lightId = 0; lightId < mLightSourceList.size(); ++lightId)
	{	
		//reduce sample counts as bounces grow
		uint32_t denominator = (param.bounces + 1)*(param.bounces + 1);
		uint32_t diffSampleCount = IPathTracerSoftShader::_MaxDiffuseSample() / denominator;
		std::vector<Vec3> dirList;
		std::vector<float> pdfList;//used by importance sampling demo
		float constantPdf = 0.0f;//used to compute pdf
#define COSINE_WEIGHTED_IMPORTANCE_SAMPLING

#ifdef COSINE_WEIGHTED_IMPORTANCE_SAMPLING
		if (param.isShadowRay && hitInfo.pHitObj->GetObjectType() == NOISE_SCENE_OBJECT_TYPE::LOGICAL_RECT)
		{
			//g.RectShadowRays(hitInfo.pos, static_cast<LogicalRect*>(hitInfo.pHitObj), diffSampleCount, dirList, constantPdf);
			g.RectShadowRays(hitInfo.pos, static_cast<LogicalRect*>(hitInfo.pHitObj), diffSampleCount, dirList, pdfList);

		}
		else
		{
			g.CosinePdfSphericalVec_ShadowRays(hitInfo.pos, mLightSourceList.at(lightId), diffSampleCount, dirList, pdfList);
		}
#else 
		g.UniformSphericalVec_ShadowRays(hitInfo.pos, mLightSourceList.at(lightId), diffSampleCount, dirList, constantPdf);
#endif

		for (uint32_t i = 0; i < diffSampleCount; ++i)
		{
			//gen random ray for diff sample
			Vec3 diffSampleDir = dirList.at(i);
			N_Ray diffSampleRay = N_Ray(hitInfo.pos, diffSampleDir);

			//!!!!!!!!!! trace shadow rays/direct lighting
			N_TraceRayPayload payload;
			N_TraceRayParam newParam = param;
			newParam.bounces = param.bounces;//shadow ray
			newParam.ray = diffSampleRay;
			newParam.isShadowRay = true;
			newParam.shadowRayLightSourceId = lightId;
			IPathTracerSoftShader::_TraceRay(newParam, payload);

			//lambert surface/ or disney diffuse
			float LdotN = diffSampleDir.Dot(hitInfo.normal);
			Vec3 viewVec = -param.ray.dir;
			Vec3 lightVec = diffSampleDir;
			Vec3 halfVector = (viewVec + lightVec);// v + l
			halfVector.Normalize();

			GI::Radiance diffuseRadiancePerLight;
			if (LdotN > 0.0f)
			{
				Vec3 lambertDiffuseBrdf = BxdfUt::LambertDiffuse(albedo, LdotN);
				GI::Radiance deltaRadiance = payload.radiance *lambertDiffuseBrdf;
				//Color4f disneyDiffuseBrdf = BxdfUt::DisneyDiffuse(albedo, viewVec, lightVec, hitInfo.normal, halfVector, 1.0f);
				//GI::Radiance deltaRadiance = payload.radiance * disneyDiffuseBrdf;

				//float pdf = 2.0f * Ut::PI;
#ifdef COSINE_WEIGHTED_IMPORTANCE_SAMPLING
				if (param.isShadowRay &&hitInfo.pHitObj->GetObjectType() == NOISE_SCENE_OBJECT_TYPE::LOGICAL_RECT)
				{
					//deltaRadiance /= constantPdf;
					deltaRadiance /= pdfList.at(i);
				}
				else
				{
					deltaRadiance /= pdfList.at(i);
				}
#else
				deltaRadiance /= constantPdf;
#endif

				diffuseRadiancePerLight += deltaRadiance;
			}
			// estimated = sum/ (pdf*count), pdf = p_hemispherical = 1/ (1/2pi) = 2pi 
			diffuseRadiancePerLight /= float(diffSampleCount);
			accumulatedDiffuseRadiance += diffuseRadiancePerLight;
		}

	}


	in_out_payload.radiance = accumulatedDiffuseRadiance + outEmission;
}

void Noise3D::GI::PathTracerShader_AreaLightingDemo::Miss(const N_TraceRayParam & param, N_TraceRayPayload & in_out_payload)
{
	in_out_payload.radiance = GI::Radiance(0,0,0);
}

GI::Radiance Noise3D::GI::PathTracerShader_AreaLightingDemo::_EvalEmission(N_Ray shadowRay, uint32_t lightSourceId)
{
	GI::IGiRenderable* pLightSrc = mLightSourceList.at(lightSourceId);
	const N_PbrtMatDesc& desc = pLightSrc->GetPbrtMaterial()->GetDesc();
	//float distInv = 1.0f / (shadowRay.dir.Length()+0.01f);
	//return GI::Radiance(desc.emission) * distInv * distInv;
	return GI::Radiance(desc.emission);
}


