
/***********************************************************************

                           class : File IO

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

/*
ios::in 		read
ios::out		write
ios::app		append, start from the end of file
ios::binary       binary R/W mode
ios::nocreate	open a file, even if file doesnt exist, the program won't create one
ios::noreplace	
ios::trunc		open a file, clear the content
ios::ate		open a file, move the cursor to the end
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
