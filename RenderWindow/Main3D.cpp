

#include "Main3D.h"
#include <sstream>

NoiseEngine Engine;
NoiseRenderer Renderer;
NoiseScene Scene;
NoiseMesh Mesh1;
NoiseCamera Camera;
NoiseLightManager LightMgr;
NoiseMaterialManager	MatMgr;
NoiseTextureManager	TexMgr;
NoiseAtmosphere			Atmos;
NoiseGUIManager		GUIMgr;
NoiseGUIButton			GUIButton1;
NoiseGUIScrollBar			GUIScrollBar1(TRUE);
NoiseGUITextBox			GUITextBox1;
NoiseGraphicObject	GraphicObjBuffer;
NoiseFontManager		fontMgr;
Noise2DTextStatic			myText1;
Noise2DTextDynamic	myText2;
Noise2DTextDynamic	myText_fps;

NoiseUtTimer NTimer(NOISE_TIMER_TIMEUNIT_MILLISECOND);
NoiseUtSlicer Slicer;
NoiseUtInputEngine inputE;
N_DirectionalLight DirLight1;
N_PointLight	 PointLight1;
N_SpotLight	SpotLight1;

float moveForwardVelocity;
std::vector<NVECTOR3>	 lineSegmentBuffer;
std::vector<N_LineStrip>	lineStrip;


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
	if(!Engine.InitD3D( hwnd, bufferWidth, bufferHeight,TRUE))return FALSE;

	Scene.InitMesh(Mesh1);
	Scene.InitRenderer(Renderer);
	Scene.InitCamera(Camera);
	Scene.InitLightManager(LightMgr);
	Scene.InitMaterialManager(MatMgr);
	Scene.InitTextureManager(TexMgr);
	Scene.InitAtmosphere(Atmos);
	fontMgr.Initialize();


	//漫反射贴图
	//TexMgr.CreateTextureFromFile(L"Earth.jpg", "Earth", TRUE,0, 0, TRUE);
	//TexMgr.CreatePureColorTexture("myText", 300, 100, NVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), TRUE);
	TexMgr.CreateTextureFromFile(L"button1.png", "GUI_Button1", TRUE, 0, 0, FALSE);
	TexMgr.CreateTextureFromFile(L"button2.png", "GUI_Button2", TRUE, 0, 0, FALSE);
	TexMgr.CreateTextureFromFile(L"button3.png", "GUI_Button3", TRUE, 0, 0, FALSE);
	TexMgr.CreateTextureFromFile(L"button4.png", "GUI_Button4", TRUE, 0, 0, FALSE);
	TexMgr.CreateTextureFromFile(L"Earth.jpg","Earth", TRUE,0, 0,FALSE);
	TexMgr.CreateTextureFromFile(L"Earth.jpg", "EarthNormalMap", TRUE, 0, 0, TRUE);
	TexMgr.CreateTextureFromFile(L"texture2.jpg", "Wood", TRUE, 0, 0, FALSE);
	TexMgr.CreateTextureFromFile(L"universe2.jpg", "Universe", FALSE, 512, 512, FALSE);
	TexMgr.CreateTextureFromFile(L"bottom-right-conner-title.jpg", "BottomRightTitle", TRUE, 0, 0, FALSE);
	TexMgr.CreateCubeMapFromDDS(L"UniverseEnv.dds", "EnvironmentMap",NOISE_CUBEMAP_SIZE_256x256);
	TexMgr.ConvertTextureToGreyMap(TexMgr.GetTextureID("EarthNormalMap"));
	TexMgr.ConvertHeightMapToNormalMap(TexMgr.GetTextureID("EarthNormalMap"),20.0f);

	//create font texture
	fontMgr.CreateFontFromFile("STXINWEI.ttf", "myFont", 24);

	fontMgr.InitStaticTextW(0, L"TextBox的CapsLock和大小写/ GUI system需不需要重构一下啊....很多Event是比较重复的....= =", 300, 100, NVECTOR4(0, 1.0f, 0.5f, 1.0f), 0, 0, myText1);

	myText1.SetTextColor(NVECTOR4(1.0f, 0, 0, 0.5f));
	myText1.SetCenterPos(300.0f, 100.0f);
	fontMgr.InitDynamicTextA(0, "abcdefghijklmnopqrstuvwxyz!@#$%^&*()_<>-+?/+ 1234567890<>?,./{}[]\\", 300, 100, NVECTOR4(0, 1.0f, 0.5f, 1.0f), 0, 0, myText2);
	myText2.SetTextColor(NVECTOR4(0.5f, 0.3f, 1.0f, 0.5f));
	myText2.SetCenterPos(300.0f,400.0f);
	fontMgr.InitDynamicTextA(0, "fps:000", 200, 100, NVECTOR4(0,0,0,1.0f), 0, 0, myText_fps);
	myText_fps.SetTextColor(NVECTOR4(0,0.3f,1.0f,0.5f));
	myText_fps.SetDiagonal(NVECTOR2(20, 20),NVECTOR2(150, 60));
	myText_fps.SetFont(0);

	Renderer.SetFillMode(NOISE_FILLMODE_SOLID);
	Renderer.SetCullMode(NOISE_CULLMODE_BACK);//NOISE_CULLMODE_BACK

	//Mesh1.LoadFile_STL("object.stl");
	Mesh1.CreateSphere(5.0f, 30, 30);
	//Mesh1.CreatePlane(10.0f, 10.0f, 5, 5);
	Mesh1.SetPosition(0,0,0);

	Camera.SetPosition(2.0f,0,0);
	Camera.SetLookAt(0,0,0);
	Camera.SetViewAngle(MATH_PI / 2.5f, 1.333333333f);
	Camera.SetViewFrustumPlane(1.0f, 500.f);
	//use bounding box of mesh to init camera pos
	NVECTOR3 AABB_MAX = Mesh1.GetBoundingBoxMax();
	float rotateRadius = sqrtf(AABB_MAX.x*AABB_MAX.x + AABB_MAX.z*AABB_MAX.z)*1.2f;
	float rotateY = Mesh1.GetBoundingBoxMax().y*1.3f;
	Camera.SetPosition(rotateRadius*0.7f, rotateY, rotateRadius*0.7f);
	Camera.SetLookAt(0, rotateY / 2, 0);

	Atmos.SetFogEnabled(FALSE);
	Atmos.SetFogParameter(7.0f, 8.0f, NVECTOR3(0, 0, 1.0f));
	Atmos.CreateSkyDome(4.0f, 4.0f, TexMgr.GetTextureID("Universe"));
	//Atmos.CreateSkyBox(10.0f, 10.0f, 10.0f, TexMgr.GetIndexByName("EnvironmentMap"));

	//——————灯光————————
	DirLight1.mAmbientColor = NVECTOR3(1.0f,1.0f,1.0f);
	DirLight1.mDiffuseColor	=	NVECTOR3(1.0f,1.0f,1.0f);
	DirLight1.mSpecularColor	=NVECTOR3(1.0f,1.0f,1.0f);
	DirLight1.mDirection = NVECTOR3(0.0f, -0.5f, 1.0f);
	DirLight1.mSpecularIntensity	=1.5f;
	DirLight1.mDiffuseIntensity = 1.0f;
	LightMgr.AddDynamicDirLight(DirLight1);

	N_Material Mat1;
	Mat1.baseMaterial.mBaseAmbientColor	= NVECTOR3(0.1f,  0.1f,0.1f);
	Mat1.baseMaterial.mBaseDiffuseColor		= NVECTOR3(1.0f,  1.0f, 1.0f);
	Mat1.baseMaterial.mBaseSpecularColor	=	NVECTOR3(1.0f, 1.0f,1.0f);
	Mat1.baseMaterial.mSpecularSmoothLevel	=	40;
	Mat1.baseMaterial.mNormalMapBumpIntensity = 0.3f;
	Mat1.baseMaterial.mEnvironmentMapTransparency = 0.05f;
	Mat1.diffuseMapID = TexMgr.GetTextureID("Wood");//"Earth");
	Mat1.normalMapID = TexMgr.GetTextureID("EarthNormalMap");
	Mat1.cubeMap_environmentMapID = NOISE_MACRO_INVALID_TEXTURE_ID;//TexMgr.GetTextureID("EnvironmentMap");
	UINT	 Mat1_ID = MatMgr.CreateMaterial(Mat1);

	//set material
	Mesh1.SetMaterial(Mat1_ID);

	GraphicObjBuffer.AddRectangle(NVECTOR2(340.0f, 430.0f), NVECTOR2(640.0f, 480.0f), NVECTOR4(0.3f, 0.3f, 1.0f, 1.0f),TexMgr.GetTextureID("BottomRightTitle"));
	
	//GUI System
	GUIMgr.Initialize(bufferWidth, bufferHeight, Renderer, inputE, fontMgr, hwnd);
	GUIMgr.SetWindowHWND(hwnd);
	GUIMgr.SetFontManager(fontMgr);
	GUIMgr.InitButton(GUIButton1);
	GUIMgr.InitScrollBar(GUIScrollBar1);
	GUIMgr.InitTextBox(GUITextBox1,0);
	GUIButton1.SetCenterPos(150.0f, 50.0f);
	GUIButton1.SetWidth(300.0f);
	GUIButton1.SetHeight(100.0f);
	GUIButton1.SetDragableX(TRUE);
	GUIButton1.SetDragableY(TRUE);
	GUIButton1.SetTexture(NOISE_GUI_BUTTON_STATE_COMMON,TexMgr.GetTextureID("GUI_Button2"));
	GUIButton1.SetTexture(NOISE_GUI_BUTTON_STATE_MOUSEON,TexMgr.GetTextureID("GUI_Button3"));
	GUIButton1.SetTexture(NOISE_GUI_BUTTON_STATE_MOUSEBUTTONDOWN,TexMgr.GetTextureID("GUI_Button4"));
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
	GUITextBox1.SetTexture_BackGround( TexMgr.GetTextureID("GUI_Button3"));
	GUITextBox1.SetTexture_Cursor( TexMgr.GetTextureID("GUI_Button2"));
	GUIMgr.Update();




	/*Slicer.Step1_LoadPrimitiveMeshFromSTLFile("object.stl");
	Slicer.Step2_Intersection(5);
	Slicer.Step3_GenerateLineStrip();// extremely neccessary for optimization
	Slicer.Step4_SaveLayerDataToFile("object.LayerOutput");*/
	//Slicer.Step3_LoadLineStripsFrom_NOISELAYER_File("object.LayerOutput");

	/*NVECTOR3 v1, v2, n;

	for (UINT i = 0;i < Slicer.GetLineStripCount();i++)
	{

		Slicer.GetLineStrip(lineStrip, i);
		for (UINT j = 0;j < lineStrip.at(i).pointList.size() - 1;j++)
		{
			v1 = lineStrip.at(i).pointList.at(j);
			v2 = lineStrip.at(i).pointList.at(j + 1);
			n = lineStrip.at(i).normalList.at(j);
			//vertex
			GraphicObjBuffer.AddLine3D(v1, v2);
			//normal
			GraphicObjBuffer.AddLine3D((v1 + v2) / 2, ((v1 + v2) / 2) + n / 4, NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4(1.0f, 0.2f, 0.2f, 1.0f));
		}
	}*/
	
	return TRUE;
};


void MainLoop()
{
	GUIMgr.Update();
	InputProcess();
	Renderer.ClearBackground();
	NTimer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS <<"fps :" << NTimer.GetFPS()<<std::endl;//I wonder why this FPS is EXACTLY the half of Graphic Debug FPS
	myText_fps.SetTextAscii(tmpS.str());
	std::stringstream tmpS2;
	tmpS2 << GUIScrollBar1.GetValue();
	float aaaa = GUIScrollBar1.GetValue();
	myText2.SetTextAscii(tmpS2.str());

	//add to render list
	Renderer.AddObjectToRenderList(Mesh1);
	Renderer.AddObjectToRenderList(GraphicObjBuffer);
	Renderer.AddObjectToRenderList(Atmos);
	Renderer.AddObjectToRenderList(myText1);
	Renderer.AddObjectToRenderList(myText2);
	Renderer.AddObjectToRenderList(myText_fps);
	
	GUIMgr.AddObjectToRenderList(GUIScrollBar1);
	GUIMgr.AddObjectToRenderList(GUIButton1);
	GUIMgr.AddObjectToRenderList(GUITextBox1);
	

	//render
	Renderer.SetBlendingMode(NOISE_BLENDMODE_OPAQUE);
	Renderer.RenderMeshes();
	Renderer.SetBlendingMode(NOISE_BLENDMODE_OPAQUE);
	Renderer.RenderAtmosphere();
	Renderer.SetBlendingMode(NOISE_BLENDMODE_OPAQUE);
	Renderer.RenderGraphicObjects();
	Renderer.SetBlendingMode(NOISE_BLENDMODE_ALPHA);
	Renderer.RenderGUIObjects();
	Renderer.SetBlendingMode(NOISE_BLENDMODE_ALPHA);
	Renderer.RenderTexts();

	//present
	Renderer.RenderToScreen();
};

void InputProcess()
{
	//inputE.Update();
	
	if (inputE.IsKeyPressed(NOISE_KEY_A))
	{
		Camera.fps_MoveRight(-0.1f, FALSE);
	}
	if (inputE.IsKeyPressed(NOISE_KEY_D))
	{
		Camera.fps_MoveRight(0.1f, FALSE);
	}
	if (inputE.IsKeyPressed(NOISE_KEY_W))
	{
		Camera.fps_MoveForward(0.1f, FALSE);
	}
	if (inputE.IsKeyPressed(NOISE_KEY_S))
	{
		Camera.fps_MoveForward(-0.1f, FALSE);
	}
	if (inputE.IsKeyPressed(NOISE_KEY_SPACE))
	{
		Camera.fps_MoveUp(0.1f);
	}
	if (inputE.IsKeyPressed(NOISE_KEY_LCONTROL))
	{
		Camera.fps_MoveUp(-0.1f);
	}

	if (inputE.IsMouseButtonPressed(NOISE_MOUSEBUTTON_LEFT))
	{
		Camera.RotateY_Yaw((float)inputE.GetMouseDiffX() / 200.0f);
		Camera.RotateX_Pitch((float)inputE.GetMouseDiffY() / 200.0f);
	}

	//quit main loop and terminate program
	if (inputE.IsKeyPressed(NOISE_KEY_ESCAPE))
	{
		Engine.SetMainLoopStatus(NOISE_MAINLOOP_STATUS_QUIT_LOOP);
	}

};


void Cleanup()
{
	inputE.SelfDestruction();
	Slicer.SelfDestruction();
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