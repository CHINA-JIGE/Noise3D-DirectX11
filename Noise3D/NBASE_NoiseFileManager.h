/***********************************************************************

                           h£ºFile Manager

************************************************************************/

#pragma once

class _declspec(dllexport) NoiseFileManager
{
public:
		NoiseFileManager();

		BOOL ImportFile_PURE(char* pFilePath, std::vector<char>& byteBuffer);

		BOOL ImportFile_STL(char* pFilePath, std::vector<NVECTOR3>& refVertexBuffer, std::vector<UINT>& refIndexBuffer, std::vector<NVECTOR3>& refNormalBuffer, std::string& refFileInfo);

		BOOL ImportFile_3DS(char* pFilePath, std::vector<NVECTOR3>& refVertexBuffer, std::vector<UINT>& refIndexBuffer);

		BOOL ImportFile_OBJ(char* pFilePath, std::vector<N_DefaultVertex>& refVertexBuffer, std::vector<UINT>& refIndexBuffer);

		BOOL ImportFile_NOISELAYER(char* pFilePath, std::vector<N_LineStrip>* pLineStripBuffer);

		BOOL ExportFile_PURE(char* pFilePath, std::vector<char>* pFileBuffer,BOOL canOverlapOld);

		BOOL ExportFile_NOISELAYER(char* pFilePath, std::vector<N_LineStrip>* pLineStripBuffer, BOOL canOverlapOld);

private:

};
