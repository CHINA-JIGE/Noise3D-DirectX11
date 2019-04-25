
#include "Noise3D.h"
#include "RealTimeRenderModule.h"
#include "MainApp.h"

MainApp app;

//Main Entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	app.Init(hInstance);
	app.InitCmdLine(szCmdLine);
	app.RunMainloop();

	//..
	return 0;
}