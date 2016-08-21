/***********************************************************************

                           h£ºFile Manager

************************************************************************/

#pragma once

namespace Noise3D
{
	struct N_MaterialDesc;

		class /*_declspec(dllexport)*/ IFileManager
		{
		public:
			IFileManager();

			BOOL ImportFile_PURE(NFilePath pFilePath, std::vector<char>& byteBuffer);

			BOOL ImportFile_STL(NFilePath pFilePath, std::vector<NVECTOR3>& refVertexBuffer, std::vector<UINT>& refIndexBuffer, std::vector<NVECTOR3>& refNormalBuffer, std::string& refFileInfo);

			BOOL ImportFile_3DS(
				NFilePath pFilePath,
				std::vector<NVECTOR3>& outVertexBuffer,
				std::vector<NVECTOR2>& outTexCoordList,
				std::vector<UINT>& outIndexBuffer,
				std::vector<N_MeshSubsetInfo>& outSubsetList,
				std::vector<N_MaterialDesc>& outMaterialList,
				std::vector<std::string>& outMatNameList,
				std::unordered_map<std::string, NFilePath>& out_TexName2FilePathPairList);

			BOOL ImportFile_OBJ(NFilePath pFilePath, std::vector<N_DefaultVertex>& outVertexBuffer, std::vector<UINT>& outIndexBuffer);

			BOOL ExportFile_PURE(NFilePath pFilePath, std::vector<char>* pFileBuffer, BOOL canOverlapOld);

		private:

		};
}