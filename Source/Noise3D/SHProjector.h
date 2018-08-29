
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

		//an interface that user can implement to pass in to 'ProjectIrradianceLightPredicate'
		struct N_SH_IRRADIANCE_SAMPLE_INTERFACE
		{
			NColor4f Sample(const NVECTOR3& dir);
		};

		class  /*_declspec(dllexport)*/ SHProjector
		{
		public:

			//environment irradiance map is sampled via a texture that is spherically mapped like a world map
			void ProjectIrradianceSphericalMap(N_SH_ORDER shOrder, ITexture* pTex, std::vector<float>& outSHCoefficient);

			//environment irradiance map is sampled via 6 textures on a cube map //+X,-X,+Y,-Y,+Z,-Z in the same texture organized in a flattened box pattern
			void ProjectIrradianceCubeMap6Faces(N_SH_ORDER shOrder, ITexture* pTexArray[6], std::vector<float>& outSHCoefficient);

			//environment irradiance map is sampled via cube map texture 
			void ProjectIrradianceCubeMap(N_SH_ORDER shOrder, ITexture* pTex, std::vector<float>& outSHCoefficient);

			//environment irradiance map is sampled via predicate given by user
			void ProjectIrradianceLightPredicate(N_SH_ORDER shOrder, ITexture* pTex, std::vector<float>& outSHCoefficient);

			//not implemented
			void ProjectTransferFunction();

		private:

		};
	}
};