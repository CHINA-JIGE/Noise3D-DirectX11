/***********************************************************************

                           h£ºGetResource (.rc)

						   desc:get binary resource (.rc) file,
						   retrieve ptr to binary memory block

************************************************************************/
#pragma once

#include "NoiseBinaryResource.h"

namespace Noise3D
{
	//.rc file memory block info
	struct N_InternalResourceInfo
	{
		void* pBuff;
		DWORD buffSize;
	};

	enum NOISE_INTERNAL_RESOURCE
	{
		NOISE_INTERNAL_RESOURCE_MAINSHADER = 0
	};

	N_InternalResourceInfo GetInternalResource(NOISE_INTERNAL_RESOURCE resType);
};