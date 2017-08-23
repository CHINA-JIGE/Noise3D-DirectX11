/***********************************************************************

                           h��File Manager

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

			bool ImportFile_PURE(NFilePath pFilePath, std::vector<char>& byteBuffer);

			bool ExportFile_PURE(NFilePath pFilePath, std::vector<char>* pFileBuffer, bool canOverlapOld);

		private:

		};
}