
/***********************************************************************

                           类：NOISE File Manager

			简述：各种文件操作，import export 甚至decode encode

************************************************************************/

#include "Noise3D.h"

/*
ios::in 		读
ios::out		写
ios::app		从文件末尾开始写
ios::binary       二进制模式
ios::nocreate	打开一个文件时，如果文件不存在，不创建文件。
ios::noreplace	打开一个文件时，如果文件不存在，创建该文件
ios::trunc		打开一个文件，然后清空内容
ios::ate		打开一个文件时，将位置移动到文件尾
*/

//in OBJ file ,vertex info is composed of indices
struct N_LoadOBJ_vertexInfoIndex
{
	N_LoadOBJ_vertexInfoIndex()
	{
		vertexID = texcoordID = vertexNormalID = 0;
	};

	N_LoadOBJ_vertexInfoIndex(int vID, int texcID, int vnID)
	{
		vertexID = vID;
		texcoordID = texcID;
		vertexNormalID = vnID;
	}

	inline BOOL operator==(N_LoadOBJ_vertexInfoIndex const& v)const
	{
		if (vertexID == v.vertexID && texcoordID == v.texcoordID && vertexNormalID == v.vertexNormalID)
		{
			return TRUE;
		}
		return FALSE;
	}

	UINT vertexID;
	UINT texcoordID;
	UINT vertexNormalID;
};

NoiseFileManager::NoiseFileManager()
{

};

BOOL NoiseFileManager::ImportFile_PURE(char * pFilePath, std::vector<char>& byteBuffer)
{
	//文件输入流
	std::ifstream fileIn(pFilePath, std::ios::binary);

	//文件不存在就return
	if (!fileIn.is_open()) 
	{
		DEBUG_MSG1("NoiseFileManager : Cannot Open File !!");
		return FALSE;
	}

	//指针移到文件尾
	fileIn.seekg(0, std::ios_base::end);

	//指针指着文件尾，当前位置就是大小
	int fileSize = (int)fileIn.tellg();


	//指针移到文件头
	fileIn.seekg(0, std::ios_base::beg);

	//...........
	int i = 0;char tmpC =0;

	//allocate new memory block , initialized with 0
	byteBuffer.resize(fileSize,0);
	while (!fileIn.eof())
	{
		
		fileIn.read(&byteBuffer.at(0), fileSize);
		//逐字节读取
		//fileIn.get(tmpC);
		//pFileBuffer->push_back(tmpC);
	}


	//关闭文件
	fileIn.close();


	return TRUE;
}

BOOL NoiseFileManager::ImportFile_STL(char * pFilePath, std::vector<NVECTOR3>& refVertexBuffer, std::vector<UINT>& refIndexBuffer, std::vector<NVECTOR3>& refNormalBuffer, std::string & refFileInfo)
{
	std::ifstream tmpFile(pFilePath, std::ios::binary);

	if (!tmpFile.good())
	{
		DEBUG_MSG1("Load STL : file Open Failed!!");
		return FALSE;
	}
	
	//move file cursor to the end
	tmpFile.seekg(0,std::ios::end);
	std::streamoff fileSize = tmpFile.tellg();
	if (fileSize < 84L)
	{
		DEBUG_MSG1("Load STL : file Damaged!!File Size is Too Small!!");
		return FALSE;
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

BOOL NoiseFileManager::ImportFile_OBJ(char * pFilePath, std::vector<N_DefaultVertex>& refVertexBuffer, std::vector<UINT>& refIndexBuffer)
{
	std::fstream fileIn(pFilePath);
	if (!fileIn.good())
	{
		DEBUG_MSG1("Import OBJ : Open File failed!!");
		return FALSE;
	}

	std::vector<NVECTOR3> pointList;//xyz buffer
	std::vector<NVECTOR2> texcoordList;//texcoord buffer
	std::vector<NVECTOR3> VNormalList;//vertex normal buffer
	std::vector<N_LoadOBJ_vertexInfoIndex> vertexInfoList;//indices combination

	//newly input string from file
	std::string currString;

	//,...............
	while (!fileIn.eof())
	{
		fileIn >> currString;

		//3d vertex : "v 1.0000000 0.52524242 5.12312345"
		if (currString == "v")
		{
			NVECTOR3 currPoint(0, 0, 0);
			fileIn >> currPoint.x >> currPoint.y >> currPoint.z;
			pointList.push_back(currPoint);
		}

		//vertex normal "vn 1.0000000 0.52524242 5.12312345"
		if (currString == "vn")
		{
			NVECTOR3 currNormal(0, 0, 0);
			fileIn >> currNormal.x >> currNormal.y >> currNormal.z;
			VNormalList.push_back(currNormal);
		}

		//texture coordinate "vt 1.0000000 0.0000000"
		if (currString == "vt")
		{
			NVECTOR2 currTexCoord(0, 0);
			fileIn >> currTexCoord.x >> currTexCoord.y;
			texcoordList.push_back(currTexCoord);
		}

		//face : if this face is trangles(OBJ also support quads or curves etc..)
		//, the combination will be 
		//  vertex index  / texcoord index  /  vnormal index (for each vertex)
		// like "1/3/4" "5/6/7"
		if (currString == "f")
		{
			for (UINT i = 0;i < 3;++i)
			{
				//the .size() will be slow????
				//static UINT currUniqueVertexCount = 0;
				N_LoadOBJ_vertexInfoIndex currVertex = { 0,0,0 };
				std::stringstream tmpSStream;
				std::string tmpString;

				//then a "1/2/3" "4/1/3" like string will be input
				fileIn >> tmpString;

				//replace "/" with SPACE ,so STD::STRINGSTREAM will automatically format
				//the string into 3 separate part
				for (auto& c : tmpString) { if (c == '/')c = ' '; }

				tmpSStream << tmpString;
				tmpSStream >> currVertex.vertexID >> currVertex.texcoordID >> currVertex.vertexNormalID;
				--currVertex.texcoordID;
				--currVertex.vertexID;
				--currVertex.vertexNormalID;

				//this will be an n^2 searching....optimization will be needed
				//non-existed element will be created
				BOOL IsVertexExist = FALSE;
				UINT  existedVertexIndex = 0;
				for (UINT i = 0;i <vertexInfoList.size();i++)
				{
					//in DEBUG mode ,[] operator will be a big performance overhead
					if (vertexInfoList[i] == currVertex)
					{
						IsVertexExist = TRUE;
						existedVertexIndex = i;
						break;
					}
				}

				if (!IsVertexExist)
				{
					vertexInfoList.push_back(currVertex);
					//the newest vertex.....
					refIndexBuffer.push_back(vertexInfoList.size() - 1);
				}
				else
				{
					refIndexBuffer.push_back(existedVertexIndex);
				}
			}
		}
	}

	fileIn.close();

	// All interested data are acquired, now convert to VB/IB
	refVertexBuffer.resize(vertexInfoList.size());
	for (UINT i = 0;i < refVertexBuffer.size();++i)
	{
		N_DefaultVertex tmpVertex = {};

		//several indices which can retrieve vertex information
		N_LoadOBJ_vertexInfoIndex& indicesCombination = vertexInfoList.at(i);
		tmpVertex.Pos = pointList.at(indicesCombination.vertexID);
		tmpVertex.Normal = VNormalList.at(indicesCombination.vertexNormalID);
		tmpVertex.TexCoord = texcoordList.at(indicesCombination.texcoordID);
		tmpVertex.Color = NVECTOR4(1.0f,1.0f,1.0f, 1.0f);
		//tangent
		NVECTOR3 tmpVec(-tmpVertex.Normal.z, 0, tmpVertex.Normal.x);
		D3DXVec3Cross(&tmpVertex.Tangent, &tmpVertex.Normal, &tmpVec);
		D3DXVec3Normalize(&tmpVertex.Tangent, &tmpVertex.Tangent);

		//.......
		refVertexBuffer.at(i)=(tmpVertex);
	}

	return TRUE;
}

BOOL NoiseFileManager::ImportFile_NOISELAYER(char * pFilePath, std::vector<N_LineStrip>* pLineStripBuffer)
{
	if (!pLineStripBuffer)
	{
		return FALSE;
	}

	//文件输入流
	std::ifstream fileIn(pFilePath, std::ios::binary);

	//文件不存在就return

	if (!fileIn.is_open())
	{
		DEBUG_MSG1("NoiseFileManager : Cannot Open File !!");
		return FALSE;
	}

	//指针移到文件尾
	fileIn.seekg(0, std::ios_base::end);

	//指针指着文件尾，当前位置就是大小
	int fileSize = (int)fileIn.tellg();


	//指针移到文件头
	fileIn.seekg(0, std::ios_base::beg);


	//some  check before importing file
	if (!pLineStripBuffer)
	{
		return FALSE;
	}


	//first import the count data of line strip
	UINT magicNum = 0;
	UINT versionID = 0;
	UINT lineStripCount=0;
	UINT currLineStripPointCount = 0;
	UINT currLIneStripNormalCount = 0;
	UINT layerID = 0;
	NVECTOR3 tmpV;
	N_LineStrip  emptyLineStrip;
	UINT i = 0, j = 0;
	

#define STREAM_READ(STREAM,OBJECT) STREAM.read((char*)&(OBJECT),sizeof(OBJECT));

	//file head
	STREAM_READ(fileIn, magicNum);

	STREAM_READ(fileIn, versionID);
	//.........how many line strips
	STREAM_READ(fileIn, lineStripCount);


	//start to read line strip
	for (i = 0;i < lineStripCount;i++)
	{
		//we can push an empty line strip at the back , but didn't specify a layerID
		//because we no longer need (it's used for optimization)
		pLineStripBuffer->push_back(emptyLineStrip);

		STREAM_READ(fileIn, layerID);
		pLineStripBuffer->at(i).LayerID = layerID;

		STREAM_READ(fileIn, currLineStripPointCount);

		STREAM_READ(fileIn, currLIneStripNormalCount);

	
		//input Points of a line strip
		for (j = 0;j < currLineStripPointCount;j++)
		{
			STREAM_READ(fileIn, tmpV);
			pLineStripBuffer->at(i).pointList.push_back(tmpV);
		}

		//input normals of line segments
		for (j = 0;j < currLIneStripNormalCount;j++)
		{
			STREAM_READ(fileIn, tmpV)
			pLineStripBuffer->at(i).normalList.push_back(tmpV);
		}

	}

	fileIn.close();

	return TRUE;
}

BOOL NoiseFileManager::ExportFile_PURE(char * pFilePath, std::vector<char>* pFileBuffer, BOOL canOverlapOld)
{

	std::ofstream fileOut;

	//can we overlap the old file??
	if(canOverlapOld)
	{
		fileOut.open(pFilePath, std::ios::binary | std::ios::trunc);
	}
	else
	{
		fileOut.open(pFilePath, std::ios::binary | std::ios::app);
	}

	//check if we have successfully opened the file
	if (!fileOut.good())
	{
		DEBUG_MSG1("NoiseFileManager : Cannot Open File !!");
		return FALSE;
	}

	//...........
	UINT i = 0;char tmpC = 0;
	for (i = 0;i < pFileBuffer->size();i++)
	{
		fileOut.put(pFileBuffer->at(i));
	}

	//关闭文件
	fileOut.close();

	return TRUE;
}

BOOL NoiseFileManager::ExportFile_NOISELAYER(char * pFilePath, std::vector<N_LineStrip>* pLineStripBuffer, BOOL canOverlapOld)
{
	std::ofstream fileOut;

	//can we overlap the old file??
	if (canOverlapOld)
	{
		fileOut.open(pFilePath, std::ios::binary | std::ios::trunc);
	}
	else
	{
		fileOut.open(pFilePath, std::ios::binary | std::ios::app);
	}

	//check if we have successfully opened the file
	if (!fileOut.good())
	{
		DEBUG_MSG1("NoiseFileManager : Cannot Open File !!");
		return FALSE;
	}

	//prepare to output,tmp var to store number
	UINT i = 0, j = 0;

	/*
	FORMAT: 
	4 byte magicNum
	4 byte versionID
	4 byte to store Line Strip Count
	and for every Line Strip :
		first		4 byte for pointList.size()
		then		4 byte for normalList.size(), but it's actually  normalList.size()-1 ,and this is a reminder that
					there are normal data to be read 
		then		4 (float) * 3 (vec3 component) *( n + n-1) byte for a whole line strip(vertex + normal)

		keep writing data until all line strip are traversed
	*/

	//convert variables into char* to directly write in a file
#define STREAM_WRITE(STREAM,OBJECT)  {(STREAM).write((char *)&(OBJECT),sizeof(OBJECT));}

	//	first 4 byte for magic number
	char magicNum[] = { 'k','A','s','T' };
	STREAM_WRITE(fileOut, magicNum);

	//	4 byte for version
	UINT32 version = 0xffffff01;
	STREAM_WRITE(fileOut, version);

	//	 4 byte for line strip count
	UINT32 lineStripCount = pLineStripBuffer->size();
	STREAM_WRITE(fileOut, lineStripCount);



	//for every line strip
	for (i = 0;i < pLineStripBuffer->size();i++)
	{
		UINT layerID = pLineStripBuffer->at(i).LayerID;
		STREAM_WRITE(fileOut, layerID);

		//first output points count of current line strip 
		UINT pointListSize = pLineStripBuffer->at(i).pointList.size();
		STREAM_WRITE(fileOut, pointListSize);

		//then normals
		UINT normalListSize = pLineStripBuffer->at(i).normalList.size();
		STREAM_WRITE(fileOut, normalListSize);

		//and traverse every vertices
		for (j = 0;j < pLineStripBuffer->at(i).pointList.size(); j++)
		{
			NVECTOR3 tmpVertex = pLineStripBuffer->at(i).pointList.at(j);
			STREAM_WRITE(fileOut, tmpVertex);
		}

		//and traverse every normal
		for (j = 0;j < pLineStripBuffer->at(i).normalList.size(); j++)
		{
			NVECTOR3 tmpNormal = pLineStripBuffer->at(i).normalList.at(j);
			STREAM_WRITE(fileOut, tmpNormal);
		}
	}

	fileOut.close();

	return TRUE;
}



/***********************************************************************
											PRIVATE
***********************************************************************/

BOOL NoiseFileManager::mFunction_ImportFile_STL_Binary(char* pFilePath, std::vector<NVECTOR3>& refVertexBuffer,
	std::vector<UINT>& refIndexBuffer, std::vector<NVECTOR3>& refNormalBuffer, std::string& refFileInfo)
{
	std::ifstream fileIn(pFilePath, std::ios::binary);

	if (!fileIn.good())
	{
		DEBUG_MSG1("Load STL Binary : Open File Failed!");
		return FALSE;
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
		DEBUG_MSG1("Load STL Binary : File Damaged!! It's not binary STL file!");
		return FALSE;
	}

	refFileInfo = headerInfo;
	//move reading cursor
	fileIn.seekg(80);

	//read bit patterns, and reinterpret the data to interested type
	//using decltype()
#define REINTERPRET_READ(var) \
	fileIn.read(dataBlock,sizeof(var));\
	var=*(decltype(var)*)(void*)dataBlock;\

	//a char array used to store bit pattern (used in REINTERPRET_READ)
	char dataBlock[5] = {};

	uint32_t triangleCount = 0;
	REINTERPRET_READ(triangleCount);

	if (triangleCount > 500000)
	{
		DEBUG_MSG1("Load STL Binary : Triangle Count is larger than 500000 / Data Damamged!!");
		fileIn.close();
		return FALSE;
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
		REINTERPRET_READ(tmpVec3.y);
		REINTERPRET_READ(tmpVec3.z);
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

	//clockwise or counterClockwise
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

	return TRUE;
}

BOOL NoiseFileManager::mFunction_ImportFile_STL_Ascii(char * pFilePath, std::vector<NVECTOR3>& refVertexBuffer, std::vector<UINT>& refIndexBuffer, std::vector<NVECTOR3>& refNormalBuffer, std::string& refFileInfo)
{
	std::ifstream fileIn(pFilePath);

	if (!fileIn.good())
	{
		DEBUG_MSG1("Load STL Ascii : Open File Failed!!");
		return FALSE;
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
		DEBUG_MSG1("Load STL Ascii : file damaged!!");
		return FALSE;
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
			fileIn >> tmpFaceNormal.x >> tmpFaceNormal.y >> tmpFaceNormal.z;
			//face normal (may be used as vertex normal)
			refNormalBuffer.push_back(tmpFaceNormal);
		}

		//"vertex" +x +y+z
		if(currString=="vertex")
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

	return TRUE;
}

