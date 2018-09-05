
/***********************************************************************

													SH Vector

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Noise3D::GI::SHVector::SHVector():
	mOrder(0),
	m_pSHFunc(nullptr)
{

}

void Noise3D::GI::SHVector::Project(int highestOrderIndex, int monteCarloSampleCount, ISphericalFunc<NVECTOR3>* pTargetFunc)
{
	if (highestOrderIndex < 0)
	{
		ERROR_MSG("SHVector: SH order index should be positive.");
		return;
	}

	if (monteCarloSampleCount < 1)
	{
		ERROR_MSG("SHVector: monte carlo ray sample count ought to be larger than 0.");
		return;
	}

	//n-order SH have n^2 coefficients in total
	mOrder = highestOrderIndex;
	int coefficientCount = highestOrderIndex * highestOrderIndex;
	mCoefficients.resize(coefficientCount);

	//select the version of Spherical Harmonic function(low order is optimized)
	mFunction_DetermineSHFuncVersion();

	//compute SH coefficients by convolving
	GI::RandomSampleGenerator randomGen;
	for (int sampleIndex = 0; sampleIndex < monteCarloSampleCount; ++sampleIndex)
	{
		NVECTOR3 dir = randomGen.UniformSpherical_Vector();

		//for every direction sample, calculate all of its SH coefficient by convolving f(x) and corresponding SH function value
		//L--band index
		for (int L = 0; L <= highestOrderIndex; ++L)
		{
			//M--coefficient index inside a SH band
			for (int M = -L; M <= L; ++M)
			{
				//convolve target spherical function 'pTargetFunc f(x)' with convolution kernel pSHFunc
				//now just sum them up on sphere surface, monte-carlo integration's division will be done later
				mCoefficients.at(SH_FlattenIndex(L, M)) += pTargetFunc->Eval(dir) *  m_pSHFunc(L, M, dir);
			}
		}
	}

	//weight = 1.0f/(p(x)) = 1.0f/ (1.0f/4pi)) = 4pi
	constexpr float c_sampleWeight = 4.0f * MATH_PI;
	float mulFactor = c_sampleWeight / float(monteCarloSampleCount);

	//done summing up sample value, do the division of numerical integration
	for (int i = 0; i < coefficientCount; ++i)
	{
		mCoefficients.at(i) *= mulFactor;
	}
}

NVECTOR3 Noise3D::GI::SHVector::Eval(NVECTOR3 dir)
{
	//float result = 0.0f;
	NVECTOR3 result = { 0,0,0 };
	for (int L = 0; L < mOrder; ++L)
	{
		//M--coefficient index inside a SH band
		for (int M = -L; M <= L; ++M)
		{
			result += mCoefficients.at(SH_FlattenIndex(L, M)) * m_pSHFunc(L, M, dir);
		}
	}
	return result;
}


NVECTOR3 Noise3D::GI::SHVector::Integrate(const SHVector& rhs)
{
	//(a1,a2,a3,a4,.....,0,0,0)
	//dot
	//(b1,b2,b3,b4,....,b5,b6,b7)
	if (mOrder != rhs.GetOrder())
	{
		WARNING_MSG("SHVector: rhs dimension not match. Extra dimension will be neglected.");
	}

	//float result = 0.0f;
	NVECTOR3 result = { 0,0,0 };
	int minOrder = min(mOrder, rhs.GetOrder());
	int coefficientCount = minOrder * minOrder;
	for (int i = 0; i < coefficientCount; ++i)
	{
		result += mCoefficients.at(i) * rhs.mCoefficients.at(i);
	}

	return result;
}

void Noise3D::GI::SHVector::GetCoefficients(std::vector<NVECTOR3>& outList)
{
	outList = mCoefficients;
}

int Noise3D::GI::SHVector::GetOrder()const
{
	return mOrder;
}

void Noise3D::GI::SHVector::SetCoefficients(int highestOrderIndex, const std::vector<NVECTOR3>& list)
{
	//n order, n^2 coefficient
	if (highestOrderIndex * highestOrderIndex != list.size())
	{
		ERROR_MSG("SHVector: n order SH vector has n^2 coefficients, input param not match!");
		return;
	}

	mOrder = highestOrderIndex;
	mCoefficients = list;
}


/**********************************************************

									PRIVATE

**********************************************************/

void Noise3D::GI::SHVector::mFunction_DetermineSHFuncVersion()
{
	//(manual Polymorphism hahahahahaha), redirect SH function pointer
	if (mOrder <= 3)
	{
		m_pSHFunc = GI::SH;
	}
	else
	{
		m_pSHFunc = GI::SH_n;
	}
}
