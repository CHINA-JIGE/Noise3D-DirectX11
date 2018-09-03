
/***********************************************************************

					 Spherical Harmonic Lighting Common

************************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		/*enum NOISE_SH_ORDER
		{
			ORDER_0=0,//1 coefficient
			ORDER_1=1,//4 coefficient
			ORDER_2=2,//9 coefficient
			ORDER_3=3,//16 coefficient
		};*/


		//Real Spherical Harmonic function. l:band index. m:coefficient index (up to band 3)
		extern float SH(int l, int m, NVECTOR3 dir);

		//Real Spherical Harmonic function (up to band 3)(theta--pitch, phi--yaw)
		extern float SH(int l, int m, float theta, float phi);

		//Real Spherical Harmonic function (infinite band, implemented with recursive formula)
		extern float SH_n(int l, int m, NVECTOR3 dir);

		//Real Spherical Harmonic function (infinite band, implemented with recursive formula)
		extern float SH_n(int l, int m, float theta, float phi);

		//extern void SH_Rotate();
	}
};
