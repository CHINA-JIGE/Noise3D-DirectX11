/***********************************************************************

                           h£ºFile Manager

************************************************************************/

#pragma once

public class _declspec(dllexport) NoiseFileManager
{
public:
		NoiseFileManager();
		
		BOOL ImportFile_PURE(char* pFilePath, std::vector<char>* pFileBuffer);

		BOOL ImportFile_STL(char* pFilePath,std::vector<NVECTOR3>*	pVertexBuffer,std::vector<UINT>* pIndexBuffer, std::vector<NVECTOR3>* pNormalBuffer,std::vector<char>*			pFileInfo);

private:

	float		mFunction_Combine4CharIntoFloat(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4);

	int		mFunction_Combine4CharIntoInt(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4);
};
