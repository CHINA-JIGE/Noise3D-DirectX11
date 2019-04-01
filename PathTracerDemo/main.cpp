
#include "Noise3D.h"
#include "mainApp.h"

MainApp app;

//Main Entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	app.Init(hInstance);
	app.RunMainloop();

	//..
	return 0;
}