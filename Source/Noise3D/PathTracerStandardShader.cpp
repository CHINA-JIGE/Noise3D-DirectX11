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
			sampler.SetTexturePtr(pTex);
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
			sampler.SetTexturePtr(pTex);
			mShVecSky.Project(3, 10000, &sampler);
		}
	}
}

void Noise3D::GI::PathTracerStandardShader::ClosestHit(const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, N_TraceRayPayload & in_out_payload)
{
	GI::AdvancedGiMaterial* pMat = hitInfo.pHitObj->GetGiMaterial();

	//deal with shadow rays/ return direct lighting first
	GI::Radiance outEmission;
	if (param.isShadowRay)
	{
		//shadow ray only sample target light source (otherwise the radiance might be sum repeatedly)
		if (hitInfo.pHitObj != mLightSourceList.at(param.shadowRayLightSourceId))
			return;

		//emissive surface only display emission (no other BSDF)
		if (pMat->IsEmissionEnabled())
		{
			//TODO:sample emissive map here
			float invDist = 1.0f / (param.ray.Distance(hitInfo.t) + 1.0f);
			outEmission = pMat->GetDesc().emission * invDist * invDist;
			in_out_payload.radiance = outEmission;
			return;
		}
	}
	else
	{
		//primary ray hit light source
		if (pMat->IsEmissionEnabled())
		{
			//TODO:sample emissive map here
			float invDist = 1.0f / (param.ray.Distance(hitInfo.t) + 1.0f);
			outEmission = pMat->GetDesc().emission * invDist * invDist;
			in_out_payload.radiance = outEmission;

			//perhaps i will delete this 'return' to count in other BxDF
			return;
		}
	}

	//integrate over the hemisphere with complete BSDF plus emission
	in_out_payload.radiance = _FinalIntegration(param, hitInfo) + outEmission;
}

void Noise3D::GI::PathTracerStandardShader::Miss(N_Ray ray, N_TraceRayPayload & in_out_payload)
{
	in_out_payload.radiance = GI::Radiance(1.0f, 0, 0);
}

/***************************************

							PRIVATE

***************************************/

GI::Radiance Noise3D::GI::PathTracerStandardShader::_FinalIntegration(const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo)
{
	//primary samples for BSDF
	int directLightingSampleCount = _MaxDiffuseSample();

	//additional samples for microfacet-based specular reflection/transmission
	int indirectBsdfSampleCount = _MaxSpecularScatterSample();

	//additional sample for indirect diffuse lighting and perhaps SH-based env lighting
	int indirectDiffuseSampleCount = _MaxDiffuseSample();

	//BxDF components' final radiance sum
	BxdfInfo info;

	//1.
	GI::Radiance d_1, s_1, t_1;
	_IntegrateBrdfDirectLighting(directLightingSampleCount,param, hitInfo, info, d_1, s_1, t_1);

	//2.
	GI::Radiance s_2, t_2;
	_IntegrateSpecularBsdfIndirect(indirectBsdfSampleCount,param, hitInfo,  s_2, t_2);

	//3.
	GI::Radiance d_2;
	 _IntegrateDiffuseIndirect(indirectDiffuseSampleCount, param, hitInfo, d_2);

	 //result k_x * (direct(emissive) + indirect)
	 GI::Radiance result = info.k_d * (d_1 + d_2) + info.k_s * (s_1 + s_2) + info.k_t * (t_1 + t_2);

	return GI::Radiance();
}

void Noise3D::GI::PathTracerStandardShader::_IntegrateBrdfDirectLighting(
	int samplesPerLight, const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, 
	BxdfInfo& outBxDF,  GI::Radiance& outDiffuse, GI::Radiance& outReflection, GI::Radiance& outTransmission)
{
	GI::RandomSampleGenerator g;

	//loop through light sources/emissive objects
	for (uint32_t lightId = 0; lightId < mLightSourceList.size(); ++lightId)
	{
		GI::Radiance DiffusePerLight, SpecularPerLight, TransmissionPerLight;

		//reduce sample counts as bounces grow
		uint32_t sampleCount = (param.diffusebounces > 1 ? 1 : samplesPerLight);

		std::vector<Vec3> dirList;
		std::vector<float> pdfList;//used by (possibly) importance sampling
		float constantPdf = 0.0f;//used to store constant pdf (like Rect)
		float shadowRayConeAngle = 0.0f;

		if (param.isShadowRay && hitInfo.pHitObj->GetObjectType() == NOISE_SCENE_OBJECT_TYPE::LOGICAL_RECT)
		{
			g.RectShadowRays(hitInfo.pos, static_cast<LogicalRect*>(hitInfo.pHitObj), sampleCount, dirList, constantPdf);
		}
		else
		{
			g.CosinePdfSphericalVec_ShadowRays(hitInfo.pos, mLightSourceList.at(lightId), sampleCount, dirList, pdfList);
		}

		for (uint32_t i = 0; i < sampleCount; ++i)
		{
			//gen random ray for direct lighting
			Vec3 sampleDir = dirList.at(i);
			N_Ray sampleRay = N_Ray(hitInfo.pos, sampleDir);
			Vec3 l = sampleDir;
			Vec3 v = -param.ray.dir;
			Vec3 n = hitInfo.normal;

			//!!!!!!!!!! trace shadow rays/direct lighting
			N_TraceRayPayload payload;
			N_TraceRayParam newParam = param;
			newParam.diffusebounces = param.diffusebounces;//shadow ray
			newParam.ray = sampleRay;
			newParam.isShadowRay = true;
			newParam.shadowRayLightSourceId = lightId;
			if (l.Dot(n) < 0)newParam.isInsideObject = true;//transmission vector
			IPathTracerSoftShader::_TraceRay(newParam, payload);


			//compute complete BxDF
			BxdfInfo bxdfInfo;
			_CompleteBSDF(sampleDir, -param.ray.dir, hitInfo, bxdfInfo);

			//accumulate radiance for 3 BxDF components
			GI::Radiance delta_d = payload.radiance * bxdfInfo.diffuseBRDF;
			GI::Radiance delta_s = payload.radiance * bxdfInfo.reflectionBRDF;
			GI::Radiance delta_t = payload.radiance * bxdfInfo.transmissionBTDF;

			//float pdf = 2.0f * Ut::PI;
			if (param.isShadowRay &&hitInfo.pHitObj->GetObjectType() == NOISE_SCENE_OBJECT_TYPE::LOGICAL_RECT)
			{
				delta_d /= constantPdf;
				delta_s /= constantPdf;
				delta_t /= constantPdf;
			}
			else
			{
				delta_d /= pdfList.at(i);
				delta_s /= pdfList.at(i);
				delta_t /= pdfList.at(i);
			}

			DiffusePerLight += delta_d;
			SpecularPerLight += delta_s;
			TransmissionPerLight += delta_t;
		}
		// estimated = sum/ (pdf*count)
		DiffusePerLight /= float(sampleCount);
		SpecularPerLight /= float(sampleCount);
		TransmissionPerLight /= float(sampleCount);

		outDiffuse += DiffusePerLight;
		outReflection += SpecularPerLight;
		outTransmission += TransmissionPerLight;
	}


}

void Noise3D::GI::PathTracerStandardShader::_IntegrateSpecularBsdfIndirect(int samplesCount, const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, GI::Radiance & outReflection, GI::Radiance & outTransmission)
{
}

void Noise3D::GI::PathTracerStandardShader::_IntegrateDiffuseIndirect(int samplesCount, const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, GI::Radiance & outDiff)
{
}

void Noise3D::GI::PathTracerStandardShader::_CompleteBSDF(Vec3 lightDir, Vec3 viewDir, const N_RayHitInfoForPathTracer & hitInfo, BxdfInfo& outBxdfInfo)
{
	Vec3 k_d = Vec3(0, 0, 0);//difuse energy ratio
	Vec3 k_s = Vec3(0, 0, 0);//(microfacet-based) specular reflection
	Vec3 k_t = Vec3(0, 0, 0);//(microfacet-based) specular refraction
	Vec3 f_d = Vec3(0, 0, 0);//diffuse BRDF
	float f_s =0.0f;//specular reflection BRDF(without Fresnel)
	float f_t = 0.0f;//specular transmission BTDF(without Fresnel)

	//vectors
	Vec3 v = viewDir;//view vec (whose ray hit current pos)
	Vec3 l = lightDir;//light vec (to sample light)
	Vec3 n = hitInfo.normal;//macro surface normal
	Vec3 h = (v + l);
	if (h != Vec3(0, 0, 0))h.Normalize();
	float LdotN = l.Dot(n);

	//material
	const N_AdvancedMatDesc& mat = hitInfo.pHitObj->GetGiMaterial()->GetDesc();
	Vec3 albedo = mat.albedo;
	float alpha = mat.roughness * mat.roughness;

	//Calculate Fresnel term first
	Vec3 F = BxdfUt::SchlickFresnel_Vec3(mat.metal_F0, v, l);

	//WARNING: fresnel term is ignored in Cook-Torrance specular reflection term 
	//k_s = m*F
	k_s = mat.metallicity * F;

	//k_d = rho *(1-m) * (1-t) * (1-s), transparency is dielectric's absorbance caused by  medium's in-homogeneousness
	k_d = albedo * (1.0f - mat.metallicity) * (1.0f - mat.transparency) * (Vec3(1.0f, 1.0f, 1.0f) - k_s);

	//k_t = rho * (1-m) * t * (1-s)
	k_t = albedo * (1.0f - mat.metallicity) * mat.transparency * (Vec3(1.0f, 1.0f, 1.0f) - k_s);

	//diffuse BRDF
	if (LdotN>0.0f &&  k_d != Vec3(0, 0, 0))
	{
		f_d = _DiffuseBRDF(albedo, l, v, n, alpha);
	}

	//specular BxDF
	float D = BxdfUt::D_GGX(n, h, alpha);//NDF
	float G = BxdfUt::G_SmithSchlickGGX(l, v, n, alpha);//shadowing-masking
	if (LdotN > 0.0f && k_s != Vec3(0, 0, 0))
	{
		f_s = _SpecularReflectionBRDF(l, v, n, D, G);
	}

	if (LdotN <0.0f && k_t != Vec3(0, 0, 0))
	{
		//f_t = _SpecularTransmissionBTDF (l, v, n, D, G);
	}

	outBxdfInfo.k_d = k_d;
	outBxdfInfo.k_s = k_s;
	outBxdfInfo.k_t = k_t;
	outBxdfInfo.diffuseBRDF = f_d;
	outBxdfInfo.reflectionBRDF = f_s;
	outBxdfInfo.transmissionBTDF = f_t;
}

Vec3 Noise3D::GI::PathTracerStandardShader::_DiffuseBRDF(Vec3 albedo, Vec3 l, Vec3 v, Vec3 n, float alpha)
{
	//try oren-nayar later
	return BxdfUt::LambertDiffuse(albedo, l, n);
}

float Noise3D::GI::PathTracerStandardShader::_SpecularReflectionBRDF(Vec3 l, Vec3 v, Vec3 n, float D, float G)
{
	return BxdfUt::CookTorranceSpecular(l, v, n, D, G);
}

float Noise3D::GI::PathTracerStandardShader::_SpecularTransmissionBTDF(Vec3 l, Vec3 v, Vec3 n, float D, float G)
{
	ERROR_MSG("not implemeted");
	return 0.0f;
}
