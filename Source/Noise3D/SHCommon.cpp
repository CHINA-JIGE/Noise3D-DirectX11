
/***********************************************************************

			Spherical Harmonic Lighting Common implementation

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

float Noise3D::GI::SH(int l, int m, NVECTOR3 dir)
{
#define SH_ASSERT(expr, prompt) if(!expr){ERROR_MSG(prompt);return 0.0f;}

	SH_ASSERT(dir.Length >= 0.01f, "SH function:  dir length is less than threshold");
	SH_ASSERT(l <= 3, "SH function: not supported if band index is larger than 3");
	SH_ASSERT(l >= 0, "SH function: band index l should be positive");
	SH_ASSERT(abs(m) <= l, "SH function: coefficient index m is out of range");

	//normalize the dir (unit sphere assumption)
	dir.Normalize();

	//scale factor of the first 4 bands' Real SH terms (0~3)
	//https://en.wikipedia.org/wiki/Table_of_spherical_harmonics#Real_spherical_harmonics
	static constexpr float shTermFactor[16] = 
	{
		//band 0
		0.28209479177f,/*(1/2)*sqrt(1/pi)*/
		//band 1
		0.4886025119f,/*sqrt(3/4pi)*/					0.4886025119f,/*sqrt(3/4pi)*/		0.4886025119f,/*sqrt(3/4pi)*/
		//band 2
		1.09254843059f,/*(1/2)sqrt(15/pi)*/		1.09254843059f,/*(1/2)sqrt(15/pi)*/		0.31539156525f,/*(1/4)sqrt(5/pi)*/		
		1.09254843059f,/*(1/2)sqrt(15/pi)*/		0.54627421529f,/*(1/4)sqrt(15/pi)*/
		//band3 
		0.59004358992f,/*(1/4)sqrt(35/2pi)*/		2.89061144264f,/*(1/2)sqrt(105/pi)*/		0.45704579946f,/*(1/4)sqrt(21/2pi)*/		
		0.37317633259f,/*(1/4)sqrt(7/pi)*/			0.45704579946f,/*(1/4)sqrt(21/2pi)*/		1.44530572132f,/*(1/4)sqrt(105/pi)*/
		0.59004358992f,/*(1/4)sqrt(35/2pi)*/
	};

	//map 2-dimension index to flattened array index(described in the <Gritty Detail> paper)
	int index = SH_FlattenIndex(l,m);
	float result = 0.0f;
	float x = dir.x, y = dir.y, z = dir.z;

	//because dir is normalized, thus vector length r can be neglected here
	switch (index)
	{
	case 0: result = shTermFactor[0]; break;

	//WARNING: the wikipedia's m subscript seems to have a little problem in Real SH's band 1
	//but after checking related formula in <gritty detail> and  <Efficient Representation of Irradiance Environment Map>
	//seem that Y_1-1=0.488603y, y_10=0.488603z, y_11=0.488603x
	case 1: result = shTermFactor[1] * y;	break;
	case 2: result = shTermFactor[2] * z;	break;
	case 3: result = shTermFactor[3] * x;	break;

	case 4: result = shTermFactor[4] * x * y;	break;
	case 5: result = shTermFactor[5] * y * z;	break;
	case 6: result = shTermFactor[6] * (-x*x - y*y + 2*z*z);	break;
	case 7: result = shTermFactor[7] * z*x;	break;
	case 8: result = shTermFactor[8] * (x*x - y*y);	 break;

	case 9: result = shTermFactor[9] * (3*x*x - y*y) * y;	break;
	case 10: result = shTermFactor[10] * x*y*z; break;
	case 11: result = shTermFactor[11] * y * (4*z*z - x*x - y*y); break;
	case 12: result = shTermFactor[12] * z * (2*z*z - 3*x*x - 3*y*y); break;
	case 13: result = shTermFactor[13] * x * (4*z*z - x*x - y*y); break;
	case 14: result = shTermFactor[14] * (x*x - y*y) *z; break;
	case 15: result = shTermFactor[15] * (x*x - 3*y*y)*x; break;
	default: result = 0.0f; ERROR_MSG("SH function: not supported if band index is larger than 3!"); break;
	}

	return result;
}

float Noise3D::GI::SH(int l, int m, float theta, float phi)
{
	NVECTOR3 vec = { sinf(theta)*cosf(phi),sinf(theta)*sinf(phi), cos(theta) };
	return SH(l, m, vec);
}

float Noise3D::GI::SH_n(int l, int m, NVECTOR3 dir)
{
	ERROR_MSG("NOT Implemented!");
	return 0.0f;
}

float Noise3D::GI::SH_n(int l, int m, float theta, float phi)
{
	ERROR_MSG("NOT Implemented!");
	return 0.0f;
}

int Noise3D::GI::SH_FlattenIndex(int l, int m)
{
	return l*(l + 1) + m;
}
