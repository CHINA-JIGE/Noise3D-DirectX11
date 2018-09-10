#include "Main3D.h"

using namespace Noise3D;

Main3DApp::Main3DApp():
	mTimer(Ut::NOISE_TIMER_TIMEUNIT_MILLISECOND)
{

}

Main3DApp::~Main3DApp()
{

}

void Main3DApp::InitNoise3D(HWND hwnd)
{
	//initialize input engine (detection for keyboard and mouse input)
	//https://stackoverflow.com/questions/4547073/c-on-windows-using-directinput-without-a-window
	//it is said that an hInstance/hWnd is not necessary for a dinput device???
	mInputE.Initialize(::GetModuleHandle(NULL), hwnd);

}

void Main3DApp::UpdateFrame()
{
}

void Main3DApp::Cleanup()
{
}
