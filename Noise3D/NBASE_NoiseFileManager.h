/***********************************************************************

                           h£ºFile Manager

************************************************************************/

#pragma once

public class _declspec(dllexport) NoiseFileManager
{
public:
		NoiseFileManager();
		
		void	SelfDestruction();

		BOOL ImportFile_PURE(char* pFilePath, std::vector<char>* pFileBuffer);

		BOOL ImportFile_STL(char* pFilePath,std::vector<NVECTOR3>*	pVertexBuffer,std::vector<UINT>* pIndexBuffer, std::vector<NVECTOR3>* pNormalBuffer,std::vector<char>*	pFileInfo);

		BOOL ImportFile_NOISELAYER(char* pFilePath, std::vector<N_LineStrip>* pLineStripBuffer);

		BOOL ExportFile_PURE(char* pFilePath, std::vector<char>* pFileBuffer,BOOL canOverlapOld);

		BOOL ExportFile_NOISELAYER(char* pFilePath, std::vector<N_LineStrip>* pLineStripBuffer, BOOL canOverlapOld);

private:

	float		mFunction_Combine4CharIntoFloat(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4);

	int		mFunction_Combine4CharIntoInt(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4);

	void		mFunction_SplitFloatInto4Char(float fValue,unsigned char& c1, unsigned char& c2, unsigned char& c3, unsigned char& c4);
	
	void		mFunction_SplitIntInto4Char(int iValue,unsigned char& c1, unsigned char& c2, unsigned char& c3, unsigned char& c4);
};
