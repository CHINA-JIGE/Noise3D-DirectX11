/****************************************************

						BxDF Utility Functions

*****************************************************/

#include "Noise3D.h"
#include "Noise3D_InDevHeader.h"

using namespace Noise3D;

Vec3 Noise3D::GI::BxdfUt::SchlickFresnel_Vec3(Vec3 F0, Vec3 v, Vec3 h)
{
	//v:view, l:light, n:normal, h:half vector, alpha:roughness^2
	//F = F0 + (1-F0)(1-(v dot H))^5
	float VdotH = v.Dot(h);
	float OneMinusCos = 1.0f - Ut::Clamp(v.Dot(h), 0.0f, 1.0f);
	float OneMinusCos2 = OneMinusCos * OneMinusCos;
	Vec3 F = F0 + (Vec3(1.0f, 1.0f, 1.0f) - F0) * OneMinusCos2 * OneMinusCos2 *OneMinusCos;
	return F;
}

float Noise3D::GI::BxdfUt::SchlickFresnel(float F0, Vec3 v, Vec3 h)
{
	//v:view, l:light, n:normal, h:half vector, alpha:roughness^2
	//F = F0 + (1-F0)(1-(v dot H))^5
	float OneMinusCos = 1.0f - Ut::Clamp(v.Dot(h), 0.0f, 1.0f);
	float OneMinusCos2 = OneMinusCos * OneMinusCos;
	float F = F0 + (1.0f - F0) * OneMinusCos2 * OneMinusCos2 *OneMinusCos;
	return F;
}

Vec3 Noise3D::GI::BxdfUt::DisneyDiffuse(Vec3 albedo, Vec3 v, Vec3 l, Vec3 n, Vec3 h, float alpha)
{
	//v:view, l:light, n:normal, h:half vector, alpha:roughness^2
	//disney diffuse = ( albedo/pi ) *(1+ (F_D90-1)(1- (v dot n))^5)*(1+ (F_D90-1)(1- (l dot n))^5)
	float oneMinusCosL = 1.0f - Ut::Clamp(l.Dot(n), 0.0f, 1.0f);
	float oneMinusCosLSqr = oneMinusCosL * oneMinusCosL;
	float oneMinusCosV = 1.0f - Ut::Clamp(v.Dot(n), 0.0f, 1.0f);
	float oneMinusCosVSqr = oneMinusCosV * oneMinusCosV;

	float LdotH = l.Dot(h);
	float F_D90 = 0.5f + 2.0f * LdotH * LdotH * alpha;
	float tmpResult =  Ut::INV_PI * (1.0f -F_D90+ (F_D90 - 1.0f) * oneMinusCosLSqr * oneMinusCosLSqr * oneMinusCosL) *
		(1.0f - F_D90 + (F_D90 - 1.0f) * oneMinusCosVSqr * oneMinusCosVSqr * oneMinusCosV);
	return albedo * tmpResult;
}

Vec3 Noise3D::GI::BxdfUt::LambertDiffuse(Vec3 albedo, Vec3 l, Vec3 n)
{
	return albedo  * l.Dot(n)  * Ut::INV_PI;
}

Vec3 Noise3D::GI::BxdfUt::LambertDiffuse(Vec3 albedo, float LdotN)
{
	return albedo  * LdotN  *Ut::INV_PI;
}

Vec3 Noise3D::GI::BxdfUt::OrenNayarDiffuse(Vec3 albedo, Vec3 l, Vec3 v, Vec3 n, float sigmaAngle)
{
	//[Oren-Nayar94] [pbrt-8.4.1]
	float A = 0.0f, B = 0.0f;
	BxdfUt::OrenNayarDiffuseAB(sigmaAngle, A, B);
	Vec3 diffuse = BxdfUt::OrenNayarDiffuse(albedo, l, v, n, A, B);
	return diffuse;
}

void Noise3D::GI::BxdfUt::OrenNayarDiffuseAB(float sigma, float & A, float & B)
{
	float sig2 = sigma * sigma;
	A = 1.0f - (sig2 / (2.0f * sig2 + 0.66f));
	B = 0.45f * sig2 / (sig2 + 0.09f);
}

Vec3 Noise3D::GI::BxdfUt::OrenNayarDiffuse(Vec3 albedo, Vec3 l, Vec3 v, Vec3 n, float A, float B)
{
	//(2019.4.20)this is modified. it originally needs acosf() and cosf/sinf
	float cosIn = std::abs(n.Dot(l));
	float cosOut = std::abs(n.Dot(v));
	float sinIn = sqrtf(1.0f - cosIn*cosIn);
	float sinOut = sqrtf(1.0f - cosOut*cosOut);
	float cosInOutTerm = std::max<float>(0.0f, cosIn*cosOut - sinIn *sinOut);
	float sinAngleAlpha = std::max<float>(sinIn, sinOut);//alpha = max(theta_i, theta_o)
	float cosAngleBeta = std::max<float>(cosIn, cosOut);//beta= min(theta_i, theta_o) = max(cos(theta_i),cos(theta_o))

	Vec3 diffuse = albedo *Ut::INV_PI * (A + B* cosInOutTerm * sinAngleAlpha * cosAngleBeta);
	return diffuse;
}

float Noise3D::GI::BxdfUt::D_GGX(Vec3 n, Vec3 h, float alpha)
{
	//Ref: [Walter07]Trowbridge-Reitz GGX NDF

	//(2019.4.23)be warned that when alpha-->0, the peak of GGX goes infinitely high
	//but the integral of D(h) over hemisphere will always be 1
	float alpha2 = std::max<float>(alpha*alpha,0.001f);
	float NdotH = n.Dot(h);
	if (NdotH <= 0.0f)return 0.0f;
	float denominator = std::max<float>(((NdotH *NdotH) * (alpha2 - 1.0f) + 1.0f),std::numeric_limits<float>::epsilon());
	float denominator2 = denominator * denominator * Ut::PI;
	float D = std::min<float>(alpha2 / denominator2, BxdfUt::D_GGX_SingularityMaxValue());
	return D;
}

const float Noise3D::GI::BxdfUt::D_GGX_SingularityMaxValue()
{
	//when alpha=0(flat surface), NdotH=0, GGX goes to infinity
	//(to let the integral of GGX over hemisphere remain 1)
	return 1e+6;//2^20
}

float Noise3D::GI::BxdfUt::D_Beckmann(Vec3 n, Vec3 h, float alpha)
{
	//Ref: [Walter07][Beckmann63] NDF
	float NdotH = n.Dot(h);
	if (NdotH <= 0.0f)return 0.0f;
	float NdotH2 = NdotH * NdotH;
	float NdotH4 = NdotH2 * NdotH2;
	float alpha2 = alpha * alpha;
	float denominator = Ut::PI * alpha2 * NdotH4;
	float expTerm = std::expf((NdotH2-1.0f)/(alpha2 * NdotH2));
	float D = (1.0f / denominator) *expTerm;
	return D;
}

float Noise3D::GI::BxdfUt::G_SmithGGX(Vec3 l, Vec3 v, Vec3 n, float alpha)
{
	//ref : [Walter07]
	//Smith : G=G(v)G(l)

	float alpha2 = alpha * alpha;

	//G1
	float NdotV = n.Dot(v);
	//if (NdotV <= 0.0f)return 0.0f;
	float denom1 = NdotV + sqrtf(alpha2 + (1.0f - alpha2)*(NdotV * NdotV));
	float G1 = 2.0f * NdotV / denom1;

	//G2
	float NdotL = n.Dot(l);
	//if (NdotL <= 0.0f)return 0.0f;
	float denom2 = NdotL + sqrtf(alpha2 + (1.0f - alpha2)*(NdotL*NdotL));
	if (denom2 == 0.0f)
	{
		int a = 1;
	}
	float G2 = 2.0f * NdotL / denom2;

	float G = G1 * G2;
	return G;
}

float Noise3D::GI::BxdfUt::G_SmithSchlickGGX(Vec3 l, Vec3 v, Vec3 n, float alpha)
{
	//ref : [Schlick94][Karis-SIGGRAPH-course-2013] UE4's impl(?)
	//Smith : G=G(v)G(l)

	//http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
	//UE4's remap
	float k = alpha / 2.0f;

	//G(v)
	float NdotV = n.Dot(v);
	float denom1 = NdotV * (1.0f - k) + k;
	float G1 = NdotV / denom1;

	//G(l)
	float NdotL = n.Dot(l);
	float denom2 = NdotL * (1.0f - k) + k;
	float G2 = NdotL / denom2;

	float G = G1*G2;
	return G;
}

float Noise3D::GI::BxdfUt::G_SmithBeckmann(Vec3 l, Vec3 v, Vec3 n, float alpha)
{
	//ref: [Walter07] for G_Beckmann
	//Smith : G=G(v)G(l)

	float G1 = 1.0f;
	float G2 = 1.0f;

	//G(v)
	{
		float NdotV = n.Dot(v);
		float NdotV2 = NdotV*NdotV;
		float denom1 = alpha * sqrtf(1.0f - NdotV2);
		float c1 = NdotV / denom1;

		if (c1 < 1.6f)
		{
			float c1_sqr = c1*c1;
			G1 = (3.353f* c1 + 2.181f*c1_sqr) / (1.0f + 2.276f * c1 + 2.577f*c1_sqr);
		}
	}

	//G(l)
	{
		float NdotL = n.Dot(l);
		float NdotL2 = NdotL*NdotL;
		float denom2 = alpha * sqrtf(1.0f - NdotL2);
		float c2 = NdotL2 / denom2;
		float G2 = 1.0f;
		if (c2 < 1.6f)
		{
			float c2_sqr = c2*c2;
			G2 = (3.353f* c2 + 2.181f*c2_sqr) / (1.0f + 2.276f * c2 + 2.577f*c2_sqr);
		}
	}

	float G = G1*G2;

	return G;
}

float Noise3D::GI::BxdfUt::CookTorranceSpecular(Vec3 l, Vec3 v, Vec3 n, float D, float G)
{
	return (D*G) / (4.0f*(v.Dot(n))*(l.Dot(n)));
}

Vec3 Noise3D::GI::BxdfUt::ComputeHalfVectorForRefraction(Vec3 viewVec, Vec3 lightVec, float eta_i, float eta_o, Vec3 n)
{
	//NOTE: 
	//viewVec = hitpos---->camera
	//lightVec = hitpos---->light source
	//normal points to medium with lower IOR(i.e. air)
	//Vec3 h_t = eta_i * inVec - eta_o * outVec;
	Vec3 h_t = -(eta_i * viewVec + eta_o * lightVec);//[Walter07]

	if (h_t.LengthSquared() < 0.01f)
	{
		//if ior are close to 1, then h_t will be almost a zero vector
		//and will cause huge numerical error
		h_t = (eta_i <= eta_o ? n: -n);
	}
	if (h_t != Vec3(0, 0, 0)) h_t.Normalize();
	return h_t;
}
