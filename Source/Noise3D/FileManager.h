/***********************************************************************

                           h£ºFile Manager

************************************************************************/

#pragma once

#include "FileMgr_3DS.h"

namespace Noise3D
{
	struct N_MaterialDesc;

		class /*_declspec(dllexport)*/ IFileManager : public IFileLoader_3ds
		{
		public:
			IFileManager();

			BOOL ImportFile_PURE(NFilePath pFilePath, std::vector<char>& byteBuffer);

			BOOL ImportFile_STL(NFilePath pFilePath, std::vector<NVECTOR3>& refVertexBuffer, std::vector<UINT>& refIndexBuffer, std::vector<NVECTOR3>& refNormalBuffer, std::string& refFileInfo);

			BOOL ImportFile_OBJ(NFilePath pFilePath, std::vector<N_DefaultVertex>& outVertexBuffer, std::vector<UINT>& outIndexBuffer);

			BOOL ExportFile_PURE(NFilePath pFilePath, std::vector<char>* pFileBuffer, BOOL canOverlapOld);

		private:

		};
}