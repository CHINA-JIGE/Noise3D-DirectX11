/***********************************************************************

                           cpp£ºGetResource (.rc)

						   desc:get binary resource (.rc) file,
						   retrieve ptr to binary memory block

************************************************************************/

#include "Noise3D.h"
#include "resource1.h"

using namespace Noise3D;

N_InternalResourceInfo Noise3D::GetInternalResource(NOISE_INTERNAL_RESOURCE resType)
{
	//pBuff,size
	N_InternalResourceInfo outInfo = { nullptr,0 };

	WORD resourceIDR_id;

	switch (resType)
	{
	case NOISE_INTERNAL_RESOURCE::NOISE_INTERNAL_RESOURCE_MAINSHADER:
		resourceIDR_id = IDR_SHADER1;
		break;

	default:
		//actually there is no way this could be executed
		return outInfo;
	}

	//to retrieve .rc file from the compile program, there are several fixed steps
	//----1. Acquire resource Handle
	//----2. Acquire the size of resource file
	//----3. Load resource file
	//----4. Lock the resource and get the pointer to the buffer

	//----1. Acquire resource Handle, 
	//the 2nd param was defined in resource header
	//and the "RCDATA" is one of resource type
	HRSRC hRsrc
		= FindResource(NULL, MAKEINTRESOURCEW(IDR_SHADER1), L"\"shader\"");
		//FindResourceA(NULL, MAKEINTRESOURCEA(IDR_SHADER1), "\"shader\"");

	//----2. use the handle to acquire the size of resource file
	outInfo.buffSize = SizeofResource(NULL, hRsrc);

	//----3. Load resource file
	HGLOBAL hGlobal = LoadResource(NULL, hRsrc);

	//----4. Lock the resource and get the pointer to the buffer
	outInfo.pBuff = LockResource(hGlobal);

	return outInfo;
};
