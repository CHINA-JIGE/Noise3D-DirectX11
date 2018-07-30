#include "Noise3D.h"
#include <sstream>

BOOL Init3D(HWND hwnd);
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
IGraphicObjectManager*	pGraphicObjMgr;
IGraphicObject*	pGO_Axis;
IGraphicObject*	pGO_TanList;
ISweepingTrailManager* pSweepingTrailMgr;
ISweepingTrail* pSweepingTrail;
ISweepingTrail* pSweepingTrail_Wire;
ILightManager* pLightMgr;
IDirLightD*		pDirLight1;
IPointLightD*	pPointLight1;
ISpotLightD*	pSpotLight1;
IFontManager* pFontMgr;
IDynamicText* pMyText_fps;


Ut::ITimer timer(Ut::NOISE_TIMER_TIMEUNIT_MILLISECOND);
Ut::IInputEngine inputE;

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
	const UINT bufferWidth = 1280;
	const UINT bufferHeight = 800;

	//init fail
	if (!pRoot->Init())return FALSE;

	//query pointer to IScene
	pScene = pRoot->GetScenePtr();

	pMeshMgr = pScene->GetMeshMgr();
	pRenderer = pScene->CreateRenderer(bufferWidth, bufferHeight, hwnd);
	pCamera = pScene->GetCamera();
	pLightMgr = pScene->GetLightMgr();
	pMatMgr = pScene->GetMaterialMgr();
	pTexMgr = pScene->GetTextureMgr();
	pAtmos = pScene->GetAtmosphere();
	pGraphicObjMgr = pScene->GetGraphicObjMgr();
	pSweepingTrailMgr = pScene->GetSweepingTraillMgr();

	pTexMgr->CreateTextureFromFile("../media/earth.jpg", "Earth", true, 1024, 1024, false);
	pTexMgr->CreateTextureFromFile("../media/universe.jpg", "Universe", false, 256, 256, false);
	pTexMgr->CreateTextureFromFile("../media/blade.jpg", "bladeTrail", true, 512, 512, false);
	ITexture* pNormalMap = pTexMgr->CreateTextureFromFile("../media/earth-normal.png", "EarthNormalMap", FALSE, 512, 512, TRUE);

	//create font texture
	pFontMgr = pScene->GetFontMgr();
	pFontMgr->CreateFontFromFile("../media/calibri.ttf", "myFont", 24);
	pMyText_fps = pFontMgr->CreateDynamicTextA("myFont", "fpsLabel", "fps:000", 200, 100, NVECTOR4(0, 0, 0, 1.0f), 0, 0);
	pMyText_fps->SetTextColor(NVECTOR4(0, 0.3f, 1.0f, 0.5f));
	pMyText_fps->SetDiagonal(NVECTOR2(20, 20), NVECTOR2(300, 60));
	pMyText_fps->SetFont("myFont");
	pMyText_fps->SetBlendMode(NOISE_BLENDMODE_ALPHA);


	//----------------------------------------------------------

	pCamera->SetPosition(0, 30.0f, 0);
	pCamera->SetLookAt(0, 0, 0);
	pCamera->SetViewAngle_Radian(MATH_PI / 2.5f, 1.333333333f);
	pCamera->SetViewFrustumPlane(1.0f, 500.f);


	pModelLoader->LoadSkyDome(pAtmos, "Universe", 4.0f, 2.0f);
	pAtmos->SetFogEnabled(false);
	pAtmos->SetFogParameter(50.0f, 100.0f, NVECTOR3(0, 0, 1.0f));

	//---------------Light-----------------
	pDirLight1 = pLightMgr->CreateDynamicDirLight("myDirLight1");
	N_DirLightDesc dirLightDesc;
	dirLightDesc.ambientColor = NVECTOR3(0.1f, 0.1f, 0.1f);
	dirLightDesc.diffuseColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	dirLightDesc.specularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	dirLightDesc.direction = NVECTOR3(1.0f, -1.0f, 0);
	dirLightDesc.specularIntensity = 1.0f;
	dirLightDesc.diffuseIntensity = 1.0f;
	pDirLight1->SetDesc(dirLightDesc);

	//bottom right
	pGO_Axis = pGraphicObjMgr->CreateGraphicObj("Axis");
	pGO_TanList = pGraphicObjMgr->CreateGraphicObj("tanList");
	pGO_Axis->AddRectangle(NVECTOR2(1080.0f, 780.0f), NVECTOR2(1280.0f,800.0f), NVECTOR4(0.3f, 0.3f, 1.0f, 1.0f), "BottomRightTitle");
	pGO_Axis->SetBlendMode(NOISE_BLENDMODE_ALPHA);

	//*********************  sweeping trail  *************************
	pSweepingTrail = pSweepingTrailMgr->CreateSweepingTrail("myFX_Trail", 1000);
	pSweepingTrail_Wire = pSweepingTrailMgr->CreateSweepingTrail("myFX_Trail_Wire", 1000);
	pSweepingTrail->SetBlendMode(NOISE_BLENDMODE_ADDITIVE);
	pSweepingTrail->SetFillMode(NOISE_FILLMODE_SOLID);
	pSweepingTrail->SetHeaderCoolDownTimeThreshold(50.0f);
	pSweepingTrail->SetMaxLifeTimeOfLineSegment(200.0f);
	pSweepingTrail->SetHeader(N_LineSegment(NVECTOR3(0.0f, -10.0f, 0.0f), NVECTOR3(0.0, 10.0f, 0.0f)));
	pSweepingTrail->SetInterpolationStepCount(5);
	pSweepingTrail->SetCubicHermiteTangentScale(0.6f);
	pSweepingTrail->SetTextureName("bladeTrail");

	pSweepingTrail_Wire->SetBlendMode(NOISE_BLENDMODE_ALPHA);
	pSweepingTrail_Wire->SetFillMode(NOISE_FILLMODE_WIREFRAME);
	pSweepingTrail_Wire->SetHeaderCoolDownTimeThreshold(50.0f);
	pSweepingTrail_Wire->SetMaxLifeTimeOfLineSegment(200.0f);
	pSweepingTrail_Wire->SetHeader(N_LineSegment(NVECTOR3(0.0f, -10.0f, 0.0f), NVECTOR3(0.0, 10.0f, 0.0f)));
	pSweepingTrail_Wire->SetInterpolationStepCount(5);
	pSweepingTrail_Wire->SetCubicHermiteTangentScale(0.6f);
	return TRUE;
};

void MainLoop()
{
	static float incrNum = 0.0;
	incrNum += 0.2f;
	//if (incrNum > 10.0f)incrNum = 10.0f;
	::Sleep(100);

	InputProcess();
	pRenderer->ClearBackground(NVECTOR4(0.2f,0.2f,0.2f,1.0f));
	timer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS << "fps :" << timer.GetFPS() << "__vertex count:" << pSweepingTrail->GetLastDrawnVerticesCount();// << std::endl;
	pMyText_fps->SetTextAscii(tmpS.str());

	pSweepingTrail->SetHeader(N_LineSegment(NVECTOR3(10.0f*sinf(incrNum), 0, 10.0f*cosf(incrNum)), NVECTOR3(5.0f*sinf(incrNum), 0, 5.0f*cosf(incrNum))));
	pSweepingTrail->Update(10.0f);

	pSweepingTrail_Wire->SetHeader(N_LineSegment(NVECTOR3(10.0f*sinf(incrNum), 0, 10.0f*cosf(incrNum)), NVECTOR3(5.0f*sinf(incrNum), 0, 5.0f*cosf(incrNum))));
	pSweepingTrail_Wire->Update(10.0f);

	/*std::vector<N_LineSegment> vList;
	std::vector<std::pair<NVECTOR3, NVECTOR3>> tgList;
	pSweepingTrail->GetTangentList(tgList);
	pSweepingTrail->GetVerticesList(vList);
	for (uint32_t i=0;i<vList.size();++i)
	{
		pGO_TanList->AdjustElementCount(NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D, tgList.size()*2);
		pGO_TanList->SetLine3D(2 * i + 0,vList.at(i).vert1,vList.at(i).vert1 + tgList.at(i).first,NVECTOR4(0,0,1.0f,1.0f), NVECTOR4(0, 0, 1.0f, 1.0f));
		pGO_TanList->SetLine3D(2 * i + 1,vList.at(i).vert2, vList.at(i).vert2 + tgList.at(i).second, NVECTOR4(0, 1.0f, 1.0f, 1.0f), NVECTOR4(0, 1.0f, 1.0f, 1.0f));
	}
	*/

	//add to render list
	//for (auto& pMesh : meshList)pRenderer->AddToRenderQueue(pMesh);
	pRenderer->AddToRenderQueue(pGO_Axis);
	pRenderer->AddToRenderQueue(pGO_TanList);
	pRenderer->AddToRenderQueue(pMyText_fps);
	pRenderer->AddToRenderQueue(pSweepingTrail);
	pRenderer->AddToRenderQueue(pSweepingTrail_Wire);
	//pRenderer->SetActiveAtmosphere(pAtmos);
	static N_PostProcessGreyScaleDesc desc;
	desc.factorR = 0.3f;
	desc.factorG = 0.59f;
	desc.factorB = 0.11f;
	//pRenderer->AddToPostProcessList_GreyScale(desc);

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
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_SPACE))
	{
		pCamera->fps_MoveUp(0.02f* (float)timer.GetInterval());
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_LCONTROL))
	{
		pCamera->fps_MoveUp(-0.02f* (float)timer.GetInterval());
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
