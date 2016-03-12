/***********************************************************************

                           h£ºFile Manager

************************************************************************/

#pragma once

class _declspec(dllexport) NoiseFileManager
{
public:
		NoiseFileManager();

		BOOL ImportFile_PURE(NFilePath pFilePath, std::vector<char>& byteBuffer);

		BOOL ImportFile_STL(NFilePath pFilePath, std::vector<NVECTOR3>& refVertexBuffer, std::vector<UINT>& refIndexBuffer, std::vector<NVECTOR3>& refNormalBuffer, std::string& refFileInfo);

		BOOL ImportFile_3DS(
			NFilePath pFilePath,
			std::vector<NVECTOR3>& outVertexBuffer,
			std::vector<NVECTOR2>& outTexCoordList,
			std::vector<UINT>& outIndexBuffer,
			std::vector<N_MeshSubsetInfo>& outSubsetList,
			std::vector<N_Material>& outMaterialList,
			std::unordered_map<std::string,NFilePath>& out_TexName2FilePathPairList);

		BOOL ImportFile_OBJ(NFilePath pFilePath, std::vector<N_DefaultVertex>& outVertexBuffer, std::vector<UINT>& outIndexBuffer);

		BOOL ExportFile_PURE(NFilePath pFilePath, std::vector<char>* pFileBuffer,BOOL canOverlapOld);

private:

};
