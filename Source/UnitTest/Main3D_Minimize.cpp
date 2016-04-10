
#include "Main3D.h"

IRoot IRoot;
IRenderer Renderer;
IScene IScene;

//Main Entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND windowHWND;
	windowHWND = IRoot.CreateRenderWindow(640, 480, L"Hahaha Render Window", hInstance);
	Init3D(windowHWND);
	IRoot.SetMainLoopFunction(MainLoop);
	IRoot.Mainloop();
	Cleanup();
	return 0;
}

BOOL Init3D(HWND hwnd)
{
	const UINT bufferWidth = 640;
	const UINT bufferHeight = 480;

	//≥ı ºªØ ß∞‹
	if (!IRoot.InitD3D(hwnd, bufferWidth, bufferHeight, TRUE))return FALSE;
	IScene.CreateRenderer(Renderer);
	return TRUE;
};


void MainLoop()
{
	Renderer.ClearBackground();

	//present
	Renderer.RenderToScreen();
};


void Cleanup()
{
	IRoot.ReleaseAll();
	IScene.ReleaseAllChildObject();
};





UINT Button1MsgProc(UINT msg) { return TRUE; };
void	InputProcess() {};