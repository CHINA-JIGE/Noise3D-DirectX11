/***********************************************************

			Path Tracer Shader: Standard Shader

			full support for Advanced GI Material 
			and an accomplished BSDF

***********************************************************/
#include "Noise3D.h"
#include "Noise3D_InDevHeader.h"

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
			//float invDist = 1.0f / (param.ray.Distance(hitInfo.t) + 1.0f);
			//outEmission = pMat->GetDesc().emission * invDist * invDist;
			outEmission = pMat->GetDesc().emission;
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
			//float invDist = 1.0f / (param.ray.Distance(hitInfo.t) + 1.0f);
			//outEmission = pMat->GetDesc().emission * invDist * invDist;
			outEmission = pMat->GetDesc().emission;
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
	Color4f c =mShVecSky.Eval(ray.dir);
	//Color4f c= GI::Radiance(0,0,0);
	in_out_payload.radiance = GI::Radiance(c.R(), c.G(), c.B());
}

/***************************************

							PRIVATE

***************************************/

GI::Radiance Noise3D::GI::PathTracerStandardShader::_FinalIntegration(const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo)
{
	//primary samples for BSDF
	int directLightingSampleCount = _MaxDiffuseSample();

	//additional samples for microfacet-based specular reflection/transmission
	int specularScatterampleCount = _MaxSpecularScatterSample();

	//additional sample for indirect diffuse lighting and perhaps SH-based env lighting
	int indirectDiffuseSampleCount = _MaxDiffuseSample();

	//1.
	GI::Radiance d_1, s_1;
	_IntegrateBrdfDirectLighting(directLightingSampleCount,param, hitInfo, d_1, s_1);

	//2.
	GI::Radiance d_2, s_2;
	_IntegrateBrdfIndirectLightingUniformly(indirectDiffuseSampleCount, param, hitInfo, d_2, s_2);

	//3.
	GI::Radiance t_1;
	_IntegrateTransmission(specularScatterampleCount, param, hitInfo, t_1);

	//4.
	GI::Radiance s_3;
	_AdditionalIntegrateSpecular(specularScatterampleCount, param, hitInfo, s_3);

	 //result. (d_direct+ d_indirect) + 0.5(s_direct + s_indirect) + 0.5(s_complete)
	//GI::Radiance result = (d_1 + d_2) + 0.3f*(s_1 + s_2) + 0.7f *s_3 + t_1;
	GI::Radiance result = (d_1 + d_2) + s_3 + t_1;

	return result;
}

void Noise3D::GI::PathTracerStandardShader::_IntegrateBrdfDirectLighting(
	int samplesPerLight, const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, 
	GI::Radiance& outDiffuse, GI::Radiance& outReflection)
{
	GI::RandomSampleGenerator g;

	//loop through light sources/emissive objects
	for (uint32_t lightId = 0; lightId < mLightSourceList.size(); ++lightId)
	{
		GI::Radiance DiffusePerLight;
		GI::Radiance SpecularPerLight;

		//reduce sample counts as bounces grow
		uint32_t sampleCount = (param.diffusebounces > 1 ? 1 : samplesPerLight);

		std::vector<Vec3> dirList;
		std::vector<float> pdfList;//used by (possibly) importance sampling

		if (mLightSourceList.at(lightId)->GetObjectType() == NOISE_SCENE_OBJECT_TYPE::LOGICAL_RECT)
		{			
			LogicalRect* pRect = dynamic_cast<LogicalRect*>(mLightSourceList.at(lightId));
			g.RectShadowRays(hitInfo.pos, pRect, sampleCount, dirList, pdfList);
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

			if (l.Dot(n) > 0.0f)
			{
				//!!!!!!!!!! trace shadow rays/direct lighting
				N_TraceRayPayload payload;
				N_TraceRayParam newParam = param;
				newParam.diffusebounces = param.diffusebounces;//shadow ray
				newParam.ray = sampleRay;
				newParam.isShadowRay = true;
				newParam.shadowRayLightSourceId = lightId;
				IPathTracerSoftShader::_TraceRay(newParam, payload);

				//compute BxDF info (vary over space)
				BxdfInfo bxdfInfo;
				_CalculateBxDF(BxDF_LightTransfer_Diffuse | BxDF_LightTransfer_Specular, sampleDir, -param.ray.dir, hitInfo, bxdfInfo);

				//accumulate radiance for 3 BxDF components
				GI::Radiance delta_d = payload.radiance * bxdfInfo.k_d *  bxdfInfo.diffuseBRDF;
				GI::Radiance delta_s = payload.radiance * bxdfInfo.k_s * bxdfInfo.reflectionBRDF;

				delta_d /= pdfList.at(i);
				delta_s /= pdfList.at(i);

				DiffusePerLight += delta_d;
				SpecularPerLight += delta_s;
			}
		}
		// estimated = sum/ (pdf*count)
		DiffusePerLight /= float(sampleCount);
		SpecularPerLight /= float(sampleCount);

		outDiffuse += DiffusePerLight;
		outReflection += SpecularPerLight;
	}
}

void Noise3D::GI::PathTracerStandardShader::_IntegrateBrdfIndirectLightingUniformly(int samplesPerLight, const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, GI::Radiance & outDiffuse, GI::Radiance & outReflection)
{
}

void Noise3D::GI::PathTracerStandardShader::_IntegrateTransmission(int samplesCount, const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, GI::Radiance & outTransmission)
{
}

void Noise3D::GI::PathTracerStandardShader::_AdditionalIntegrateSpecular(int samplesCount, const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, GI::Radiance & outReflection)
{
	GI::RandomSampleGenerator g;
	const N_AdvancedMatDesc& mat = hitInfo.pHitObj->GetGiMaterial()->GetDesc();

	//reduce sample counts as bounces grow
	int sampleCount = (param.specularReflectionBounces > 1 ? 1 : samplesCount);
	std::vector<Vec3> dirList;
	std::vector<float> pdfList;//used by (possibly) importance sampling

	//estimate ray cone angle (according to microfacet BRDF)
	float EstimatedConeAngle = 0.001f+ (Ut::PI / 2.0001f) * mat.roughness * mat.roughness;

	//gen samples for specular reflection (cone angle varys with roughness)
	Vec3 reflectedDir = Vec3::Reflect(param.ray.dir, hitInfo.normal);
	reflectedDir.Normalize();
	g.CosinePdfSphericalVec_Cone(reflectedDir, EstimatedConeAngle, sampleCount, dirList, pdfList);

	for (uint32_t i = 0; i < sampleCount; ++i)
	{
		//gen random ray for direct lighting
		Vec3 sampleDir = dirList.at(i);
		N_Ray sampleRay = N_Ray(hitInfo.pos, sampleDir);
		Vec3 l = sampleDir;
		Vec3 v = -param.ray.dir;
		Vec3 n = hitInfo.normal;

		if (l.Dot(n) > 0.0f)
		{
			//!!!!!!!!!! trace shadow rays/direct lighting
			N_TraceRayPayload payload;
			N_TraceRayParam newParam = param;
			newParam.specularReflectionBounces = param.specularReflectionBounces+1;
			newParam.ray = sampleRay;
			newParam.isShadowRay = false;
			IPathTracerSoftShader::_TraceRay(newParam, payload);

			//compute BxDF info (vary over space)
			BxdfInfo bxdfInfo;
			_CalculateBxDF(BxDF_LightTransfer_Specular, sampleDir, -param.ray.dir, hitInfo, bxdfInfo);

			//accumulate radiance for 3 BxDF components
			GI::Radiance delta_s = payload.radiance * bxdfInfo.k_s * bxdfInfo.reflectionBRDF;
			delta_s /= pdfList.at(i);
			outReflection += delta_s;
		}
	}
	// estimated = sum/ (pdf*count)
	outReflection /= float(sampleCount);
}

void Noise3D::GI::PathTracerStandardShader::_CalculateBxDF(uint32_t lightTransferType, Vec3 lightDir, Vec3 viewDir, const N_RayHitInfoForPathTracer & hitInfo, BxdfInfo& outBxdfInfo)
{
	if (lightTransferType == 0)return;

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
	Vec3 F0 = Ut::Lerp(Vec3(0.03f, 0.03f, 0.03f), mat.metal_F0, mat.metallicity);
	Vec3 F = BxdfUt::SchlickFresnel_Vec3(F0, v, h);

	//k_s = m*F
	k_s = F;

	//k_d = rho *(1-m) * (1-t) * (1-s), transparency is dielectric's absorbance caused by  medium's in-homogeneousness
	k_d = albedo * (1.0f - mat.metallicity) * (1.0f - mat.transparency) * (Vec3(1.0f, 1.0f, 1.0f) - k_s);

	//k_t = rho * (1-m) * t * (1-s)
	k_t = albedo * (1.0f - mat.metallicity) * mat.transparency * (Vec3(1.0f, 1.0f, 1.0f) - k_s);

	//diffuse BRDF
	if (lightTransferType | uint32_t(BxDF_LightTransfer_Diffuse))
	{
		if (LdotN > 0.0f &&  k_d != Vec3(0, 0, 0))
		{
			f_d = _DiffuseBRDF(albedo, l, v, n, alpha);
		}
	}

	//specular BxDF
	float D = BxdfUt::D_GGX(n, h, alpha);//NDF
	//float D = BxdfUt::D_Beckmann(n, h, alpha);
	float G = BxdfUt::G_SmithSchlickGGX(l, v, n, alpha);//shadowing-masking
	if (lightTransferType | BxDF_LightTransfer_Specular)
	{
		if (LdotN > 0.0f && k_s != Vec3(0, 0, 0))
		{
			//WARNING: fresnel term is ignored in Cook-Torrance specular reflection term 
			f_s = _SpecularReflectionBRDF(l, v, n, D, G);
		}
	}

	//specular transmission
	if (lightTransferType | BxDF_LightTransfer_Transmission)
	{
		if (LdotN < 0.0f && k_t != Vec3(0, 0, 0))
		{
			f_t = _SpecularTransmissionBTDF(l, v, n, D, G);
		}
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
