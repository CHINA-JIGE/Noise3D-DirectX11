#include "Noise3D.h"
#include <sstream>
#include <iostream>

struct ConvertInfo
{
	std::string filePath;
	float heightFieldScale;
	float r;
	float g;
	float b;
	std::string outputFormat;
};

void Cleanup();
void ReadCmd(PSTR szCmdLine ,ConvertInfo& outInfo);
Noise3D::NOISE_IMAGE_FILE_FORMAT GetFormat(std::string fm);

using namespace Noise3D;

IRoot* pRoot;
IRenderer* pRenderer;
IScene* pScene;
IMaterialManager*	pMatMgr;
ITextureManager*	pTexMgr;

//Main Entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//get Root interface
	pRoot = GetRoot();

	if (!pRoot->Init())return FALSE;

	//query pointer to IScene
	pScene = pRoot->GetScenePtr();
	pMatMgr = pScene->GetMaterialMgr();
	pTexMgr = pScene->GetTextureMgr();

	//read conversion info
	ConvertInfo info;
	ReadCmd(szCmdLine, info);

	//convert
	ITexture* pNormalMap = pTexMgr->CreateTextureFromFile(info.filePath, "texture", true, 0, 0, true);
	pNormalMap->ConvertTextureToGreyMap(info.r, info.g, info.b);
	pNormalMap->ConvertHeightMapToNormalMap(info.heightFieldScale);
	pNormalMap->SaveTextureToFile(info.filePath + "_NormalMap." + info.outputFormat, GetFormat(info.outputFormat));

	Cleanup();

	return true;
}

void Cleanup()
{
	pRoot->ReleaseAll();
};

void ReadCmd(PSTR szCmdLine, ConvertInfo & outInfo)
{
	std::stringstream ss;
	
	ss << szCmdLine;
	ss >> outInfo.filePath;
	ss >> outInfo.heightFieldScale;
	ss >> outInfo.r;
	ss >> outInfo.g;
	ss >> outInfo.b;
	ss >> outInfo.outputFormat;
}

Noise3D::NOISE_IMAGE_FILE_FORMAT GetFormat(std::string fm)
{
	if (fm == "bmp")return NOISE_IMAGE_FILE_FORMAT_BMP;
	if (fm == "jpg")return NOISE_IMAGE_FILE_FORMAT_JPG;
	if (fm == "dds")return NOISE_IMAGE_FILE_FORMAT_DDS;
	std::cout << "Image format not supported!! Defaulted to .dds!" << std::endl;
	return NOISE_IMAGE_FILE_FORMAT_DDS;
}
