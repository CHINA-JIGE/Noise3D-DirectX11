
/***********************************************************

						GI: BxDF Utility functions
				(Bidirectional xx Distribution Function)

**************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		class /*_declspec(dllexport)*/ BxdfUt
		{
		public:

			/*
			TERMS:
			alpha : measure of roughness. in UE4, alpha=Roughness^2
			l : light direction
			v : view direciton
			h : half vector (of l & v), which is microfacet's normal
			n : normal (of macrosurface)
			albedo : 'diffuse color' of material
			*/

			static Vec3 SchlickFresnel_Vec3(Vec3 F0, Vec3 v, Vec3 h);

			static float SchlickFresnel(float F0, Vec3 v, Vec3 h);

			static Vec3 DisneyDiffuse(Vec3 albedo, Vec3 v, Vec3 l, Vec3 n, Vec3 h, float alpha);

			static Vec3 LambertDiffuse(Vec3 albedo, Vec3 l, Vec3 n);

			static Vec3 LambertDiffuse(Vec3 albedo, float LdotN);

			static Vec3 OrenNayarDiffuse(Vec3 albedo, Vec3 l, Vec3 v, Vec3 n, float sigmaAngle);

			static void OrenNayarDiffuseAB(float sigma, float& A, float& B);

			static Vec3 OrenNayarDiffuse(Vec3 albedo, Vec3 l, Vec3 v, Vec3 n, float A, float B);

			static float D_GGX(Vec3 n, Vec3 h, float alpha);//NDF, Normal distribution function

			static float D_Beckmann(Vec3 n, Vec3 h, float alpha);//NDF, Normal distribution function

			//Smith's method: G=G0(l)G0(v)
			static float G_SmithGGX(Vec3 l, Vec3 v, Vec3 n, float alpha);

			static float G_SmithSchlickGGX(Vec3 l, Vec3 v, Vec3 n, float alpha);

			static float G_SmithBeckmann(Vec3 l, Vec3 v, Vec3 n, float alpha);

			//WARNING: this Cook-Torrance term computes WITHOUT Fresnel Term
			static float CookTorranceSpecular(Vec3 l, Vec3 v, Vec3 n, float D, float G);

		private:

		};
	}
}