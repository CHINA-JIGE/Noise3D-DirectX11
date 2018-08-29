
/***********************************************************************

					 Spherical Harmonic Lighting Common

************************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		enum N_SH_ORDER
		{
			ORDER_0=0,//1 coefficient
			ORDER_1=1,//4 coefficient
			ORDER_2=2,//9 coefficient
			ORDER_3=3,//16 coefficient
		};


		//Real Spherical Harmonic function. l:band index. m:coefficient index
		float SH(int l, int m, NVECTOR3 dir);

		//Real Spherical Harmonic function
		float SH(int l, int m, float theta, float phi);

	}
};
