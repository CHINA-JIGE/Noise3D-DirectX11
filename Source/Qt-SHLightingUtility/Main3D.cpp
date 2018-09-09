#include "stdafx.h"//pch
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
	mInputE.Initialize(hInstance, hwnd);

}

void Main3DApp::UpdateFrame()
{
}

void Main3DApp::Cleanup()
{
}
