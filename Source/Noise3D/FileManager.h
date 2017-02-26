/***********************************************************************

                           h£ºFile Manager

************************************************************************/

#pragma once

#include "FileIO_STL.h"
#include "FileIO_OBJ.h"
#include "FileIO_3DS.h"


namespace Noise3D
{
	struct N_MaterialDesc;

		class /*_declspec(dllexport)*/ IFileManager : 
			public IFileIO_3DS,
			public IFileIO_STL,
			public IFileIO_OBJ
		{
		public:
			IFileManager();

			BOOL ImportFile_PURE(NFilePath pFilePath, std::vector<char>& byteBuffer);

			BOOL ExportFile_PURE(NFilePath pFilePath, std::vector<char>* pFileBuffer, BOOL canOverlapOld);

		private:

		};
}