#include <fstream>
#include <iostream>
#include <string>
#include <vector>

typedef unsigned int UINT;

int main(int argc, char* argv[])
{
	//split command into 3 attr
	std::string fileInName, fileOutName, ArrayName;
	fileInName = argv[1];
	fileOutName = argv[2];
	ArrayName = argv[3];

	//file In Name
	std::ifstream binaryFileIn(fileInName, std::ios::binary);

	//file open failed
	if (!binaryFileIn.good())
	{
		std::cout << "Failed to open binary file!!" << std::endl;
		std::cout << "cmd format : [fileInPath] [fileOutPath] [ArrayName]" << std::endl;
		system("pause");
		return 666;
	}

	//get file size
	binaryFileIn.seekg(0, std::ios::end);
	UINT fileSize = binaryFileIn.tellg();
	binaryFileIn.seekg(0);

	//read file to buffer
	std::vector<unsigned char> fileBuff(fileSize, 0);
	binaryFileIn.read((char*)&fileBuff.at(0), fileSize);
	binaryFileIn.close();

	std::ofstream fileOut(fileOutName, std::ios::app);
	fileOut << "const char "<<ArrayName <<"["<< fileSize << "] = {";

	for (UINT i = 0;i < fileSize;i++)
	{
		fileOut << UINT(fileBuff.at(i)) << ", ";
		if (i % 50 == 49)fileOut << std::endl;//next line
	}

	fileOut << "};";
	fileOut.close();

	std::cout << "Succeeded!!";
	system("pause");
};