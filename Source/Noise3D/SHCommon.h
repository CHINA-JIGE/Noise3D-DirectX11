
/***********************************************************************

					 Spherical Harmonic Lighting Common

************************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		//Real Spherical Harmonic function evaluation. l:band index. m:coefficient index (band 0 ~3)
		extern float SH(int l, int m, Vec3 dir);

		//Real Spherical Harmonic function evaluation(band 0~3)(theta--pitch; phi--yaw, start from z axis)
		extern float SH(int l, int m, float yaw, float pitch);

		//Real Spherical Harmonic function evaluation(infinite band, implemented with recursive formula)
		extern float SH_Recursive(int l, int m, Vec3 dir);

		//Real Spherical Harmonic function evaluation(infinite band, implemented with recursive formula)(theta--pitch; phi--yaw, start from z axis)
		extern float SH_Recursive(int l, int m, float yaw, float pitch);

		//given 2-dimension params and return flattened linear index
		extern int SH_FlattenIndex(int l, int m);

		//SH normalization term K
		extern float SH_NormalizationTermK(int l, int m);

		//associated legendre polynomial will be used by SH function
		extern float AssociatedLegendrePolynomial_Recursive(int l, int m, float x);

		//associated legendre polynomial will be used by SH function
		extern float AssociatedLegendrePolynomial_LowOrder(int l, int m, float x);

	}
};
