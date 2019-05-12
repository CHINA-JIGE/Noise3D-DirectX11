/***********************************************************

			Path Tracer Shader: Standard Shader

			full support for Advanced GI Material 
			and an accomplished BSDF

***********************************************************/
#include "Noise3D.h"
#include "Noise3D_InDevHeader.h"

using namespace Noise3D;

Noise3D::GI::PathTracerStandardShader::PathTracerStandardShader():
	mSkyLightMultiplier(1.0f)
{
}

void Noise3D::GI::PathTracerStandardShader::SetSkyLightMultiplier(float multiplier)
{
	mSkyLightMultiplier = multiplier;
}

void Noise3D::GI::PathTracerStandardShader::SetSkyLightType(NOISE_PATH_TRACER_SKYLIGHT_TYPE type)
{
	mSkyLightType = type;
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
			mShVecSky.Project(3, 5000, &sampler);
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
			mShVecSky.Project(3, 5000, &sampler);
		}
	}
}

void Noise3D::GI::PathTracerStandardShader::ClosestHit(const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, N_TraceRayPayload & in_out_payload)
{
	GI::PbrtMaterial* pMat = hitInfo.pHitObj->GetPbrtMaterial();
	const N_PbrtMatDesc& mat = pMat->GetDesc();

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
			Vec3 emission = mat.emission;
			if (mat.pEmissiveMap != nullptr)
			{
				Color4f emissionSampled = mat.pEmissiveMap->SamplePixelBilinear(hitInfo.texcoord);
				Vec3 emissionMultiplier = Vec3(emissionSampled.x, emissionSampled.y, emissionSampled.z);
				emission *= emissionMultiplier;
			}
			outEmission = emission;
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
			Vec3 emission = mat.emission;
			if (mat.pEmissiveMap != nullptr)
			{
				Color4f emissionSampled = mat.pEmissiveMap->SamplePixelBilinear(hitInfo.texcoord);
				Vec3 emissionMultiplier = Vec3(emissionSampled.x, emissionSampled.y, emissionSampled.z);
				emission *= emissionMultiplier;
			}
			outEmission = emission;

			//non-emissive part will be treated as common surface, no directly return
			if (outEmission != Vec3(0, 0, 0))
			{
				if (param.isIndirectLightOnly)return;

				//ignore other BxDF, return.
				in_out_payload.radiance = outEmission;
				return;
			}
			else
			{
				//continue
				//non-emissive part will be treated as common surface
			}
		}
	}

	//integrate over the hemisphere with complete BSDF plus emission
	in_out_payload.radiance = _FinalIntegration(param, hitInfo) + outEmission;
}

void Noise3D::GI::PathTracerStandardShader::Miss(const N_TraceRayParam & param, N_TraceRayPayload & in_out_payload)
{
	if (mSkyLightType == NOISE_PATH_TRACER_SKYLIGHT_TYPE::NONE)
	{
		in_out_payload.radiance = GI::Radiance(0,0,0);
		return;
	}

	if (param.isSHEnvLight || mSkyLightType == NOISE_PATH_TRACER_SKYLIGHT_TYPE::SPHERICAL_HARMONIC)
	{
		if (m_pSkyBoxTex != nullptr || m_pSkyDomeTex != nullptr)
		{
			Color4f skyColor = mShVecSky.Eval(param.ray.dir);
			in_out_payload.radiance = GI::Radiance(skyColor.R(), skyColor.G(), skyColor.B());
			if (param.bounces != 0)in_out_payload.radiance *= mSkyLightMultiplier;
			return;
		}
	}

	if (mSkyLightType == NOISE_PATH_TRACER_SKYLIGHT_TYPE::SKY_DOME)
	{
		if (m_pSkyDomeTex != nullptr)
		{
			Texture2dSampler_Spherical sampler;
			if (param.bounces == 0)	sampler.SetFilterMode(true);
			sampler.SetTexturePtr(m_pSkyDomeTex);
			Color4f skyColor = sampler.Eval(param.ray.dir);
			in_out_payload.radiance = GI::Radiance(skyColor.R(), skyColor.G(), skyColor.B());
			if (param.bounces != 0)in_out_payload.radiance *= mSkyLightMultiplier;
			return;
		}
	}

	if (mSkyLightType == NOISE_PATH_TRACER_SKYLIGHT_TYPE::SKY_BOX)
	{
		if (m_pSkyBoxTex != nullptr)
		{
			CubeMapSampler sampler;
			sampler.SetTexturePtr(m_pSkyBoxTex);
			Color4f skyColor = sampler.Eval(param.ray.dir);
			in_out_payload.radiance = GI::Radiance(skyColor.R(), skyColor.G(), skyColor.B());
			if (param.bounces != 0)in_out_payload.radiance *= mSkyLightMultiplier;
			return;
		}
	}

	in_out_payload.radiance = GI::Radiance(0,0,0);
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

	GI::PbrtMaterial* pMat = hitInfo.pHitObj->GetPbrtMaterial();
	const GI::N_PbrtMatDesc& mat = pMat->GetDesc();

	//1. diffuse direct lighting (seems there is no need....)
	//GI::Radiance d_1;
	//_IntegrateDiffuseDirectLighting(directLightingSampleCount,param, hitInfo, d_1);

	//2. diffuse hemisphere
	GI::Radiance d_2;
	_IntegrateDiffuse(indirectDiffuseSampleCount, param, hitInfo, d_2);

	//3. specular hemisphere
	GI::Radiance s_1;
	_IntegrateSpecular(specularScatterampleCount, param, hitInfo, s_1);

	//4.
	GI::Radiance t_1;
	_IntegrateTransmission(specularScatterampleCount, param, hitInfo, t_1);

	 //result. (d_direct+ d_indirect) + 0.5(s_direct + s_indirect) + 0.5(s_complete)
	//GI::Radiance result = (d_1 + d_2) + s_3 + t_1;
	GI::Radiance result = d_2 + s_1 + t_1;
	return result;
}

/*void Noise3D::GI::PathTracerStandardShader::_IntegrateDiffuseDirectLighting(
	int samplesPerLight, const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, 
	GI::Radiance& outDiffuse)
{
	GI::RandomSampleGenerator g;

	//loop through light sources/emissive objects
	for (uint32_t lightId = 0; lightId < mLightSourceList.size(); ++lightId)
	{
		GI::Radiance DiffusePerLight;

		//reduce sample counts as bounces grow
		if (param.specularReflectionBounces > 1 || param.bounces > 1)
			samplesPerLight = 1;

		std::vector<Vec3> dirList;
		std::vector<float> pdfList;//used by (possibly) importance sampling

		if (mLightSourceList.at(lightId)->GetObjectType() == NOISE_SCENE_OBJECT_TYPE::LOGICAL_RECT)
		{			
			LogicalRect* pRect = dynamic_cast<LogicalRect*>(mLightSourceList.at(lightId));
			g.RectShadowRays(hitInfo.pos, pRect, samplesPerLight, dirList, pdfList);
		}
		else
		{
			g.CosinePdfSphericalVec_ShadowRays(hitInfo.pos, mLightSourceList.at(lightId), samplesPerLight, dirList, pdfList);
		}

		for (int i = 0; i < samplesPerLight; ++i)
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
				newParam.bounces = param.bounces;//shadow ray
				newParam.ray = sampleRay;
				newParam.isShadowRay = true;
				newParam.shadowRayLightSourceId = lightId;
				IPathTracerSoftShader::_TraceRay(newParam, payload);

				//compute BxDF info (vary over space)
				BxdfInfo bxdfInfo;
				_CalculateBxDF(BxDF_LightTransfer_Diffuse, sampleDir, -param.ray.dir, hitInfo, bxdfInfo);

				//accumulate radiance for 3 BxDF components
				float cosTerm = std::max<float>(n.Dot(l), 0.0f);
				GI::Radiance delta_d = payload.radiance * bxdfInfo.k_d *  bxdfInfo.diffuseBRDF * cosTerm;

				//N samples share 2*pi*(1-cosf(coneAngleOfShadowRay)) sr, 
				//d\omega in the rendering equation need to be considered more carefully
				//can't just simply divide pdf 

				delta_d /= pdfList.at(i);

				DiffusePerLight += delta_d;
			}
		}
		// estimated = sum/ (pdf*count)
		DiffusePerLight /= float(samplesPerLight);

		outDiffuse += DiffusePerLight;
	}
}*/

void Noise3D::GI::PathTracerStandardShader::_IntegrateDiffuse(int sampleCount, const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, GI::Radiance& outDiffuse)
{
	GI::RandomSampleGenerator g;
	const N_PbrtMatDesc& mat = hitInfo.pHitObj->GetPbrtMaterial()->GetDesc();

	//reduce sample counts as bounces grow
	if (param.bounces > 0)
		sampleCount = 1;
	std::vector<Vec3> dirList;
	std::vector<float> pdfList;//used by (possibly) importance sampling

	//estimate ray cone angle (according to microfacet BRDF)
	g.CosinePdfSphericalVec_Cone(hitInfo.normal, Ut::PI/2.0f, sampleCount, dirList, pdfList);

	for (int i = 0; i < sampleCount; ++i)
	{
		//gen random ray for direct lighting
		Vec3 sampleDir = dirList.at(i);
		N_Ray sampleRay = N_Ray(hitInfo.pos, sampleDir);
		Vec3 l = sampleDir;
		Vec3 v = -param.ray.dir;
		Vec3 n = hitInfo.normal;
		Vec3 h = l + v;
		if (h == Vec3(0, 0, 0))h = n;
		h.Normalize();

		if (l.Dot(n) > 0.0f)
		{
			//!!!!!!!!!! trace shadow rays/direct lighting
			N_TraceRayPayload payload;
			N_TraceRayParam newParam = param;
			newParam.bounces = param.bounces + 1;
			//newParam.specularReflectionBounces = param.specularReflectionBounces + 1;
			newParam.ray = sampleRay;
			newParam.isShadowRay = false;
			newParam.isSHEnvLight = true;
			//newParam.isIndirectLightOnly = true;
			IPathTracerSoftShader::_TraceRay(newParam, payload);

			//compute BxDF info (vary over space)
			BxdfInfo bxdfInfo;
			_CalculateBxDF(BxDF_LightTransfer_Diffuse, l, v, h,  hitInfo, bxdfInfo);

			//clamp cos term in Rendering Equation
			float cosTerm = std::max<float>(n.Dot(l), 0.0f);
			GI::Radiance delta_d = payload.radiance * bxdfInfo.k_d * bxdfInfo.diffuseBRDF * cosTerm;
			delta_d /= pdfList.at(i);
			outDiffuse += delta_d;
		}
	}
	// estimated = sum/ (pdf*count)
	outDiffuse /= float(sampleCount);
}

void Noise3D::GI::PathTracerStandardShader::_IntegrateSpecular(int sampleCount, const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, GI::Radiance & outReflection)
{
	GI::RandomSampleGenerator g;
	const N_PbrtMatDesc& mat = hitInfo.pHitObj->GetPbrtMaterial()->GetDesc();

	//reduce sample counts as bounces grow
	if (param.bounces >0)
		sampleCount = 1;
	std::vector<Vec3> dirList;
	std::vector<float> pdfList;//used by (possibly) importance sampling

	//gen samples/paths for specular reflection (cone angle varys with roughness)
	Vec3 reflectedDir = Vec3::Reflect(param.ray.dir, hitInfo.normal);
	float alpha = _RoughnessToAlpha(mat.roughness);
	Vec3 n = hitInfo.normal;
	Vec3 v = -param.ray.dir;//view vector
	v.Normalize();
	g.GGXImportanceSampling_SpecularReflection(reflectedDir, v, n, alpha, sampleCount, dirList, pdfList);

	for (int i = 0; i < sampleCount; ++i)
	{
		//gen random ray for direct lighting
		Vec3 sampleDir = dirList.at(i);
		N_Ray sampleRay = N_Ray(hitInfo.pos, sampleDir);
		Vec3 l = sampleDir;
		l.Normalize();

		Vec3 h = l + v;
		if (h == Vec3(0, 0, 0))h = n;
		h.Normalize();

		if (l.Dot(n) > 0.0f)
		{
			//!!!!!!! trace rays
			N_TraceRayPayload payload;
			N_TraceRayParam newParam = param;
			newParam.bounces = param.bounces + 1;
			newParam.ray = sampleRay;
			newParam.isShadowRay = false;
			newParam.isSHEnvLight = false;
			IPathTracerSoftShader::_TraceRay(newParam, payload);

			//compute BxDF info (vary over space)
			BxdfInfo bxdfInfo;
			_CalculateBxDF(BxDF_LightTransfer_Specular, l, v, h, hitInfo, bxdfInfo);

			//clamp cos term in Rendering Equation
			float cosTerm = std::max<float>(n.Dot(l), 0.0f);
			GI::Radiance delta_s;
			delta_s = payload.radiance * bxdfInfo.k_s * bxdfInfo.reflectionBRDF * cosTerm;
			//delta_s = payload.radiance * bxdfInfo.k_s * bxdfInfo.reflectionBrdfDividedByD * cosTerm;

			//(2019.4.25)an optimization can be made: that GGX importance sampling pdf's D term
			//can be cancelled with CookTorrance specular's D term
			//but for clarity and less ambiguity, i won't cancelled these 2 D terms from both places
			float pdf = pdfList.at(i);
			if (pdf != 0.0f) 
			{
				delta_s /= pdfList.at(i);

				//"one extra step" for GGX importance sampling: https://agraphicsguy.wordpress.com/2015/11/01/sampling-microfacet-brdf/
				//	equivalent to pdfList.at(i) /= (4.0f * l.Dot(h));
				delta_s *= (4.0f * l.Dot(h));
				outReflection += delta_s;
			}
		}
	}
	// estimated = sum/ (pdf*count)
	outReflection /= float(sampleCount);

}

void Noise3D::GI::PathTracerStandardShader::_IntegrateTransmission(int sampleCount, const N_TraceRayParam & param, const N_RayHitInfoForPathTracer & hitInfo, GI::Radiance & outTransmission)
{
	//[Walter07]
	GI::RandomSampleGenerator g;
	const N_PbrtMatDesc& mat = hitInfo.pHitObj->GetPbrtMaterial()->GetDesc();
	float alpha = _RoughnessToAlpha(mat.roughness);

	//normal and view vector
	Vec3 n = hitInfo.normal;
	Vec3 v = param.ray.dir;
	n.Normalize();
	v.Normalize();

	//reduce sample counts as bounces grow
	if (param.bounces >0)
		sampleCount = 1;
	std::vector<Vec3> dirList;
	std::vector<float> pdfList;//used by (possibly) importance sampling

	//set eta_i, eta_o (refractive index of path's (not light ray) incident/outgoing medium)
	float eta_i = 1.0f;
	float eta_o = mat.ior;
	if (param.isInsideObject)std::swap(eta_i, eta_o);//note that we're talking about 'PATH' not 'light ray'

	//calculate main refracted dir or total internal reflection dir
	Vec3 refractedDir = param.isInsideObject ?
		Vec3::Refract(param.ray.dir, -hitInfo.normal, eta_i/ eta_o) :
		Vec3::Refract(param.ray.dir, hitInfo.normal, eta_i / eta_o);


	//determine if it's total internal reflection
	bool isInternalReflection = (refractedDir == Vec3(0, 0, 0));

#pragma region INTERNAL_REFLECTION
	//**************Internal Reflection********
	//(2019.5.12)actually this is not accurate, because every sample direction should be
	//calculated if it will cause internal reflection.
	//codes below only considers internal reflection with 1 sample
	if (param.isInsideObject && isInternalReflection)
	{
		Vec3 internalReflectedDir = Vec3::Reflect(param.ray.dir, -hitInfo.normal);
		Vec3 l = internalReflectedDir;//light path
		l.Normalize();
		Vec3 h_t = BxdfUt::ComputeHalfVectorForRefraction(v, l, eta_i, eta_o, n);

		N_TraceRayPayload payload_internalReflection;
		N_TraceRayParam newParam = param;
		newParam.bounces = param.bounces + 1;
		newParam.ray = N_Ray(hitInfo.pos, internalReflectedDir); ;
		newParam.isShadowRay = false;
		newParam.isSHEnvLight = false;
		newParam.isInsideObject = true;
		IPathTracerSoftShader::_TraceRay(newParam, payload_internalReflection);
		//light ray inside objects
		BxdfInfo bxdfInfo;
		_CalculateBxDF(BxDF_LightTransfer_InternalReflection, l, v, h_t, hitInfo, bxdfInfo);

		//clamp cos term in Rendering Equation
		float cosTerm = std::max<float>(abs(n.Dot(l)), 0.0f);
		GI::Radiance delta_t;
		delta_t = payload_internalReflection.radiance * bxdfInfo.k_t * bxdfInfo.transmissionBTDF * cosTerm;
		delta_t *= (4.0f * abs(l.Dot(h_t)));
		outTransmission += delta_t;
		return;
	}

	//*********************
#pragma region INTERNAL_REFLECTION


	//*******transmission/refraction********
	//gen paths for specular transmission (cone angle varys with roughness)
	g.GGXImportanceSampling_SpecularTransmission(param.ray.dir, refractedDir, eta_i, eta_o, n, alpha, sampleCount, dirList, pdfList);

	for (int i = 0; i < sampleCount; ++i)
	{
		//gen random ray for direct lighting
		Vec3 sampleDir = dirList.at(i);
		N_Ray sampleRay = N_Ray(hitInfo.pos, sampleDir);
		Vec3 l = sampleDir;//light path
		l.Normalize();

		//half vector h_t should be carefully dealt with
		Vec3 h_t = BxdfUt::ComputeHalfVectorForRefraction(v, l, eta_i, eta_o, n);

		//!!!!!!! trace rays
		N_TraceRayPayload payload_refraction;
		N_TraceRayParam newParam_refraction = param;
		newParam_refraction.bounces = param.bounces + 1;
		newParam_refraction.ray = sampleRay;
		newParam_refraction.isShadowRay = false;
		newParam_refraction.isSHEnvLight = false;

		//compute BxDF info (vary over space)
		BxdfInfo bxdfInfoRefraction;
		if (param.isInsideObject)
		{
			newParam_refraction.isInsideObject = false;
			IPathTracerSoftShader::_TraceRay(newParam_refraction, payload_refraction);
			//light ray incident from object to air
			_CalculateBxDF(BxDF_LightTransfer_Transmission_PathObjectToAir, l, v, h_t, hitInfo, bxdfInfoRefraction);
			
		}
		else
		{
			newParam_refraction.isInsideObject = true;
			IPathTracerSoftShader::_TraceRay(newParam_refraction, payload_refraction);
			//light ray incident from air to object
			_CalculateBxDF(BxDF_LightTransfer_Transmission_PathAirToObject, l, v, h_t, hitInfo, bxdfInfoRefraction);
		}


		//clamp cos term in Rendering Equation
		float cosTerm = std::max<float>(abs(n.Dot(l)), 0.0f);
		GI::Radiance delta_t;
		delta_t = payload_refraction.radiance * bxdfInfoRefraction.k_t * bxdfInfoRefraction.transmissionBTDF * cosTerm;

		//(2019.4.25)an optimization can be made: that GGX importance sampling pdf's D term
		//can be cancelled with CookTorrance specular's D term
		//but for clarity and less ambiguity, i won't cancelled these 2 D terms from both places
		float pdf = pdfList.at(i);
		if (pdf != 0.0f)
		{
			delta_t /= pdfList.at(i);

			//"one extra step" for GGX importance sampling: https://agraphicsguy.wordpress.com/2015/11/01/sampling-microfacet-brdf/
			//	equivalent to pdfList.at(i) /= (4.0f * l.Dot(h));
			delta_t *= (4.0f * abs(l.Dot(h_t)));
			outTransmission += delta_t;
		}
	}

	// estimated = sum/ (pdf*count)
	outTransmission /= float(sampleCount);
}

void Noise3D::GI::PathTracerStandardShader::_CalculateBxDF(uint32_t lightTransferType, Vec3 lightDir, Vec3 viewDir, Vec3 halfVector, const N_RayHitInfoForPathTracer & hitInfo, BxdfInfo& outBxdfInfo)
{
	if (lightTransferType == 0)return;

	Vec3 k_d = Vec3(0, 0, 0);//difuse energy ratio
	Vec3 k_s = Vec3(0, 0, 0);//(microfacet-based) specular reflection
	Vec3 k_t = Vec3(0, 0, 0);//(microfacet-based) specular refraction
	Vec3 f_d = Vec3(0, 0, 0);//diffuse BRDF
	float f_s = 0.0f;//specular reflection BRDF(without Fresnel)
	float f_s2 = 0.0f;//specular reflection Brdf (without multiplying NDF)
	float f_t = 0.0f;//specular transmission BTDF(without Fresnel)

	//vectors
	Vec3 v = viewDir;//view vec (whose ray hit current pos)
	v.Normalize();
	Vec3 l = lightDir;//light vec (to sample light)
	l.Normalize();
	Vec3 n = hitInfo.normal;//macro surface normal
	n.Normalize();
	Vec3 h = halfVector;
	h.Normalize();
	float LdotN = l.Dot(n);

	//material
	const N_PbrtMatDesc& mat = hitInfo.pHitObj->GetPbrtMaterial()->GetDesc();

	//albedo & albedo map (diffusive)
	Vec3 albedo = mat.albedo;
	if (mat.pAlbedoMap != nullptr)
	{
		Color4f albedoSampled = mat.pAlbedoMap->SamplePixelBilinear(hitInfo.texcoord);
		albedo *= Vec3(albedoSampled.x, albedoSampled.y, albedoSampled.z);
	}

	//roughness and roughness map
	float roughness = mat.roughness;
	if (mat.pRoughnessMap != nullptr)
	{
		Color4f roughnessSampled = mat.pRoughnessMap->SamplePixelBilinear(hitInfo.texcoord);
		float roughness_greyScale = 0.33333333f*(roughnessSampled.x + roughnessSampled.y + roughnessSampled.z);//convert the grey scale
		roughness = roughness_greyScale;
	}
	float alpha = _RoughnessToAlpha(roughness);

	//Calculate Fresnel term, but sample metallicity map first
	float metallicity = mat.metallicity;
	if (mat.pMetallicityMap != nullptr)
	{
		Color4f metalSampled = mat.pMetallicityMap->SamplePixelBilinear(hitInfo.texcoord);
		float metallicityMultipler = 0.33333333f*(metalSampled.x + metalSampled.y + metalSampled.z);//convert the grey scale
		metallicity *= metallicityMultipler;
	}

	const Vec3 defaultDielectric_F0 = Vec3(0.03f, 0.03f, 0.03f);
	Vec3 F0 = Ut::Lerp(defaultDielectric_F0, mat.metal_F0, metallicity);
	Vec3 F;
	if (lightTransferType & BxDF_LightTransfer_Transmission_PathAirToObject)
	{
		F = BxdfUt::SchlickFresnel_Vec3(F0, -v, h);
	}
	else
	{
		F = BxdfUt::SchlickFresnel_Vec3(F0, v, h);
	}


	//k_s = m*F
	k_s = F;

	//k_d = rho *(1-m) * (1-t) * (1-s), transparency is dielectric's absorbance caused by  medium's in-homogeneousness
	k_d = albedo * (1.0f - metallicity) * (1.0f - mat.transparency) * (Vec3(1.0f, 1.0f, 1.0f) - k_s);

	//k_t = rho * (1-m) * t * (1-s)
	k_t = albedo * (1.0f - metallicity) * mat.transparency * (Vec3(1.0f, 1.0f, 1.0f) - k_s);

	//diffuse BRDF
	if (lightTransferType & BxDF_LightTransfer_Diffuse)
	{
		if (LdotN > 0.0f &&  k_d != Vec3(0, 0, 0))
		{
			f_d = _DiffuseBRDF(albedo, l, v, n, alpha);
		}
	}

	//specular BxDF
	float D = BxdfUt::D_GGX(n, h, alpha);//NDF
	float G = BxdfUt::G_SmithSchlickGGX(l, v, n, alpha);//shadowing-masking
	if (lightTransferType & BxDF_LightTransfer_Specular)
	{
		if (LdotN > 0.0f && k_s != defaultDielectric_F0)
		{

			//WARNING: fresnel term is ignored in Cook-Torrance specular reflection term 
			f_s = _SpecularReflectionBRDF(l, v, n, D, G) ;
			//f_s2 = _SpecularReflectionBrdfDividedByD(l, v, n, G);
		}
	}

	//specular transmission
	if (lightTransferType & BxDF_LightTransfer_Transmission_PathObjectToAir)
	{
		if (LdotN > 0.0f && k_t != Vec3(0, 0, 0))
		{
			f_t = _SpecularTransmissionBTDF(l, v, n, h, D, G, mat.ior, 1.0f);
		}
	}
	else if (lightTransferType & BxDF_LightTransfer_Transmission_PathAirToObject)
	{
		if (LdotN < 0.0f && k_t != Vec3(0, 0, 0))
		{
			f_t = _SpecularTransmissionBTDF(l, v, n, h, D, G, 1.0f, mat.ior);
		}
	}
	else if(lightTransferType & BxDF_LightTransfer_InternalReflection)
	{
		if (LdotN < 0.0f && k_t != Vec3(0, 0, 0))
		{
			f_t = _SpecularTransmissionBTDF(l, v, n, h, D, G, mat.ior, 1.0f);
		}
	}


	outBxdfInfo.k_d = k_d;
	outBxdfInfo.k_s = k_s;
	outBxdfInfo.k_t = k_t;
	outBxdfInfo.diffuseBRDF = f_d;
	outBxdfInfo.reflectionBRDF = f_s;
	//outBxdfInfo.reflectionBrdfDividedByD = f_s2;
	outBxdfInfo.transmissionBTDF = f_t;
	outBxdfInfo.D = D;

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

float Noise3D::GI::PathTracerStandardShader::_SpecularReflectionBrdfDividedByD(Vec3 l, Vec3 v, Vec3 n, float G)
{
	return G / (4.0f*(v.Dot(n))*(l.Dot(n)));//D is cancelled in integration using GGX importance sampling
}

float Noise3D::GI::PathTracerStandardShader::_SpecularTransmissionBTDF(Vec3 l, Vec3 v, Vec3 n, Vec3 h, float D, float G, float eta_i, float eta_o)
{
	float nominator = abs(l.Dot(h)) * abs(v.Dot(h)) * eta_o * eta_o * G * D;
	float VdotH = v.Dot(h);
	float LdotH = l.Dot(h);
	float denom1 = eta_i * VdotH + eta_o * LdotH;
	//float denom1 = eta_i * VdotH - eta_o * LdotH;
	float denominator = abs(l.Dot(n)) * abs(v.Dot(n)) * denom1 * denom1;
	return nominator/denominator;
}

inline float Noise3D::GI::PathTracerStandardShader::_RoughnessToAlpha(float r)
{
	//could be r*r
	return r*r;
}
