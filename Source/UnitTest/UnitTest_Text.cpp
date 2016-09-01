

#include "Main3D.h"
#include <sstream>

using namespace Noise3D;

IRoot* pRoot;
IRenderer* pRenderer;
IScene* pScene;
IFontManager* pFontMgr;
IDynamicText* pMyText_fps;
IStaticText*		pMyText1;
IDynamicText* pMyText2;

Ut::ITimer NTimer(NOISE_TIMER_TIMEUNIT_MILLISECOND);
Ut::IInputEngine inputE;

//Main Entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//get Root interface
	pRoot = GetRoot();

	//create a window (using default window creation function)
	HWND windowHWND;
	windowHWND = pRoot->CreateRenderWindow(640, 480, L"Hahaha Render Window", hInstance);

	//initialize input engine (detection for keyboard and mouse input)
	inputE.Initialize(hInstance, windowHWND);

	//D3D and scene object init
	Init3D(windowHWND);

	//register main loop function
	pRoot->SetMainLoopFunction(MainLoop);

	//enter main loop
	pRoot->Mainloop();

	//program end
	Cleanup();

	//..
	return 0;
}

BOOL Init3D(HWND hwnd)
{
	const UINT bufferWidth = 640;
	const UINT bufferHeight = 480;

	//初始化失败
	if (!pRoot->InitD3D(hwnd, bufferWidth, bufferHeight, TRUE))return FALSE;

	//query pointer to IScene
	pScene = pRoot->GetScenePtr();

	pRenderer = pScene->GetRenderer();

	//create font texture
	pFontMgr = pScene->GetFontMgr();
	pFontMgr->CreateFontFromFile("media//STHUPO.ttf", "font1", 36);
	pFontMgr->CreateFontFromFile("media//STXINWEI.TTF", "font2", 36);
	pMyText_fps = pFontMgr->CreateDynamicTextA("font1", "fpsLabel", "fps:000", 200, 100, NVECTOR4(0, 0, 0, 1.0f), 0, 0);
	pMyText_fps->SetTextColor(NVECTOR4(0, 0.3f, 1.0f, 0.5f));
	pMyText_fps->SetDiagonal(NVECTOR2(20, 20), NVECTOR2(150, 60));
	pMyText_fps->SetFont("font2");

	pMyText1 = pFontMgr->CreateStaticTextW("font2", "testLabel1", L"我了个去！鸡哥最他妈帅了。、，=", 500, 300);
	pMyText1->SetTextColor(NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	pMyText1->SetCenterPos({ 500,200 });

	pMyText2 = pFontMgr->CreateDynamicTextA("font1", "testLabel2", "testtesttest!!.,/!@#$%^", 300, 100);
	pMyText2->SetTextColor(NVECTOR4(1.0f, 1.0f, 0, 1.0f));
	pMyText2->SetCenterPos({ 300,300 });

	pRenderer->SetFillMode(NOISE_FILLMODE_SOLID);
	pRenderer->SetCullMode(NOISE_CULLMODE_NONE);//NOISE_CULLMODE_BACK

	return TRUE;
};


void MainLoop()
{

	//GUIMgr.Update();
	InputProcess();
	pRenderer->ClearBackground();
	NTimer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS << "fps :" << NTimer.GetFPS() << std::endl;
	pMyText_fps->SetTextAscii(tmpS.str());


	//add to render list
	pRenderer->AddObjectToRenderList(pMyText_fps);
	pRenderer->AddObjectToRenderList(pMyText1);
	pRenderer->AddObjectToRenderList(pMyText2);

	//render
	pRenderer->SetBlendingMode(NOISE_BLENDMODE_ALPHA);
	pRenderer->RenderTexts();

	//present
	pRenderer->RenderToScreen();
};

void InputProcess()
{
};


void Cleanup()
{
	pRoot->ReleaseAll();
};
