/***********************************************************************

								Random Sample Generator

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

std::default_random_engine Noise3D::GI::RandomSampleGenerator::mRandomEngine = std::default_random_engine();

std::uniform_real_distribution<float> Noise3D::GI::RandomSampleGenerator::mCanonicalDist = std::uniform_real_distribution<float>(0.0, 1.0f);

Noise3D::GI::RandomSampleGenerator::RandomSampleGenerator()
{
}

float Noise3D::GI::RandomSampleGenerator::CanonicalReal()
{
	return mCanonicalDist(mRandomEngine);
}

NVECTOR3 Noise3D::GI::RandomSampleGenerator::UniformSpherical_Vector()
{
	float var1 = mCanonicalDist(mRandomEngine);
	float var2 = mCanonicalDist(mRandomEngine);

	//according to <gritty detail> and my own paper note
	//we can generate random points that uniformly distribute on a sphere 
	//by mapping a pair of canonical variable (x,y) to azimulthal angle (theta, phi) using formula:
	//(theta,phi) <-- (2arccos(sqrt(1-x)) , 2\pi * y)
	//or
	//(theta,phi)<-- (arccos(1-2x),2\pi*y)
	//but obviously, arccos(1-2x) is a alittle more computationally efficient
	float theta = acosf(1 - 2.0f * var1);
	float phi = 2 * MATH_PI * var2;

	//this parameterization is different from the common one
	NVECTOR3 out;
	out.x = sinf(theta)*cosf(phi);
	out.y = cos(theta);
	out.z = sinf(theta)*sinf(phi);
	return out;
}

NVECTOR2 Noise3D::GI::RandomSampleGenerator::UniformSpherical_Azimulthal()
{
	std::uniform_real_distribution<float> canonicalDist(0.0f, 1.0f);
	float var1 = canonicalDist(mRandomEngine);
	float var2 = canonicalDist(mRandomEngine);

	//according to <gritty detail> and my own paper note
	//we can generate random points that uniformly distribute on a sphere 
	//by mapping a pair of canonical variable (x,y) to azimulthal angle (theta, phi) using formula:
	//(theta,phi) <-- (2arccos(sqrt(1-x)) , 2\pi * y)
	//or
	//(theta,phi)<-- (arccos(1-2x),2\pi*y)
	//but obviously, arccos(1-2x) is a alittle more computationally efficient
	float theta = acosf(1 - 2.0f * var1);
	float phi = 2 * MATH_PI * var2;
	return NVECTOR2(theta, phi);
}
