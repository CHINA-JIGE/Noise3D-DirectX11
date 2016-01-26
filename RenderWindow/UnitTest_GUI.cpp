
#include "Main3D.h"
#include <sstream>

NoiseEngine Engine;
NoiseRenderer Renderer;
NoiseScene Scene;
NoiseTextureManager	TexMgr;
NoiseGraphicObject	GraphicObj;

NoiseGUIManager		GUIMgr;
NoiseGUIButton			GUIButton1;
NoiseGUIScrollBar			GUIScrollBar1(TRUE);
NoiseGUITextBox			GUITextBox1;

NoiseFontManager		fontMgr;
Noise2DTextStatic			myText1;
Noise2DTextDynamic	myText2;
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

	//初始化失败
	if (!Engine.InitD3D(hwnd, bufferWidth, bufferHeight, TRUE))return FALSE;

	Scene.InitRenderer(Renderer);
	Scene.InitTextureManager(TexMgr);
	fontMgr.Initialize();


	//漫反射贴图
	TexMgr.CreateTextureFromFile(L"button1.png", "GUI_Button1", TRUE, 0, 0, FALSE);
	TexMgr.CreateTextureFromFile(L"button2.png", "GUI_Button2", TRUE, 0, 0, FALSE);
	TexMgr.CreateTextureFromFile(L"button3.png", "GUI_Button3", TRUE, 0, 0, FALSE);
	TexMgr.CreateTextureFromFile(L"button4.png", "GUI_Button4", TRUE, 0, 0, FALSE);
	TexMgr.CreateTextureFromFile(L"universe2.jpg", "Universe", FALSE, 512, 512, FALSE);
	TexMgr.CreateTextureFromFile(L"bottom-right-conner-title.jpg", "BottomRightTitle", TRUE, 0, 0, FALSE);

	//create font texture
	fontMgr.CreateFontFromFile("STXINWEI.ttf", "myFont", 24);

	fontMgr.InitStaticTextW(0, L"TextBox的CapsLock和大小写/ GUI system需不需要重构一下啊....很多Event是比较重复的....= =", 300, 100, NVECTOR4(0, 1.0f, 0.5f, 1.0f), 0, 0, myText1);

	myText1.SetTextColor(NVECTOR4(1.0f, 0, 0, 0.5f));
	myText1.SetCenterPos(300.0f, 100.0f);
	fontMgr.InitDynamicTextA(0, "abcdefghijklmnopqrstuvwxyz!@#$%^&*()_<>-+?/+ 1234567890<>?,./{}[]\\", 300, 100, NVECTOR4(0, 1.0f, 0.5f, 1.0f), 0, 0, myText2);
	myText2.SetTextColor(NVECTOR4(0.5f, 0.3f, 1.0f, 0.5f));
	myText2.SetCenterPos(300.0f, 400.0f);
	fontMgr.InitDynamicTextA(0, "fps:000", 200, 100, NVECTOR4(0, 0, 0, 1.0f), 0, 0, myText_fps);
	myText_fps.SetTextColor(NVECTOR4(0, 0.3f, 1.0f, 0.5f));
	myText_fps.SetDiagonal(NVECTOR2(20, 20), NVECTOR2(150, 60));
	myText_fps.SetFont(0);

	Renderer.SetFillMode(NOISE_FILLMODE_SOLID);
	Renderer.SetCullMode(NOISE_CULLMODE_BACK);//NOISE_CULLMODE_BACK


	GraphicObj.AddRectangle(NVECTOR2(340.0f, 430.0f), NVECTOR2(640.0f, 480.0f), NVECTOR4(0.3f, 0.3f, 1.0f, 1.0f), TexMgr.GetTextureID("BottomRightTitle"));


	//GUI System
	GUIMgr.Initialize(bufferWidth, bufferHeight, Renderer, inputE, fontMgr, hwnd);
	GUIMgr.SetWindowHWND(hwnd);
	GUIMgr.SetFontManager(fontMgr);
	GUIMgr.InitButton(GUIButton1);
	GUIMgr.InitScrollBar(GUIScrollBar1);
	GUIMgr.InitTextBox(GUITextBox1, 0);
	GUIButton1.SetCenterPos(150.0f, 50.0f);
	GUIButton1.SetWidth(300.0f);
	GUIButton1.SetHeight(100.0f);
	GUIButton1.SetDragableX(TRUE);
	GUIButton1.SetDragableY(TRUE);//TRUE);
	GUIButton1.SetTexture(NOISE_GUI_BUTTON_STATE_COMMON, TexMgr.GetTextureID("GUI_Button1"));
	GUIButton1.SetTexture(NOISE_GUI_BUTTON_STATE_MOUSEON, TexMgr.GetTextureID("GUI_Button2"));
	GUIButton1.SetTexture(NOISE_GUI_BUTTON_STATE_MOUSEBUTTONDOWN, TexMgr.GetTextureID("GUI_Button3"));
	GUIButton1.SetEventProcessCallbackFunction(Button1MsgProc);
	GUIScrollBar1.SetCenterPos(300.0f, 300.0f);
	GUIScrollBar1.SetWidth(30.0f);
	GUIScrollBar1.SetHeight(300.0f);
	GUIScrollBar1.SetTexture_ScrollButton(NOISE_GUI_BUTTON_STATE_COMMON, TexMgr.GetTextureID("GUI_Button1"));
	GUIScrollBar1.SetTexture_ScrollButton(NOISE_GUI_BUTTON_STATE_MOUSEON, TexMgr.GetTextureID("GUI_Button2"));
	GUIScrollBar1.SetTexture_ScrollButton(NOISE_GUI_BUTTON_STATE_MOUSEBUTTONDOWN, TexMgr.GetTextureID("GUI_Button3"));
	GUIScrollBar1.SetTexture_ScrollGroove(TexMgr.GetTextureID("GUI_Button4"));
	GUIScrollBar1.SetAlignment(FALSE);
	GUIScrollBar1.SetValue(0.5f);
	GUITextBox1.SetTextColor({ 1.0f,0,0,1.0f });
	GUITextBox1.SetCenterPos(200.0f, 400.0f);
	GUITextBox1.SetWidth(300.0f);
	GUITextBox1.SetHeight(80.0f);
	GUITextBox1.SetFont(0);
	GUITextBox1.SetTexture_BackGround(TexMgr.GetTextureID("GUI_Button3"));
	GUITextBox1.SetTexture_Cursor(TexMgr.GetTextureID("GUI_Button2"));
	GUIMgr.Update();


	return TRUE;
};


void MainLoop()
{
	inputE.Update();
	GUIMgr.Update();

	Renderer.ClearBackground();
	NTimer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS << "fps :" << NTimer.GetFPS() << std::endl;//I wonder why this FPS is EXACTLY the half of Graphic Debug FPS
	myText_fps.SetTextAscii(tmpS.str());
	std::stringstream tmpS2;
	tmpS2 << GUIScrollBar1.GetValue();
	float aaaa = GUIScrollBar1.GetValue();
	myText2.SetTextAscii(tmpS2.str());

	//add to render list
	Renderer.AddObjectToRenderList(myText1);
	Renderer.AddObjectToRenderList(myText2);
	Renderer.AddObjectToRenderList(myText_fps);

	GUIMgr.AddObjectToRenderList(GUIScrollBar1);
	GUIMgr.AddObjectToRenderList(GUIButton1);
	GUIMgr.AddObjectToRenderList(GUITextBox1);


	//render
	Renderer.SetBlendingMode(NOISE_BLENDMODE_ALPHA);
	Renderer.RenderGraphicObjects();
	Renderer.SetBlendingMode(NOISE_BLENDMODE_ALPHA);
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