
#include "Main3D.h"

using namespace Noise3D;

IRoot* pRoot;
IRenderer* pRenderer;
IScene* pScene;

//Main Entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//Get the only ROOT of Noise3D
	pRoot = GetRoot();


	//use internal windows creation function
	HWND windowHWND;
	windowHWND = pRoot->CreateRenderWindow(640, 480, L"Hahaha Render Window", hInstance);

	const UINT bufferWidth = 640;
	const UINT bufferHeight = 480;
	BOOL initSucceed = pRoot->InitD3D(windowHWND, bufferWidth, bufferHeight, TRUE);
	if (!initSucceed)return FALSE;

	//Get the only SCENE of Noise3D::Root
	pScene = pRoot->GetScenePtr();
	pRenderer = pScene->GetRenderer();

	//register MAINLOOP function (it will be called every frame)
	pRoot->SetMainLoopFunction(MainLoop);

	//do some customized init stuff
	Init3D();

	//start main loop
	pRoot->Mainloop();

	//terminate cleaning
	Cleanup();
	return 0;
}

BOOL Init3D()
{

	return TRUE;
};


void MainLoop()
{
	pRenderer->ClearBackground();

	//present
	pRenderer->RenderToScreen();
};


void Cleanup()
{
	pRoot->ReleaseAll();
};

void	InputProcess() {};