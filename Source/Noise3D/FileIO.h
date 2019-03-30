/***********************************************************************

                           h£ºFile Manager

************************************************************************/

#pragma once

#include "FileIO_STL.h"
#include "FileIO_OBJ.h"
//#include "FileIO_3DS.h"


namespace Noise3D
{
	struct N_MaterialDesc;

		class /*_declspec(dllexport)*/ IFileIO : 
			public IFileIO_STL,
			public IFileIO_OBJ
			//public IFileIO_3DS,
		{
		public:
			IFileIO();

			bool ImportFile_PURE(NFilePath pFilePath, std::vector<char>& byteBuffer);

			bool ExportFile_PURE(NFilePath pFilePath, const std::vector<char>& pFileBuffer, bool canOverlapOld);

			bool ExportFile_PURE(NFilePath pFilePath,const uint8_t* pData,size_t byteSize, bool canOverlapOld);

		private:

		};
}