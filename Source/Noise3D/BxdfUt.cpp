/****************************************************

						BxDF Utility Functions

*****************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Vec3 Noise3D::GI::BxdfUt::SchlickFresnel_Vec3(Vec3 F0, Vec3 v, Vec3 h)
{
	//v:view, l:light, n:normal, h:half vector, alpha:roughness^2
	//F = F0 + (1-F0)(1-(v dot H))^5
	float OneMinusCos = Ut::Clamp(v.Dot(h),0.0f,1.0f);
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

Color4f Noise3D::GI::BxdfUt::DisneyDiffuse(Color4f albedo, Vec3 v, Vec3 l, Vec3 n, Vec3 h, float alpha)
{
	albedo.w = 0.0f;
	//v:view, l:light, n:normal, h:half vector, alpha:roughness^2
	//disney diffuse = ( albedo/pi ) *(1+ (F_D90-1)(1- (v dot n))^5)*(1+ (F_D90-1)(1- (l dot n))^5)
	float oneMinusCosL = 1.0f - Ut::Clamp(l.Dot(n), 0.0f, 1.0f);
	float oneMinusCosLSqr = oneMinusCosL * oneMinusCosL;
	float oneMinusCosV = 1.0f - Ut::Clamp(v.Dot(n), 0.0f, 1.0f);
	float oneMinusCosVSqr = oneMinusCosV * oneMinusCosV;

	float LdotH = l.Dot(h);
	float F_D90 = 0.5f + 2.0f * LdotH * LdotH * alpha;
	float tmpResult =  Ut::INV_PI * (1.0f + (F_D90 - 1.0f) * oneMinusCosLSqr * oneMinusCosLSqr * oneMinusCosL) *
		(1.0f + (F_D90 - 1.0f) * oneMinusCosVSqr * oneMinusCosVSqr * oneMinusCosV);
	return albedo * tmpResult;
}
