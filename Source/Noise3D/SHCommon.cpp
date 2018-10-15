
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
	//NVECTOR3 vec = { sinf(yaw)*cosf(pitch), cosf(yaw)*cosf(pitch), sinf(pitch) };
	NVECTOR3 vec=Ut::YawPitchToDirection(yaw, pitch);
	return SH(l, m, vec);
}

float Noise3D::GI::SH_Recursive(int l, int m, NVECTOR3 dir)
{
	SH_ASSERT(dir.Length() >= 0.01f, "SH function:  dir length is less than threshold");
	//normalize the dir (unit sphere assumption)
	dir.Normalize();

	float yaw = 0.0f, pitch = 0.0f;
	// [can't use here] Ut::DirectionToYawPitch(dir,yaw,pitch);
	//in wikipedia 	//https://en.wikipedia.org/wiki/Table_of_spherical_harmonics#Real_spherical_harmonics 
	//and <gritty detail> paper, the SH function's spherical parameterization is
	// (sin\theta cos\phi, sin\theta sin\phi, cos\theta) ---> (x,y,z)

	//and we use a similar one
	// (sin\theta cos\phi,  cos\theta, sin\theta sin\phi) ---> (x,y,z)
	// NOTE: and Condon-Shortley phase (-1)^m in 
	pitch = acosf(dir.y);//dir.z
	yaw = atan2(dir.z, dir.x);//z x

	return SH_Recursive(l, m, yaw, pitch);
}

float Noise3D::GI::SH_Recursive(int l, int m, float yaw, float pitch)
{
	//associate legendre polynomial is implemented in a recursive way. for more detail
	//plz refer to <Spherical Harmonic Lighting: the Gritty Details>
	auto K = [](int l, int m)->float
	{
		// renormalisation constant term(normalize Associated Legendre Polynomial) for SH function
		float temp = (float(2*l + 1)*Ut::Factorial64(l - m)) / (4.0f*Ut::PI* float(Ut::Factorial64(l + m)));
		return sqrtf(temp);
	};

	//theta--pitch, phi--yaw
	const float sqrt2 = sqrtf(2.0f);
	if (m == 0)return K(l, 0) * GI::AssociatedLegendrePolynomial_Recursive(l, 0, cosf(pitch));
	else if (m > 0)return sqrt2 * K(l, m) * cosf(m*yaw) * GI::AssociatedLegendrePolynomial_Recursive(l, m, cosf(pitch));
	else return sqrt2 * K(l, -m) * sinf(-m*yaw )* GI::AssociatedLegendrePolynomial_Recursive(l, -m, cosf(pitch));// + Ut::PI / 2.0f
	return 0.0f;
}

int Noise3D::GI::SH_FlattenIndex(int l, int m)
{
	return l*(l + 1) + m;
}

float Noise3D::GI::AssociatedLegendrePolynomial_Recursive(int l, int m, float x)
{
	SH_ASSERT(l >= 0, "AssociatedLegendrePolynomial: l must be non-negative");
	SH_ASSERT(m>=0,"AssociatedLegendrePolynomial: m must be non-negative");
	SH_ASSERT(m <= l, "AssociatedLegendrePolynomial: coefficient index m is out of range");

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
		float doubleFactorialTermBase = 1.0f;// n!!
		for (int i = 1; i <= m; ++i)
		{
			//ignore Condon-Shortley phase (-1.0f)^m for unification with the hardcoded SH function (which ignores this (-1)^m term)
			P_m_m *= 1.0f * (doubleFactorialTermBase * sqrtTermBase);
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
		P_result = (float(2 * tmpL - 1) * x * P_termBandMinu1 - float(tmpL + m - 1) * P_termBandMinu2)/float(tmpL-m);
		//prepare for next iteration
		P_termBandMinu2 = P_termBandMinu1;
		P_termBandMinu1 = P_result;
	}

	return P_result;
}

float Noise3D::GI::AssociatedLegendrePolynomial_LowOrder(int l, int m, float x)
{
	SH_ASSERT(l >= 0, "ALP: l must be non-negative");
	SH_ASSERT(l <= 4, "ALP: this is low order version, maximum order 4 (0~4).");
	SH_ASSERT(m >= 0, "ALP: m must be non-negative");
	SH_ASSERT(m <= l, "ALP: coefficient index m is out of range");
	
	//P00 P10 P11 P20 P21 P22 ....
	int flattenedIndex = l * (l + 1) / 2 + m;
	float result = 0.0f;
	switch (flattenedIndex)
	{
		case 0: result = 1.0f; break;

		case 1: result = x; break;
		case 2: result = -sqrtf(1.0f - x*x); break;

		case 3: result = 0.5f * (3 * x * x - 1.0f); break;
		case 4: result = -3.0f * x * sqrtf(1 - x*x); break;
		case 5: result = 3.0f * (1 - x*x); break;

		case 6: result = 0.5f * x * (5 * x * x - 3.0f); break;
		case 7: result = 1.5f* (1 - 5.0f *x*x)*sqrtf(1 - x*x); break;
		case 8: result = 15.0f * x * (1 - x*x); break;
		case 9: result = -15.0f * pow(sqrtf(1 - x*x), 3); break;

		case 10: result = 0.125f * (35.0f*x*x*x*x - 30.0f *x*x + 3.0f); break;
		case 11: result = 2.5f * x * (3.0f - 7.0f*x*x)*sqrtf(1 - x*x); break;
		case 12: result = 7.5f * (7.0f*x*x - 1.0f)*(1 - x*x); break;
		case 13: result = -105.0f * x * pow(sqrtf(1 - x*x), 3); break;
		case 14: result = 105.0f * (1 - x*x)* (1 - x*x); break;

	}

	return result;
}
