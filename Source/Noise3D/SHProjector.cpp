
/***********************************************************************

		SH projector (input a spherical function like cube map, output
		convolved result of SH coefficient vector

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

void Noise3D::GI::SHProjector::Project(int highestOrderIndex, int monteCarloSampleCount, N_SH_SPHERICAL_REAL_FUNC_INTERFACE* pTargetFunc, std::vector<float>& outSHCoefficient)
{
	if (highestOrderIndex < 0) 
	{
		ERROR_MSG("SHProjector: SH order index should be positive.");
		return;
	}

	if (monteCarloSampleCount < 1)
	{
		ERROR_MSG("SHProjector: monte carlo ray sample count ought to be larger than 0.");
		return;
	}

	//n-order SH have n^2 coefficients in total
	uint32_t coefficientCount = highestOrderIndex * highestOrderIndex;
	outSHCoefficient.resize(coefficientCount);

	//select the version of Spherical Harmonic function(low order is optimized)
	//(manual Polymorphism hahahahahaha)
	float(*pSHFunc)(int l, int m, NVECTOR3 dir) =nullptr;
	if (highestOrderIndex <= 3)
	{
		pSHFunc = GI::SH;
	}
	else
	{
		pSHFunc = GI::SH_n;
	}

	//compute SH coefficients by convolving
	GI::RandomSampleGenerator randomGen;
	for (int sampleIndex = 0; sampleIndex < monteCarloSampleCount; ++sampleIndex)
	{
		NVECTOR3 dir = randomGen.UniformSpherical_Vector();

		//for every direction sample, calculate all of its SH coefficient by convolving f(x) and corresponding SH function value
		//L--band index
		for (uint32_t L = 0; L < highestOrderIndex; ++L)
		{
			//M--coefficient index inside a SH band
			for (int M = -L; M <= L; ++M)
			{
				//convolve target spherical function 'pTargetFunc f(x)' with convolution kernel pSHFunc
				//now just sum them up on sphere surface, monte-carlo integration's division will be done later
				outSHCoefficient.at(L*(L+1)+M) += pTargetFunc->Eval(dir) *  pSHFunc(L, M, dir);
			}
		}
	}

	//weight = 1.0f/(p(x)) = 1.0f/ (1.0f/4pi)) = 4pi
	constexpr float c_sampleWeight = 4.0f * MATH_PI;
	float mulFactor = c_sampleWeight / float(monteCarloSampleCount);

	//done summing up sample value, do the division of numerical integration
	for (int i = 0; i < coefficientCount; ++i)
	{
		outSHCoefficient.at(i) *= mulFactor;
	}
}
