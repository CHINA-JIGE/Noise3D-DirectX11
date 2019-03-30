#include "Noise3D.h"
#include <sstream>

BOOL Init3D(HWND hwnd);
void SHPreprocess();
void MainLoop();
void Cleanup();
void	InputProcess();

using namespace Noise3D;

IRoot* pRoot;
IRenderer* pRenderer;
IScene* pScene;
ICamera* pCamera;
IAtmosphere* pAtmos;
IModelLoader* pModelLoader;
IMeshManager* pMeshMgr;
std::vector<IMesh*> meshList;
IMaterialManager*	pMatMgr;
ITextureManager*	pTexMgr;
ITexture* pOriginTex;
ITexture* pShTex;
IGraphicObjectManager*	pGraphicObjMgr;
IGraphicObject*	pGO_Axis;
IGraphicObject*	pGO_GUI;
IFontManager* pFontMgr;
IDynamicText* pMyText_fps;


Ut::Timer timer(Ut::NOISE_TIMER_TIMEUNIT_MILLISECOND);
Ut::InputEngine inputE;

//Main Entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{

	//get Root interface
	pRoot = GetRoot();

	//create a window (using default window creation function)
	HWND windowHWND;
	windowHWND = pRoot->CreateRenderWindow(1280, 800, L"Hahaha Render Window", hInstance);

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
	const UINT bufferWidth = 1200;
	const UINT bufferHeight = 600;

	//init fail
	if (!pRoot->Init())return FALSE;

	//query pointer to IScene
	pScene = pRoot->GetScenePtr();

	pMeshMgr = pScene->GetMeshMgr();
	pRenderer = pScene->CreateRenderer(bufferWidth, bufferHeight, hwnd);
	pCamera = pScene->GetCamera();
	pMatMgr = pScene->GetMaterialMgr();
	pTexMgr = pScene->GetTextureMgr();
	pAtmos = pScene->GetAtmosphere();
	pGraphicObjMgr = pScene->GetGraphicObjMgr();

	pOriginTex = pTexMgr->CreateTextureFromFile("../media/chuyin.jpg", "Tex", true, 512, 512, true);
	pShTex = pTexMgr->CreatePureColorTexture("ShTex", 512, 512, Vec4(1.0f, 0.0f, 0.0f, 1.0f), true);
	SHPreprocess();

	//create font texture
	pFontMgr = pScene->GetFontMgr();
	pFontMgr->CreateFontFromFile("../media/calibri.ttf", "myFont", 24);
	pMyText_fps = pFontMgr->CreateDynamicTextA("myFont", "fpsLabel", "fps:000", 200, 100, Vec4(0, 0, 0, 1.0f), 0, 0);
	pMyText_fps->SetTextColor(Vec4(0, 0.3f, 1.0f, 0.5f));
	pMyText_fps->SetDiagonal(Vec2(20, 20), Vec2(300, 60));
	pMyText_fps->SetFont("myFont");
	pMyText_fps->SetBlendMode(NOISE_BLENDMODE_ALPHA);


	//----------------------------------------------------------

	pCamera->SetPosition(0, 100.0f, 0);
	pCamera->SetLookAt(0, 0, 0);
	pCamera->SetViewAngle_Radian(MATH_PI / 3.0f, 1.333333333f);
	pCamera->SetViewFrustumPlane(1.0f, 500.f);


	pModelLoader->LoadSkyDome(pAtmos, "Universe", 4.0f, 2.0f);
	pAtmos->SetFogEnabled(false);
	pAtmos->SetFogParameter(50.0f, 100.0f, Vec3(0, 0, 1.0f));


	/*pGO_Axis = pGraphicObjMgr->CreateGraphicObj("Axis");
	pGO_Axis->AddRectangle(Vec2(1080.0f, 780.0f), Vec2(1280.0f,800.0f), Vec4(0.3f, 0.3f, 1.0f, 1.0f), "BottomRightTitle");
	pGO_Axis->SetBlendMode(NOISE_BLENDMODE_ALPHA);
	pGO_Axis->AddLine3D({ 0,0,0 }, { 100.0f,0,0 }, { 1.0f,0,0,1.0f }, { 1.0f,0,0,1.0f });
	pGO_Axis->AddLine3D({ 0,0,0 }, { 0,100.0f,0 }, { 0,1.0f,0,1.0f }, { 0,1.0f,0,1.0f });
	pGO_Axis->AddLine3D({ 0,0,0 }, { 0,0,100.0f }, { 0,0,1.0f,1.0f }, { 0,0,1.0f,1.0f });*/
	pGO_GUI = pGraphicObjMgr->CreateGraphicObj("tanList");
	pGO_GUI->SetBlendMode(NOISE_BLENDMODE_ALPHA);
	pGO_GUI->AddRectangle(Vec2(50.0f, 50.0f), Vec2(50.0f + 512.0f, 50.0f+512.0f), Vec4(1.0f, 0, 0, 1.0f), "Tex");
	pGO_GUI->AddRectangle(Vec2(50.0f+512.0f+30.0f, 50.0f), Vec2(50.0f + 512.0f + 30.0f + 512.0f, 50.0f + 512.0f), Vec4(1.0f, 0, 0, 1.0f), "ShTex");

	return TRUE;
};

void SHPreprocess()
{
	GI::SHVector shvec;
	GI::ISphericalMappingTextureSampler defaultSphFunc;
	defaultSphFunc.SetTexture(pOriginTex);
	shvec.Project(3, 10000, &defaultSphFunc);

	uint32_t width = pShTex->GetWidth();
	uint32_t height = pShTex->GetHeight();
	std::vector<NColor4u> colorBuff(width*height);
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			float normalizedU = float(x) / float(width);//[0,1]
			float normalizedV = float(y) / float(height);//[0,1]
			float yaw = (normalizedU - 0.5f) * 2.0f * MATH_PI;//[-pi,pi]
			float pitch = (normalizedV - 0.5f) * MATH_PI;//[pi/2,-pi/2]
			Vec3 dir = { sinf(yaw)*cosf(pitch),  sinf(pitch) ,cosf(yaw)*cosf(pitch)};
			Vec3 reconstructedColor = shvec.Eval(dir);
			NColor4u color = { uint8_t(reconstructedColor.x * 255.0f), uint8_t(reconstructedColor.y * 255.0f) , uint8_t(reconstructedColor.z * 255.0f),255 };
			colorBuff.at(y*width + x) = color;
		}
	}
	pShTex->SetPixelArray(colorBuff);
	pShTex->UpdateToVideoMemory();
}

void MainLoop()
{
	static float incrNum = 0.0;

	InputProcess();
	pRenderer->ClearBackground(Vec4(0.2f,0.2f,0.2f,1.0f));
	timer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS << "fps :" << timer.GetFPS();
	pMyText_fps->SetTextAscii(tmpS.str());


	//add to render list
	//for (auto& pMesh : meshList)pRenderer->AddToRenderQueue(pMesh);
	pRenderer->AddToRenderQueue(pGO_Axis);
	pRenderer->AddToRenderQueue(pGO_GUI);
	pRenderer->AddToRenderQueue(pMyText_fps);

	//render
	pRenderer->Render();

	//present
	pRenderer->PresentToScreen();
};

void InputProcess()
{
	inputE.Update();

	if (inputE.IsKeyPressed(Ut::NOISE_KEY_A))
	{
		pCamera->fps_MoveRight(-0.02f * (float)timer.GetInterval() , FALSE);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_D))
	{
		pCamera->fps_MoveRight(0.02f *  (float)timer.GetInterval(), FALSE);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_W))
	{
		pCamera->fps_MoveForward(0.02f*  (float)timer.GetInterval(), FALSE);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_S))
	{
		pCamera->fps_MoveForward(-0.02f*  (float)timer.GetInterval(), FALSE);
	}

	if (inputE.IsMouseButtonPressed(Ut::NOISE_MOUSEBUTTON_LEFT))
	{
		pCamera->RotateY_Yaw((float)inputE.GetMouseDiffX() / 200.0f);
		pCamera->RotateX_Pitch((float)inputE.GetMouseDiffY() / 200.0f);
	}

	//quit main loop and terminate program
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_ESCAPE))
	{
		pRoot->SetMainLoopStatus(NOISE_MAINLOOP_STATUS_QUIT_LOOP);
	}

};

void Cleanup()
{
	pRoot->ReleaseAll();
};
