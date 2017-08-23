
/***********************************************************************

					Description : STL file Operation

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

/*******************************************************************

									INTERFACE

*********************************************************************/
bool IFileIO_STL::ImportFile_STL(NFilePath pFilePath, std::vector<NVECTOR3>& refVertexBuffer, std::vector<UINT>& refIndexBuffer, std::vector<NVECTOR3>& refNormalBuffer, std::string & refFileInfo)
{
	std::ifstream tmpFile(pFilePath, std::ios::binary);

	if (!tmpFile.good())
	{
		ERROR_MSG("Load STL : file Open Failed!!");
		return false;
	}

	//move file cursor to the end
	tmpFile.seekg(0, std::ios::end);
	std::streamoff static_fileSize = tmpFile.tellg();
	if (static_fileSize < 84L)
	{
		ERROR_MSG("Load STL : file Damaged!!File Size is Too Small!!");
		return false;
	}
	tmpFile.seekg(0);

	char firstChar;
	tmpFile.read(&firstChar, 1);
	tmpFile.close();

	//ASCII STL starts with "solid"
	if (firstChar == 's')
	{
		return mFunction_ImportFile_STL_Ascii(pFilePath, refVertexBuffer, refIndexBuffer, refNormalBuffer, refFileInfo);
	}
	else
	{
		return mFunction_ImportFile_STL_Binary(pFilePath, refVertexBuffer, refIndexBuffer, refNormalBuffer, refFileInfo);
	}
}

bool IFileIO_STL::ExportFile_STL_Binary(NFilePath filePath, const std::string & headerInfo, const std::vector<NVECTOR3>& inVertexBuffer, const std::vector<UINT>& inIndexBuffer)
{
	std::ofstream fileOut(filePath, std::ios::binary);

	if (!fileOut.is_open())
	{
		ERROR_MSG("Export STL Binary : Open/Create File Failed! File path :" + filePath);
		return false;
	}

	/*STL: Baidu encyclopedia

	binary STL use fixed length of bit patterns to store vertex information,
	At the beginning, 80 bytes of header will be some custom info,
	Right behind the header , next 4 bytes will be the total number of triangles;

	the rest of the file will represent every triangles in 50 bytes blocks:

	3xfloat =12bytes ------- Facet Normal
	3xfloat =12bytes ------- Vertex1
	3xfloat =12bytes ------- Vertex2
	3xfloat =12bytes ------- Vertex3
	2 byte ------ face attribute info (I don't know what's the use...)

	the length of a complete STL will be 50 *(triangleCount) + 84  */

	//file header
	fileOut.write(headerInfo.c_str(), 80);

	//move reading cursor
	fileOut.seekp(80);

	//write bit patterns, and reinterpret the data to interested type
	//using decltype()

#define REINTERPRET_WRITE(var) \
	fileOut.write((char*)&var,sizeof(var));\

	UINT triangleCount = inIndexBuffer.size()/3;
	REINTERPRET_WRITE(triangleCount);

	for(UINT i=0;i<triangleCount;++i)
	{
		//3 vertices of a triangle
		NVECTOR3 v1 = inVertexBuffer.at(inIndexBuffer.at(3 * i + 0));
		NVECTOR3 v2 = inVertexBuffer.at(inIndexBuffer.at(3 * i + 1));
		NVECTOR3 v3 = inVertexBuffer.at(inIndexBuffer.at(3 * i + 2));
		NVECTOR3 edge1 = v3 - v1;
		NVECTOR3 edge2 = v2 - v1;
		NVECTOR3 triNorm(0, 0, 0);

		//normal vector of this triangle
		D3DXVec3Cross(&triNorm, &edge1, &edge2);


		//a facet normal
		REINTERPRET_WRITE(triNorm.x);
		REINTERPRET_WRITE(triNorm.z);
		REINTERPRET_WRITE(triNorm.y);

		//3 vertices
		REINTERPRET_WRITE(v1.x);
		REINTERPRET_WRITE(v1.z);
		REINTERPRET_WRITE(v1.y);

		REINTERPRET_WRITE(v2.x);
		REINTERPRET_WRITE(v2.z);
		REINTERPRET_WRITE(v2.y);

		REINTERPRET_WRITE(v3.x);
		REINTERPRET_WRITE(v3.z);
		REINTERPRET_WRITE(v3.y);


		uint16_t faceAttr = 0;
		REINTERPRET_WRITE(faceAttr);
	}


	fileOut.close();

	return true;
}

bool IFileIO_STL::ExportFile_STL_Binary(NFilePath filePath, const std::string & headerInfo, const std::vector<NVECTOR3>& inVertexBuffer)
{
	std::ofstream fileOut(filePath, std::ios::binary);

	if (!fileOut.is_open())
	{
		ERROR_MSG("Export STL Binary : Open/Create File Failed! File path :" + filePath);
		return false;
	}

	/*STL: Baidu encyclopedia

	binary STL use fixed length of bit patterns to store vertex information,
	At the beginning, 80 bytes of header will be some custom info,
	Right behind the header , next 4 bytes will be the total number of triangles;

	the rest of the file will represent every triangles in 50 bytes blocks:

	3xfloat =12bytes ------- Facet Normal
	3xfloat =12bytes ------- Vertex1
	3xfloat =12bytes ------- Vertex2
	3xfloat =12bytes ------- Vertex3
	2 byte ------ face attribute info (I don't know what's the use...)

	the length of a complete STL will be 50 *(triangleCount) + 84  */

	//file header
	fileOut.write(headerInfo.c_str(), 80);

	//move reading cursor
	fileOut.seekp(80);

	//write bit patterns, and reinterpret the data to interested type
	//using decltype()

/*(#define REINTERPRET_WRITE(var) \
	var=*(decltype(var)*)(void*)dataBlock;\
	fileOut.write(dataBlock,sizeof(var));\*/

#define REINTERPRET_WRITE(var) \
	fileOut.write((char*)&var,sizeof(var));\

	UINT triangleCount = inVertexBuffer.size() / 3;
	REINTERPRET_WRITE(triangleCount);

	for (UINT i = 0; i<triangleCount; ++i)
	{
		//3 vertices of a triangle
		NVECTOR3 v1 = inVertexBuffer.at(3 * i + 0);
		NVECTOR3 v2 = inVertexBuffer.at(3 * i + 1);
		NVECTOR3 v3 = inVertexBuffer.at(3 * i + 2);
		NVECTOR3 edge1 = v3 - v1;
		NVECTOR3 edge2 = v2 - v1;
		NVECTOR3 triNorm(0, 0, 0);

		//normal vector of this triangle
		D3DXVec3Cross(&triNorm, &edge1, &edge2);


		//a facet normal
		REINTERPRET_WRITE(triNorm.x);
		REINTERPRET_WRITE(triNorm.z);
		REINTERPRET_WRITE(triNorm.y);

		//3 vertices
		REINTERPRET_WRITE(v1.x);
		REINTERPRET_WRITE(v1.z);
		REINTERPRET_WRITE(v1.y);

		REINTERPRET_WRITE(v2.x);
		REINTERPRET_WRITE(v2.z);
		REINTERPRET_WRITE(v2.y);

		REINTERPRET_WRITE(v3.x);
		REINTERPRET_WRITE(v3.z);
		REINTERPRET_WRITE(v3.y);


		uint16_t faceAttr = 0;
		REINTERPRET_WRITE(faceAttr);
	}


	fileOut.close();

	return true;
}


/*******************************************************************

								LOCAL FUNCTION

*********************************************************************/
bool IFileIO_STL::mFunction_ImportFile_STL_Binary(NFilePath pFilePath, std::vector<NVECTOR3>& refVertexBuffer,
	std::vector<UINT>& refIndexBuffer, std::vector<NVECTOR3>& refNormalBuffer, std::string& refFileInfo)
{
	std::ifstream fileIn(pFilePath, std::ios::binary);

	if (!fileIn.good())
	{
		ERROR_MSG("Load STL Binary : Open File Failed!");
		return false;
	}

	/*STL: Baidu encyclopedia

	binary STL use fixed length of bit patterns to store vertex information,
	At the beginning, 80 bytes of header will be some custom info,
	Right behind the header , next 4 bytes will be the total number of triangles;

	the rest of the file will represent every triangles in 50 bytes blocks:

	3xfloat =12bytes ------- Facet Normal
	3xfloat =12bytes ------- Vertex1
	3xfloat =12bytes ------- Vertex2
	3xfloat =12bytes ------- Vertex3
	2 byte ------ face attribute info (I don't know what's the use...)

	the length of a complete STL will be 50 *(triangleCount) + 84  */


	//newly input string
	char headerInfo[81] = {};
	fileIn.read(headerInfo, 80);

	//the first char could not be "s" in BINARY mode , in order to distinguish form ASCII mode,
	//which starts with "solid"
	if (headerInfo[0] == 's')
	{
		ERROR_MSG("Load STL Binary : File Damaged!! It's not binary STL file!");
		return false;
	}

	refFileInfo = headerInfo;
	//move reading cursor
	fileIn.seekg(80);

	//read bit patterns, and reinterpret the data to interested type
	//using decltype()
#define REINTERPRET_READ(var) fileIn.read((char*)&var,sizeof(var));

	uint32_t triangleCount = 0;
	REINTERPRET_READ(triangleCount);

	if (triangleCount > 10000000)
	{
		ERROR_MSG("Load STL Binary : Triangle Count is larger than 10000000 or Data was damamged!!");
		fileIn.close();
		return false;
	}

	//then reserve spaces for vectors (optimization)
	refVertexBuffer.reserve(triangleCount * 3);
	refNormalBuffer.reserve(triangleCount);
	refIndexBuffer.reserve(triangleCount * 3);

	while (!fileIn.eof())
	{

		//a facet normal
		NVECTOR3 tmpVec3(0, 0, 0);
		REINTERPRET_READ(tmpVec3.x);
		REINTERPRET_READ(tmpVec3.z);
		REINTERPRET_READ(tmpVec3.y);
		refNormalBuffer.push_back(tmpVec3);

		//3 vertices
		for (UINT i = 0;i < 3;i++)
		{
			REINTERPRET_READ(tmpVec3.x);
			REINTERPRET_READ(tmpVec3.z);
			REINTERPRET_READ(tmpVec3.y);
			refVertexBuffer.push_back(tmpVec3);
		}

		uint16_t faceAttr = 0;
		REINTERPRET_READ(faceAttr);
	}

	//clockwise or counterClockwise (?)
	for (UINT i = 0;i < refVertexBuffer.size();i += 3)
	{
		std::swap(refVertexBuffer[i + 1], refVertexBuffer[i + 2]);
	}

	//without optimization, vertices can be overlapped
	refIndexBuffer.resize(refVertexBuffer.size());
	for (UINT i = 0;i < refIndexBuffer.size();i++)
	{
		refIndexBuffer.at(i) = i;
	}


	fileIn.close();

	return true;
}

bool IFileIO_STL::mFunction_ImportFile_STL_Ascii(NFilePath pFilePath, std::vector<NVECTOR3>& refVertexBuffer, std::vector<UINT>& refIndexBuffer, std::vector<NVECTOR3>& refNormalBuffer, std::string& refFileInfo)
{
	std::ifstream fileIn(pFilePath);

	if (!fileIn.good())
	{
		ERROR_MSG("Load STL Ascii : Open File Failed!!");
		return false;
	}

	//newly input string
	std::string currString;

	//object Name
	std::string objectName;

	//the first line (object name) need to be dealt with specially
	fileIn >> currString;

	//the beginning of the 
	if (currString == "solid")
	{
		//the name of the object could be null, thus input the first line instead of
		//input next string will be safer
		char pFirstLineString[80] = {};
		fileIn.getline(pFirstLineString, 80);
		objectName = pFirstLineString;

		//delete the space at the front
		if (objectName.size()> 0)objectName.erase(objectName.begin());
	}
	else
	{
		ERROR_MSG("Load STL Ascii : file damaged!!");
		return false;
	}

	refFileInfo = objectName;


	//loop reading
	while (!fileIn.eof())
	{
		fileIn >> currString;

		if (currString == "endsolid")break;

		//"facet normal" + x+y+z
		if (currString == "normal")
		{
			NVECTOR3 tmpFaceNormal(0, 0, 0);
			fileIn >> tmpFaceNormal.x >> tmpFaceNormal.z >> tmpFaceNormal.y;
			//face normal (may be used as vertex normal)
			refNormalBuffer.push_back(tmpFaceNormal);
		}

		//"vertex" +x +y+z
		if (currString == "vertex")
		{
			NVECTOR3 tmpPoint(0, 0, 0);
			fileIn >> tmpPoint.x >> tmpPoint.z >> tmpPoint.y;
			refVertexBuffer.push_back(tmpPoint);
		}
	}

	//clockwise or counterClockwise
	for (UINT i = 0;i < refVertexBuffer.size();i += 3)
	{
		std::swap(refVertexBuffer[i + 1], refVertexBuffer[i + 2]);
	}


	fileIn.close();

	//without optimization, vertices can be overlapped
	refIndexBuffer.resize(refVertexBuffer.size());
	for (UINT i = 0;i < refIndexBuffer.size();i++)
	{
		refIndexBuffer.at(i) = i;
	}

	return true;
}

