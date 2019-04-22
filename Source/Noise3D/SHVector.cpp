
/***********************************************************************

													SH Vector

************************************************************************/

#include "Noise3D.h"
#include "Noise3D_InDevHeader.h"

using namespace Noise3D;

Noise3D::GI::SHVector::SHVector():
	mOrder(0),
	mIsInitialized(false)
{

}

void Noise3D::GI::SHVector::Project(int highestOrderIndex, int monteCarloSampleCount, ISphericalFunc<Color4f>* pTargetFunc)
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
	int coefficientCount = (highestOrderIndex+1) * (highestOrderIndex+1);//0-based index
	mCoefficients.resize(coefficientCount);

	//compute SH coefficients by convolving
	GI::RandomSampleGenerator randomGen;
#pragma omp parallel for
	for (int sampleIndex = 0; sampleIndex < monteCarloSampleCount; ++sampleIndex)
	{
		Vec3 dir = randomGen.UniformSphericalVec();

		//for every direction sample, calculate all of its SH coefficient by convolving f(x) and corresponding SH function value
		//L--band index
		for (int L = 0; L <= highestOrderIndex; ++L)
		{
			//M--coefficient index inside a SH band
			for (int M = -L; M <= L; ++M)
			{
				//convolve target spherical function 'pTargetFunc f(x)' with convolution kernel pSHFunc
				//now just sum them up on sphere surface, monte-carlo integration's division will be done later
				Color4f color = pTargetFunc->Eval(dir);
				float sphFunc = GI::SH(L, M, dir);
				mCoefficients.at(SH_FlattenIndex(L, M)) += color * sphFunc;
			}
		}
	}

	//weight = 1.0f/(p(x)) = 1.0f/ (1.0f/4pi)) = 4pi
	constexpr float c_sampleWeight = 4.0f * Ut::PI;
	float mulFactor = c_sampleWeight / float(monteCarloSampleCount);

	//done summing up sample value, do the division of numerical integration
	for (int i = 0; i < coefficientCount; ++i)
	{
		mCoefficients.at(i) *= mulFactor;
	}

	if (!mIsInitialized)
	{
		mRotatedCoefficients = mCoefficients;
	}

	mIsInitialized = true;
}

Color4f Noise3D::GI::SHVector::Eval(Vec3 dir)
{
	Vec4 result = { 0,0,0,0};
	for (int L = 0; L <= mOrder; ++L)
	{
		//M--coefficient index inside a SH band
		for (int M = -L; M <= L; ++M)
		{
			//result = sum(coefficient * SH_basis)
			result += mCoefficients.at(SH_FlattenIndex(L, M)) * GI::SH(L, M, dir);
		}
	}
	result = Noise3D::Ut::Clamp(result, Vec4(0, 0, 0, 0), Vec4(1.0f, 1.0f, 1.0f,1.0f));
	return result;
}

Color4f Noise3D::GI::SHVector::EvalRotated(Vec3 dir)
{
	Vec4 result = { 0,0,0,0 };
	for (int L = 0; L <= mOrder; ++L)
	{
		//M--coefficient index inside a SH band
		for (int M = -L; M <= L; ++M)
		{
			//result = sum(coefficient * SH_basis)
			result += mRotatedCoefficients.at(SH_FlattenIndex(L, M)) * GI::SH(L, M, dir);
		}
	}
	result = Noise3D::Ut::Clamp(result, Vec4(0, 0, 0, 0), Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	return result;
}

Color4f Noise3D::GI::SHVector::Integrate(const SHVector& rhs)
{
	//(a1,a2,a3,a4,.....,0,0,0)
	//dot
	//(b1,b2,b3,b4,....,b5,b6,b7)
	if (mOrder != rhs.GetOrder())
	{
		WARNING_MSG("SHVector: rhs dimension not match. Extra dimension will be neglected.");
	}

	//float result = 0.0f;
	Color4f result = { 0,0,0,0 };
	int minOrder = min(mOrder, rhs.GetOrder());
	int coefficientCount = minOrder * minOrder;
	for (int i = 0; i < coefficientCount; ++i)
	{
		result += mCoefficients.at(i) * rhs.mCoefficients.at(i);
	}

	return result;
}

void Noise3D::GI::SHVector::SetRotation(RigidTransform t)
{
	GI::SHRotationWignerMatrix mat(mOrder);
	mRotatedCoefficients.resize(mCoefficients.size());
	mat.Multiply(t,mCoefficients, mRotatedCoefficients);
}

void Noise3D::GI::SHVector::GetCoefficients(std::vector<Color4f>& outList)
{
	outList = mCoefficients;
}

void Noise3D::GI::SHVector::GetRotatedCoefficients(std::vector<Color4f>& outList)
{
	outList = mRotatedCoefficients;
}

int Noise3D::GI::SHVector::GetOrder()const
{
	return mOrder;
}

void Noise3D::GI::SHVector::SetCoefficients(int highestOrderIndex, const std::vector<Color4f>& list)
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

bool Noise3D::GI::SHVector::IsInitialized() const
{
	return mIsInitialized;
}


/**********************************************************

									PRIVATE

**********************************************************/