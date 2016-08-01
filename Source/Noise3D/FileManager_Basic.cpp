
/***********************************************************************

                           类：NOISE File Manager

			简述：各种文件操作，不同文件还是分在不同的cpp实现吧，
			相对独立一点

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

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

IFileManager::IFileManager()
{

};

BOOL IFileManager::ImportFile_PURE(NFilePath pFilePath, std::vector<char>& byteBuffer)
{
	//文件输入流
	std::ifstream fileIn(pFilePath, std::ios::binary);

	//文件不存在就return
	if (!fileIn.is_open()) 
	{
		ERROR_MSG("FileManager : Cannot Open File !!");
		return FALSE;
	}

	//指针移到文件尾
	fileIn.seekg(0, std::ios_base::end);

	//指针指着文件尾，当前位置就是大小
	int static_fileSize = (int)fileIn.tellg();


	//指针移到文件头
	fileIn.seekg(0, std::ios_base::beg);

	//...........
	int i = 0;char tmpC =0;

	//allocate new memory block , initialized with 0
	byteBuffer.resize(static_fileSize,0);
	while (!fileIn.eof())
	{
		
		fileIn.read(&byteBuffer.at(0), static_fileSize);
		//逐字节读取
		//fileIn.get(tmpC);
		//pFileBuffer->push_back(tmpC);
	}


	//关闭文件
	fileIn.close();


	return TRUE;
}

BOOL IFileManager::ExportFile_PURE(NFilePath pFilePath, std::vector<char>* pFileBuffer, BOOL canOverlapOld)
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
		ERROR_MSG("FileManager : Cannot Open File !!");
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
