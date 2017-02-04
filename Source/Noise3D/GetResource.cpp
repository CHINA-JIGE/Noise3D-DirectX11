/***********************************************************************

                           cpp£ºGetResource (.rc)

						   desc:get binary resource (.rc) file,
						   retrieve ptr to binary memory block

************************************************************************/

#include "Noise3D.h"

#ifndef _DEBUG
using namespace Noise3D;

/*N_InternalResourceInfo Noise3D::GetInternalResource(NOISE_INTERNAL_RESOURCE resType)
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
	
	//get the executing file path of current exe/lib (whatever it is= =)
	wchar_t executingFilePath[MAX_PATH + 1];
	NOISE_MACRO_FUNCTION_WINAPI::GetModuleFileNameW(NULL, executingFilePath, sizeof(executingFilePath));

	//get handle from file path
	//HMODULE hMod = ::GetModuleHandleW(executingFilePath);
	HINSTANCE hInst = ::GetModuleHandleW(executingFilePath);

	//----1. Acquire resource Handle, 
	//the 2nd param was defined in resource header
	//and the "RCDATA" is one of resource type

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//BUT!!! the .res file should be linked in the EXE or DLL!!! or resource won't be found!!!
	HRSRC hRsrc
		= ::FindResourceW(hInst, MAKEINTRESOURCEW(IDR_SHADER1), L"SHADER");
		//FindResourceA(NULL, MAKEINTRESOURCEA(IDR_SHADER1), "\"shader\"");

	UINT err = GetLastError();

	//----2. use the handle to acquire the size of resource file
	outInfo.buffSize = ::SizeofResource(NULL, hRsrc);

	//----3. Load resource file
	HGLOBAL hGlobal = ::LoadResource(NULL, hRsrc);

	//----4. Lock the resource and get the pointer to the buffer
	outInfo.pBuff = ::LockResource(hGlobal);

	return outInfo;
};*/

N_InternalResourceInfo Noise3D::GetInternalResource(NOISE_INTERNAL_RESOURCE resType)
{
	N_InternalResourceInfo outInfo = { nullptr,0 };

	switch (resType)
	{
	case NOISE_INTERNAL_RESOURCE::NOISE_INTERNAL_RESOURCE_MAINSHADER:
		outInfo.buffSize = sizeof(shaderBinary);
		outInfo.pBuff = (void*)shaderBinary;
		break;

	default:
		//actually there is no way this could be executed
		return outInfo;
	}
	return outInfo;
}

#endif