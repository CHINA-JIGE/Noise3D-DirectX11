
/***********************************************************************

								h£ºSHVector

		Desc: Spherical Harmonic(SH) Lighting is a kind of Precomputed Radiance Transfer (PRT) 
		technique. SH based techniques project spherical function into frequency domain
		as Fourier Transform do to 1D function. For more detail, plz refer to
		https://github.com/CHINA-JIGE/JiGe-NotesOfVisualComputing
		for more notes/papers/ppt

		SHVector is a class performing SH coefficients operation like project/integrate/evaluate.
		n-order SH coefficient(n^2 coefficients in total) can be generated from an input spherical function. 
		Spherical function can be in a variety of forms, like CubeMap. 
		But when it comes to the format of spherical function, I'll leave the discretion to user.

************************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		//an interface that user can implement to pass in as the input of data(spherical function)
		template <typename T>
		struct ISphericalFunc 
		{
			virtual T Eval(const NVECTOR3& dir)=0;	
		};

		class ISphericalTextureSampler : public ISphericalFunc<float>
		{

		};

		//a general Spherical Harmonic coefficent vector class (several operation on SH coefficients are available)
		//template <typename T>
		class /*_declspec(dllexport)*/ SHVector
		{
		public:

			SHVector();

			//low order SH functions is optimized with hardcoded terms (0 ~ 3 orders, SH() )
			//higher order SH functions is implemented using recursive method of Spherical Harmonic Terms (SH_n())
			void Project(int highestOrder, int monteCarloSampleCount, ISphericalFunc<float>* pTargetFunc);

			//reconstruct SH signal and evaluate spherical function value in given direction
			float Eval(NVECTOR3 dir);

			//perform SH-based integration (common usage is integration between spherical irradiance function
			//& transfer function to get the final illuminated color) (Actually a dot product of 2 SH vectors)
			//if the 2 operand's dimension are not equal, 0 will be used to pad to calculate dot product
			float Integrate(const SHVector& rhs);

			//
			void Rotate(float theta, float phi);

			//get SH coefficient
			void GetCoefficients(std::vector<float>& outList);

			//get current SH order
			int GetOrder() const;

			//manually set 
			void SetCoefficients(int highestOrder, const std::vector<float>& list);

		private:
		
			//determine which version of SH function (pointer) to use according to SH order
			void mFunction_DetermineSHFuncVersion();

			//manual polymorphism. choose SH function's version according to SH order
			float(*m_pSHFunc)(int l, int m, NVECTOR3 dir);

			//SH order/highest band index
			int mOrder;

			//SH coefficients
			std::vector<float> mCoefficients;

		};


	}
};