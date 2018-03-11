
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

IFileIO::IFileIO()
{

};

bool IFileIO::ImportFile_PURE(NFilePath pFilePath, std::vector<char>& byteBuffer)
{
	std::ifstream fileIn(pFilePath, std::ios::binary);

	if (!fileIn.is_open()) 
	{
		ERROR_MSG("FileManager : Cannot Open File !!");
		return false;
	}

	//file pointer move to eof
	fileIn.seekg(0, std::ios_base::end);

	//get file size
	int static_fileSize = (int)fileIn.tellg();

	//file pointer move to the beginning
	fileIn.seekg(0, std::ios_base::beg);

	//...........
	int i = 0;char tmpC =0;

	//allocate new memory block , initialized with 0
	byteBuffer.resize(static_fileSize,0);
	fileIn.read(&byteBuffer.at(0), static_fileSize);

	fileIn.close();

	return true;
}

bool IFileIO::ExportFile_PURE(NFilePath pFilePath, const std::vector<char>& pFileBuffer, bool canOverlapOld)
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
	if (!fileOut.is_open())
	{
		ERROR_MSG("FileManager : Cannot Open File !!");
		return false;
	}

	fileOut.write(&pFileBuffer.at(0), pFileBuffer.size());

	//close file
	fileOut.close();

	return true;
}

bool Noise3D::IFileIO::ExportFile_PURE(NFilePath pFilePath, const uint8_t * pData, size_t byteSize, bool canOverlapOld)
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
		ERROR_MSG("FileManager : Cannot Open File !!");
		return false;
	}

	//...........
	fileOut.write((char*)pData, byteSize);

	fileOut.close();

	return true;
}
