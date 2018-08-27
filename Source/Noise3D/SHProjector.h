
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
		enum N_SH_ORDER
		{
			ORDER_0,//1 coefficient
			ORDER_1,//4 coefficient
			ORDER_2,//9 coefficient
			ORDER_3,//16 coefficient
		};

		//in which form the lighting representation is
		enum N_SH_LIGHTING_REPRESENTATION
		{
			TEXTURE_SPHERICAL_MAPPING,//one texture mapping to a sphere(
			TEXTURE_CUBE_MAPPING_6_INDIVIDUAL_FACES,//given 6 individual pictures
			TEXTURE_CUBE_MAPPING_COMBINED_FACES//+X,-X,+Y,-Y,+Z,-Z in the same texture in a flattened box pattern
		};

		class  /*_declspec(dllexport)*/ SHProjector
		{
		public:

		private:

		};
	}
};