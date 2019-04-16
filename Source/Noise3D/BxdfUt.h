
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

			static Vec3 SchlickFresnel_Vec3(Vec3 F0, Vec3 v, Vec3 h);

			static float SchlickFresnel(float F0, Vec3 v, Vec3 h);

			static Color4f DisneyDiffuse(Color4f albedo, Vec3 v, Vec3 l, Vec3 n, Vec3 h, float alpha);

			static Color4f LambertDiffuse(Color4f albedo, Vec3 l, Vec3 n);

			static Color4f LambertDiffuse(Color4f albedo, float LdotN);

		private:

		};
	}
}