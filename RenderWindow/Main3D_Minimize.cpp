
#include "Main3D.h"

NoiseEngine Engine;
NoiseRenderer Renderer;
NoiseScene Scene;

//Main Entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND windowHWND;
	windowHWND = Engine.CreateRenderWindow(640, 480, L"Hahaha Render Window", hInstance);
	Init3D(windowHWND);
	Engine.SetMainLoopFunction(MainLoop);
	Engine.Mainloop();
	Cleanup();
	return 0;
}

BOOL Init3D(HWND hwnd)
{
	const UINT bufferWidth = 640;
	const UINT bufferHeight = 480;

	//≥ı ºªØ ß∞‹
	if (!Engine.InitD3D(hwnd, bufferWidth, bufferHeight, TRUE))return FALSE;
	Scene.InitRenderer(Renderer);
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
	Engine.ReleaseAll();
	Scene.ReleaseAllChildObject();
};





UINT Button1MsgProc(UINT msg) { return TRUE; };
void	InputProcess() {};