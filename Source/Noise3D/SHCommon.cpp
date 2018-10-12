
/***********************************************************************

			Spherical Harmonic Lighting Common implementation

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;
#define SH_ASSERT(expr, prompt) if((expr)==false){ERROR_MSG(prompt);return 0.0f;}

float Noise3D::GI::SH(int l, int m, NVECTOR3 dir)
{
	SH_ASSERT(dir.Length() >= 0.01f, "SH function:  dir length is less than threshold");
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
	default: result = SH_Recursive(l,m,dir); break;
	}

	return result;
}

float Noise3D::GI::SH(int l, int m, float yaw, float pitch)
{
	NVECTOR3 vec = { sinf(yaw)*cosf(pitch), cosf(yaw)*cosf(pitch), sinf(pitch) };
	return SH(l, m, vec);
}

float Noise3D::GI::SH_Recursive(int l, int m, NVECTOR3 dir)
{
	SH_ASSERT(dir.Length() >= 0.01f, "SH function:  dir length is less than threshold");
	//normalize the dir (unit sphere assumption)
	dir.Normalize();

	float yaw = 0.0f, pitch = 0.0f;
	Ut::DirectionToYawPitch(dir,yaw,pitch);
	return SH_Recursive(l, m, yaw, pitch);
}

float Noise3D::GI::SH_Recursive(int l, int m, float yaw, float pitch)
{
	//associate legendre polynomial is implemented in a recursive way. for more detail
	//plz refer to <Spherical Harmonic Lighting: the Gritty Details>
	auto K = [](int l, int m)->float
	{
		// renormalisation constant term(normalize Associated Legendre Polynomial) for SH function
		float temp = ((2.0f*l + 1.0f)*Ut::Factorial(l - m)) / (4.0f*Ut::PI*Ut::Factorial(l + m));
		return sqrtf(temp);
	};

	//theta--pitch, phi--yaw
	const float sqrt2 = sqrtf(2.0f);
	if (m == 0)return K(l, 0) * GI::AssociatedLegendrePolynomial(l, 0, cos(pitch-Ut::PI/2.0f));
	else if (m > 0)return sqrt2 * K(l, m) * cosf(m*yaw) * GI::AssociatedLegendrePolynomial(l, m, cos(pitch + Ut::PI / 2.0f));
	else return sqrt2 * K(l, -m) * sinf(-m*yaw)* GI::AssociatedLegendrePolynomial(l, -m, cos(pitch + Ut::PI / 2.0f));
	return 0.0f;
}

int Noise3D::GI::SH_FlattenIndex(int l, int m)
{
	return l*(l + 1) + m;
}

float Noise3D::GI::AssociatedLegendrePolynomial(int l, int m, float x)
{
	if (m<0)
	{
		ERROR_MSG("AssociatedLegendrePolynomial: m must be non-negative");
		return 0.0f;
	}
	//there is 3 recurrence relation to calculate given ALP, (then ALP is used to compute SH function)
	//***to calculate diagonal elements
	//1. P_m_m = (-1)^m * (2m-1)!! * (sqrt(1-x^2)^m)  (WARNING, (2m-1)!! = 1 * 3 * 5 * .... (2m-1) , this is double factorial
	//***to calculate second top diagonal elements
	//2. P_m_(m+1) = x(2m+1)P_m_m
	//***to calculate every column
	//3.  (l-m)P_m_l = x(2l-1)P_m_(l-1)- (l+m-1)P_m_(l-2)

	//***stage 1*** if(l==m), return in advance (use eq. 1)
	float P_m_m = 1.0f;//p_0_0=1
	if (m > 0)
	{
		float sqrtTermBase = sqrt(1 - x*x);
		float doubleFactorialTermBase = 1.0f;
		for (int i = 1; i <= m; ++i)
		{
			P_m_m *= (-doubleFactorialTermBase * sqrtTermBase);
			doubleFactorialTermBase += 2.0f;
		}
	}
	if (l == m)return P_m_m;

	//***stage 2*** if(l==m+1), return in advance (use eq. 2)
	float P_m_mp1 = x * (2 * m + 1) * P_m_m;
	if (l == m + 1) return P_m_mp1;

	//***stage 3***we now have the first and second term in this column (in which m are the same),
	// iterate to get target band's element (use eq. 3)
	float P_result = 0.0f;
	float P_termBandMinu2 = P_m_m;
	float P_termBandMinu1 = P_m_mp1;
	for (int tmpL = m + 2; tmpL <= l; ++tmpL)
	{
		P_result = (2.0f * float(tmpL) - 1.0f) * x * P_termBandMinu1 - float(tmpL - m + 1) * P_termBandMinu2;
		//prepare for next iteration
		P_termBandMinu2 = P_termBandMinu1;
		P_termBandMinu1 = P_result;
	}

	return P_result;
}
