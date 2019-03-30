/***********************************************************************

                           h£ºSTL file import/export

************************************************************************/

#pragma once

namespace Noise3D
{
	class IFileIO_STL
	{
	public:

		bool ImportFile_STL(NFilePath filePath, std::vector<Vec3>& outVertexBuffer, std::vector<UINT>& outIndexBuffer, std::vector<Vec3>& outNormalBuffer, std::string & outFileInfo);
	
		bool ExportFile_STL_Binary(NFilePath filePath, const std::string& headerInfo,const std::vector<Vec3>& inVertexBuffer,const std::vector<UINT>& inIndexBuffer);

		bool ExportFile_STL_Binary(NFilePath filePath, const std::string& headerInfo,const std::vector<Vec3>& inVertexBuffer);

	private:

		bool mFunction_ImportFile_STL_Binary(NFilePath pFilePath, std::vector<Vec3>& outVertexBuffer, std::vector<UINT>& outIndexBuffer, std::vector<Vec3>& outNormalBuffer, std::string& outFileInfo);

		bool mFunction_ImportFile_STL_Ascii(NFilePath pFilePath, std::vector<Vec3>& outVertexBuffer, std::vector<UINT>& outIndexBuffer, std::vector<Vec3>& outNormalBuffer, std::string& outFileInfo);

	};



};
