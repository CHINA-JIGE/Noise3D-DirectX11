
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

NoiseFileManager::NoiseFileManager()
{

};

BOOL NoiseFileManager::ImportFile_PURE(char * pFilePath, std::vector<char>* pFileBuffer)
{
	if (!pFileBuffer)
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

	//...........
	int i = 0;char tmpC =0;

	//allocate new memory block , initialized with 0
	pFileBuffer->resize(fileSize,0);
	while (!fileIn.eof())
	{
		
		fileIn.read(&pFileBuffer->at(0), fileSize);
		//逐字节读取
		//fileIn.get(tmpC);
		//pFileBuffer->push_back(tmpC);
	}


	//关闭文件
	fileIn.close();


	return TRUE;
}

BOOL NoiseFileManager::ImportFile_STL(char* pFilePath,std::vector<NVECTOR3>* pVertexBuffer,std::vector<UINT>* pIndexBuffer,std::vector<NVECTOR3>* pNormalBuffer, std::vector<char>* pFileInfo )
{
	
	//定义一个临时file buffer
	std::vector<char> tmpFileBuffer;

	//导入文件
	if (!ImportFile_PURE(pFilePath, &tmpFileBuffer))
	{
		DEBUG_MSG1( "Noise File Manager :  Load Pure File Failed !! ");
		return FALSE;
	}

		
	/*STL: Baidu encyclopedia
	二进制STL文件用固定的字节数来给出三角面片的几何信息。
		文件起始的80个字节是文件头，用于存贮文件名；
		紧接着用 4 个字节的整数来描述模型的三角面片个数，
		后面逐个给出每个三角面片的几何信息。每个三角面片占用固定的50个字节，依次是 :

		3个4字节浮点数(角面片的法矢量)
		3个4字节浮点数(1顶点的坐标)
		3个4字节浮点数(2顶点的坐标)
		3个4字节浮点数(3顶点的坐标)

		三角面片的最后2个字节用来描述三角面片的属性信息。
		一个完整二进制STL文件的大小为三角形面片数乘以 50再加上84个字节。*/


		//初始化迭代器
		std::vector<char>::iterator tmp_iterStart = tmpFileBuffer.begin();
		std::vector<char>::iterator tmp_iterEnd	 = tmpFileBuffer.begin();


		//加载80字节STL文件信息 ,拿迭代器作为vector区间表示器
		tmp_iterEnd += 80;


		//vector.assign()
		//将区间[first, last)的元素赋值到当前的vector容器中，或者赋n个值为x的元素到vector容器中，
		//这个容器会清除掉vector容器中以前的内容。
		if(pFileInfo)
		{
			pFileInfo->assign(tmp_iterStart, tmp_iterEnd);
		}


		//准备读4字节三角形个数
		int triangleCount = 0;

		//.................三角形个数
		triangleCount = mFunction_Combine4CharIntoInt(
			tmpFileBuffer.at(80), tmpFileBuffer.at(81), tmpFileBuffer.at(82), tmpFileBuffer.at(83));


		//开始迭代读取每个顶点的分量(float)
		int i = 0, j = 0;
		float tmpF = 0.0f;
		NVECTOR3 tmpVec3(0,0,0);
		char c1 = 0, c2 = 0, c3 = 0, c4 = 0;
		const int baseOffset = 84;
		//三角形的遍历
		for ( i = 0;i < triangleCount;i++)
		{
			//一个三角形内顶点的遍历
			for (j = 0;j < 4; j++)
			{
					//50 * i是因为每个三角形占了50个字节, j*12 是因为一个顶点占12字节
					//注意3dmax坐标到dx坐标的转换
					c1 = tmpFileBuffer[baseOffset + 50 * i + j * 12 +  0];
					c2 = tmpFileBuffer[baseOffset + 50 * i + j * 12 + 1];
					c3 = tmpFileBuffer[baseOffset + 50 * i + j * 12 + 2];
					c4 = tmpFileBuffer[baseOffset + 50 * i + j * 12 + 3];
					tmpVec3.x = mFunction_Combine4CharIntoFloat(c1, c2, c3, c4);

					c1 = tmpFileBuffer[baseOffset + 50 * i + j * 12 + 4];
					c2 = tmpFileBuffer[baseOffset + 50 * i + j * 12 + 5];
					c3 = tmpFileBuffer[baseOffset + 50 * i + j * 12 + 6];
					c4 = tmpFileBuffer[baseOffset + 50 * i + j * 12 + 7];
					tmpVec3.z = mFunction_Combine4CharIntoFloat(c1, c2, c3, c4);

					c1 = tmpFileBuffer[baseOffset + 50 * i + j * 12 + 8];
					c2 = tmpFileBuffer[baseOffset + 50 * i + j * 12 + 9];
					c3 = tmpFileBuffer[baseOffset + 50 * i + j * 12 + 10];
					c4 = tmpFileBuffer[baseOffset + 50 * i + j * 12 + 11];
					tmpVec3.y = mFunction_Combine4CharIntoFloat(c1, c2, c3, c4);

					//判断现在加载的是顶点还是法线
					switch ( j )
					{

					// j =0 加载的是法线（第一个12字节）
					case 0:
						if (pNormalBuffer)
						{
							pNormalBuffer->push_back(tmpVec3);
						}
						break;

					// j = 1，2，3加载的是顶点（第2，3，4个12字节）
					default:
						pVertexBuffer->push_back(tmpVec3);
						pIndexBuffer->push_back(3 * i + j - 1);
						break;
					}
			}//搞定一个三角形 next到下一个50字节blobk
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

float NoiseFileManager::mFunction_Combine4CharIntoFloat(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4)
{

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//这里有个巨坑的，据说计算机的数字储存，是从低位开始的
	//也就是说，小的位在左边，大的位在右边
	int tmpInt = static_cast<int>(c1) | static_cast<int>(c2) << 8 | static_cast<int>(c3) << 16 | static_cast<int>(c4) << 24;
	float tmpFloat =0.0f	;

	//int转float不要乱来....我都根本不知道Static_cast有没有做什么手脚，reinterpret不知道为什么用不了
	memcpy(&tmpFloat, &tmpInt, 4);
	return tmpFloat;
}

int NoiseFileManager::mFunction_Combine4CharIntoInt(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4)
{

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//这里有个巨坑的，据说计算机的数字储存，是从低位开始的
	//也就是说，小的位在左边，大的位在右边
	int tmpInt = static_cast<int>(c1) | static_cast<int>(c2) << 8 | static_cast<int>(c3) << 16 | static_cast<int>(c4) << 24;
	return tmpInt;
}

void NoiseFileManager::mFunction_SplitFloatInto4Char(float fValue, unsigned char & c1, unsigned char & c2, unsigned char & c3, unsigned char & c4)
{
	int tmpI = 0;
	memcpy(&tmpI, &fValue, 4);
	c1 =	(char)	((tmpI & 0x000000FF) );
	c2 = (char)	((tmpI & 0x0000FF00) >> 8);
	c3 = (char)	((tmpI & 0x00FF0000) >> 16);
	c4 = (char)	((tmpI & 0xFF000000)>>24);
}

void NoiseFileManager::mFunction_SplitIntInto4Char(int iValue, unsigned char & c1, unsigned char & c2, unsigned char & c3, unsigned char & c4)
{
	c1 = (char)((iValue & 0x000000FF));
	c2 = (char)((iValue & 0x0000FF00) >> 8);
	c3 = (char)((iValue & 0x00FF0000) >> 16);
	c4 = (char)((iValue & 0xFF000000) >> 24);
}


