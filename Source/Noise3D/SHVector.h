
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
		//a general Spherical Harmonic coefficent vector class (several operation on SH coefficients are available)
		//template <typename T> but if i use template in early dev phase, i can't take advantage of intelli-sense
		class /*_declspec(dllexport)*/ SHVector
		{
		public:

			SHVector();

			//low order SH functions is optimized with hardcoded terms (0 ~ 3 orders, SH() )
			//higher order SH functions is implemented using recursive method of Spherical Harmonic Terms (SH_Recursive())
			void Project(int highestOrderIndex, int monteCarloSampleCount, ISphericalFunc<Color4f>* pTargetFunc);

			//reconstruct SH signal and evaluate spherical function value in given direction
			Color4f Eval(Vec3 dir);

			//reconstruct rotated SH signal and evaluate spherical function value in given direction
			Color4f EvalRotated(Vec3 dir);

			//perform SH-based integration (common usage is integration between spherical irradiance function
			//& transfer function to get the final illuminated color) (Actually a dot product of 2 SH vectors)
			//if the 2 operand's dimension are not equal, 0 will be used to pad to calculate dot product
			Color4f Integrate(const SHVector& rhs);

			//make use of SHRotation class. Note that we don't
			void SetRotation(RigidTransform t);

			//get SH coefficient
			void GetCoefficients(std::vector<Color4f>& outList);

			//get Rotated copy of SH coefficients
			void GetRotatedCoefficients(std::vector<Color4f>& outList);

			//get current SH order
			int GetOrder() const;

			//manually set coefficient (rgba, 4 channels, 1 Vec4 stands for 1 coefficient in RGBA channel seperately)
			void SetCoefficients(int highestOrderIndex, const std::vector<Color4f>& list);

			//has it been initialized by SH Projection?
			bool IsInitialized() const;

		private:

			//init by SH Projection
			bool mIsInitialized;

			//SH order/highest band index
			int mOrder;

			//SH coefficients
			//std::vector<float> mCoefficients;
			std::vector<Color4f> mCoefficients;
			std::vector<Color4f> mRotatedCoefficients;//rotation might cause accumulated numerical errors!! so i split it up.
		};


	}
};