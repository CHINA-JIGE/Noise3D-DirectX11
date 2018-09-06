
/***********************************************************************

					 Spherical Harmonic Lighting Common

************************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		//Real Spherical Harmonic function. l:band index. m:coefficient index (up to band 3)
		extern float SH(int l, int m, NVECTOR3 dir);

		//Real Spherical Harmonic function (up to band 3)(theta--pitch; phi--yaw, start from z axis)
		extern float SH(int l, int m, float yaw, float pitch);

		//Real Spherical Harmonic function (infinite band, implemented with recursive formula)
		extern float SH_n(int l, int m, NVECTOR3 dir);

		//Real Spherical Harmonic function (infinite band, implemented with recursive formula)(theta--pitch; phi--yaw, start from z axis)
		extern float SH_n(int l, int m, float yaw, float pitch);

		//given 2-dimension params and return flattened linear index
		extern int SH_FlattenIndex(int l, int m);
	}
};
