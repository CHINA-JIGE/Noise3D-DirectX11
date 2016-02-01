
#include "Main3D.h"
#include <sstream>

NoiseEngine Engine;
NoiseRenderer Renderer;
NoiseScene Scene;
NoiseTextureManager	TexMgr;
NoiseGraphicObject	GraphicObj;

NoiseGUIManager		GUIMgr;
NoiseGUIOscilloscope	GUIOsc(1.0f,1.0f);//Ê¾²¨Æ÷

NoiseFontManager		fontMgr;
Noise2DTextDynamic	myText_fps;

NoiseUtTimer NTimer(NOISE_TIMER_TIMEUNIT_MILLISECOND);
NoiseUtInputEngine inputE;


//Main Entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND windowHWND;
	windowHWND = Engine.CreateRenderWindow(640, 480, L"Hahaha Render Window", hInstance);
	inputE.Initialize(hInstance, windowHWND);
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

	//³õÊ¼»¯Ê§°Ü
	if (!Engine.InitD3D(hwnd, bufferWidth, bufferHeight, TRUE))return FALSE;

	Scene.InitRenderer(Renderer);
	Scene.InitTextureManager(TexMgr);
	fontMgr.Initialize();


	//Âþ·´ÉäÌùÍ¼
	TexMgr.CreateTextureFromFile(L"button1.png", "GUI_Button1", TRUE, 0, 0, FALSE);
	TexMgr.CreateTextureFromFile(L"button2.png", "GUI_Button2", TRUE, 0, 0, FALSE);
	TexMgr.CreateTextureFromFile(L"button3.png", "GUI_Button3", TRUE, 0, 0, FALSE);
	TexMgr.CreateTextureFromFile(L"button4.png", "GUI_Button4", TRUE, 0, 0, FALSE);
	TexMgr.CreateTextureFromFile(L"universe2.jpg", "Universe", FALSE, 512, 512, FALSE);
	TexMgr.CreateTextureFromFile(L"bottom-right-conner-title.jpg", "BottomRightTitle", TRUE, 0, 0, FALSE);

	//create font texture
	fontMgr.CreateFontFromFile("STXINWEI.ttf", "myFont", 24);
	fontMgr.InitDynamicTextA(0, "fps:000", 200, 100, NVECTOR4(0, 0, 0, 1.0f), 0, 0, myText_fps);
	myText_fps.SetTextColor(NVECTOR4(0, 0.3f, 1.0f, 0.5f));
	myText_fps.SetDiagonal(NVECTOR2(20, 20), NVECTOR2(150, 60));
	myText_fps.SetFont(0);

	Renderer.SetFillMode(NOISE_FILLMODE_SOLID);
	Renderer.SetCullMode(NOISE_CULLMODE_BACK);//NOISE_CULLMODE_BACK


	 //GUI System
	GUIMgr.Initialize(bufferWidth, bufferHeight, Renderer, inputE, fontMgr, hwnd);
	GUIMgr.InitOscilloscope(GUIOsc);
	GUIOsc.SetDiagonal({ 50.0f,50.0f }, {590.0f,430.0f});
	GUIOsc.SetSignalTimeInterval(0.2f);
	GUIOsc.SetScreenTimeRange(0.0f, 20.0f);
	GUIOsc.SetTriggerRecordingTime(20.0f);
	GUIOsc.SetMaxAmplitude(3.0f);

	GUIMgr.Update();


	return TRUE;
};


void MainLoop()
{
	static UINT frameCount = 0;
	++frameCount;
	inputE.Update();
	GUIMgr.Update();
	Renderer.ClearBackground();
	NTimer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS << "fps :" << NTimer.GetFPS() << std::endl;//I wonder why this FPS is EXACTLY the half of Graphic Debug FPS
	myText_fps.SetTextAscii(tmpS.str());

	if(frameCount%5==0)GUIOsc.PushSignal(sinf(0.05f*frameCount) + 0.01f * (rand() % 100));

	//add to render list
	Renderer.AddObjectToRenderList(myText_fps);
	GUIMgr.AddObjectToRenderList(GUIOsc);

	//render
	Renderer.SetBlendingMode(NOISE_BLENDMODE_ALPHA);
	Renderer.RenderGraphicObjects();
	Renderer.SetBlendingMode(NOISE_BLENDMODE_ADDITIVE);
	Renderer.RenderGUIObjects();
	Renderer.SetBlendingMode(NOISE_BLENDMODE_ALPHA);
	Renderer.RenderTexts();

	//present
	Renderer.RenderToScreen();
};


void Cleanup()
{
	inputE.SelfDestruction();
	NTimer.SelfDestruction();
	fontMgr.SelfDestruction();
	GUIMgr.SelfDestruction();

	Engine.ReleaseAll();
	Scene.ReleaseAllChildObject();
};


UINT Button1MsgProc(UINT msg)
{
	switch (msg)
	{
	case NOISE_GUI_EVENTS_COMMON_MOUSEDOWN:
		break;

	case NOISE_GUI_EVENTS_COMMON_MOUSEUP:
		break;

	case NOISE_GUI_EVENTS_COMMON_MOUSEMOVE:
		break;
	default:
		break;
	}
	return 0;
}

void	InputProcess() {};