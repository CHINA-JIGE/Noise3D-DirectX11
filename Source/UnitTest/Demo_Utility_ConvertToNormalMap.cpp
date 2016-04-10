
#include "Main3D.h"
#include <sstream>

IRoot IRoot;
IRenderer Renderer;
IScene IScene;
IGraphicObject		GraphicObj;
ITextureManager	TexMgr;

static std::wstring fileName;
static float heightFieldScale =10.0f;
static float colorFactorR = 0.3f;
static float colorFactorG = 0.59f;
static float colorFactorB = 0.1f;
static std::string outputFormat;

//Main Entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	std::stringstream sstr;
	sstr << szCmdLine;

	//FILE NAME
	std::string tmpFileNameA;
	sstr >> tmpFileNameA;
	//convert to wstring
	for (auto c : tmpFileNameA)fileName.push_back(c);

	//Bump Intensity
	sstr >> heightFieldScale;

	//color factor (when converting to grey map)
	sstr >> colorFactorR;
	sstr >> colorFactorG;
	sstr >> colorFactorB;

	//output format
	sstr >> outputFormat;
	for (auto& c : outputFormat)tolower(c);

	HWND windowHWND;
	windowHWND = IRoot.CreateRenderWindow(640, 480, L"Hahaha Render Window", hInstance);
	Init3D(windowHWND);
	Cleanup();
	return 0;
}

BOOL Init3D(HWND hwnd)
{
	const UINT bufferWidth = 640;
	const UINT bufferHeight = 480;

	//≥ı ºªØ ß∞‹
	if (!IRoot.InitD3D(hwnd, bufferWidth, bufferHeight, TRUE))return FALSE;

	IScene.CreateTextureManager(TexMgr);

	TexMgr.CreateTextureFromFile(fileName.c_str(), "picture", TRUE, 0, 0, TRUE);
	TexMgr.ConvertTextureToGreyMapEx("picture", colorFactorR, colorFactorG, colorFactorB);
	TexMgr.ConvertHeightMapToNormalMap("picture", heightFieldScale);
	TexMgr.UpdateTextureDataToGraphicMemory("picture");

	if (outputFormat == "bmp")
	{
		TexMgr.SaveTextureToFile("picture", (fileName + L"_NormalMap.bmp").c_str(), NOISE_TEXTURE_SAVE_FORMAT_BMP);
	}
	else
	{
		if (outputFormat == "jpg")
		{
			TexMgr.SaveTextureToFile("picture", (fileName + L"_NormalMap.jpg").c_str(), NOISE_TEXTURE_SAVE_FORMAT_JPG);
		}
		else
		{
			if (outputFormat == "png")
			{
				TexMgr.SaveTextureToFile("picture", (fileName + L"_NormalMap.png").c_str(), NOISE_TEXTURE_SAVE_FORMAT_PNG);
			}
			else
			{
				::MessageBoxA(0, "file format not support!!", 0, 0);
			}
		}
	}

	return TRUE;
};

void MainLoop() { return; };
void Cleanup() {};
void	InputProcess() {};
UINT Button1MsgProc(UINT msg) { return 0; };