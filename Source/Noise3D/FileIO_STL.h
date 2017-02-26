/***********************************************************************

                           h£ºSTL file import/export

************************************************************************/

#pragma once

namespace Noise3D
{
	class IFileIO_STL
	{
	public:

		BOOL ImportFile_STL(NFilePath filePath, std::vector<NVECTOR3>& outVertexBuffer, std::vector<UINT>& outIndexBuffer, std::vector<NVECTOR3>& outNormalBuffer, std::string & outFileInfo);
	
		BOOL ExportFile_STL_Binary(NFilePath filePath, const std::string& headerInfo,const std::vector<NVECTOR3>& inVertexBuffer,const std::vector<UINT>& inIndexBuffer);

		BOOL ExportFile_STL_Binary(NFilePath filePath, const std::string& headerInfo,const std::vector<NVECTOR3>& inVertexBuffer);

	private:

		BOOL mFunction_ImportFile_STL_Binary(NFilePath pFilePath, std::vector<NVECTOR3>& outVertexBuffer, std::vector<UINT>& outIndexBuffer, std::vector<NVECTOR3>& outNormalBuffer, std::string& outFileInfo);

		BOOL mFunction_ImportFile_STL_Ascii(NFilePath pFilePath, std::vector<NVECTOR3>& outVertexBuffer, std::vector<UINT>& outIndexBuffer, std::vector<NVECTOR3>& outNormalBuffer, std::string& outFileInfo);

	};



};
