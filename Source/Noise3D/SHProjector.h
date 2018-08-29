
/***********************************************************************

								h£ºSHProjector

		Desc: Spherical Harmonic(SH) Lighting is a kind of Precomputed Radiance Transfer (PRT) 
		technique. SH based techniques project spherical function into frequency domain
		as Fourier Transform do to 1D function. For more detail, plz refer to
		https://github.com/CHINA-JIGE/JiGe-NotesOfVisualComputing
		for more notes/papers/ppt

		SHProjector is a utility class to generate n-order SH coefficient(n^2 coefficients in total)
		from an input spherical function. Spherical function can be in a variety of forms,
		like CubeMap.

************************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{

		//an interface that user can implement to pass in as the input of light source
		struct N_SH_SPHERICAL_FUNC_INTERFACE
		{
			virtual NColor4f Eval(const NVECTOR3& dir)=0;
		};


		//a general Spherical Harmonic projector of a given spherical function
		class /*_declspec(dllexport)*/ SHProjector
		{
		public:

			//low order SH functions is optimized with hardcoded terms
			void Project_LowOrder(NOISE_SH_ORDER shOrder, uint32_t monteCarloSampleCount, N_SH_SPHERICAL_FUNC_INTERFACE* pTargetFunc, std::vector<float>& outSHCoefficient);

			//SH functions is implemented using recursive method of Spherical Harmonic Terms
			void Project_NOrder(uint32_t highestOrderIndex, uint32_t monteCarloSampleCount, N_SH_SPHERICAL_FUNC_INTERFACE* pTargetFunc, std::vector<float>& outSHCoefficient);
		};


		class  /*_declspec(dllexport)*/ SHIrradianceProjector
		{
		public:

			//environment irradiance map is sampled via a texture that is spherically mapped like a world map
			void ProjectIrradianceSphericalMap(NOISE_SH_ORDER shOrder, ITexture* pTex, std::vector<float>& outSHCoefficient);

			//environment irradiance map is sampled via 6 textures on a cube map //+X,-X,+Y,-Y,+Z,-Z in the same texture organized in a flattened box pattern
			void ProjectIrradianceCubeMap6Faces(NOISE_SH_ORDER shOrder, ITexture* pTexArray[6], std::vector<float>& outSHCoefficient);

			//environment irradiance map is sampled via cube map texture 
			void ProjectIrradianceCubeMap(NOISE_SH_ORDER shOrder, ITexture* pTex, std::vector<float>& outSHCoefficient);

			//environment irradiance map is sampled via predicate given by user
			void ProjectIrradianceLightPredicate(NOISE_SH_ORDER shOrder, ITexture* pTex, std::vector<float>& outSHCoefficient);

		private:

		};

	}
};