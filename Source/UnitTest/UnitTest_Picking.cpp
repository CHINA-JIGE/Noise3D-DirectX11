

#include "Main3D.h"
#include <sstream>

using namespace Noise3D;

IRoot* pRoot;
IRenderer* pRenderer;
IScene* pScene;
ICamera* pCamera;
IAtmosphere* pAtmos;
IModelLoader* pModelLoader;
IMeshManager* pMeshMgr;
IMesh* pMesh1;
IMaterialManager*	pMatMgr;
ITextureManager*	pTexMgr;
ICollisionTestor*		pCT;
IGraphicObjectManager*	pGraphicObjMgr;
IGraphicObject*	pGraphicObjBuffer;
ILightManager* pLightMgr;
IDirLightD*		pDirLight1;
IPointLightD*	pPointLight1;
ISpotLightD*	pSpotLight1;
IFontManager* pFontMgr;
IDynamicText* pMyText_fps;


Ut::ITimer NTimer(NOISE_TIMER_TIMEUNIT_MILLISECOND);
Ut::IInputEngine inputE;

//Main Entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{

	//get Root interface
	pRoot = GetRoot();

	//create a window (using default window creation function)
	HWND windowHWND;
	windowHWND = pRoot->CreateRenderWindow(1280, 720, L"Hahaha Render Window", hInstance);

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
	const UINT bufferHeight = 720;

	//兜兵晒払移
	if (!pRoot->InitD3D(hwnd))return FALSE;

	//query pointer to IScene
	pScene = pRoot->GetScenePtr();

	//retrieve meshMgr and Create new mesh
	pMeshMgr = pScene->GetMeshMgr();

	//use "myMesh1" string to initialize UID (unique-Identifier)
	pMesh1 = pMeshMgr->CreateMesh("myMesh1");


	pRenderer = pScene->CreateRenderer(bufferWidth, bufferHeight, TRUE);
	pCamera = pScene->GetCamera();
	pLightMgr = pScene->GetLightMgr();
	pMatMgr = pScene->GetMaterialMgr();
	pTexMgr = pScene->GetTextureMgr();
	pAtmos = pScene->GetAtmosphere();
	pGraphicObjMgr = pScene->GetGraphicObjMgr();
	pCT = pScene->GetCollisionTestor();

	//create font texture
	pFontMgr = pScene->GetFontMgr();
	pFontMgr->CreateFontFromFile("media/STXINWEI.ttf", "myFont", 24);
	pMyText_fps = pFontMgr->CreateDynamicTextA("myFont", "fpsLabel", "fps:000", 200, 100, NVECTOR4(0, 0, 0, 1.0f), 0, 0);
	pMyText_fps->SetTextColor(NVECTOR4(0, 0.3f, 1.0f, 0.5f));
	pMyText_fps->SetDiagonal(NVECTOR2(20, 20), NVECTOR2(150, 60));
	pMyText_fps->SetFont("myFont");

	pRenderer->SetFillMode(NOISE_FILLMODE_SOLID);
	pRenderer->SetCullMode(NOISE_CULLMODE_NONE);//NOISE_CULLMODE_BACK

	//------------------MESH INITIALIZATION----------------
	pModelLoader->LoadSphere(pMesh1, 5.0f, 30, 30);


	const std::vector<N_DefaultVertex>* pTmpVB;
	pTmpVB = pMesh1->GetVertexBuffer();
	pGraphicObjBuffer = pGraphicObjMgr->CreateGraphicObj("normalANDTangent");
	/*for (auto v : tmpVB)
	{
	pGraphicObjBuffer->AddLine3D(v.Pos, v.Pos + 5.0f*v.Normal, NVECTOR4(1.0f, 0, 0, 1.0f), NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));//draw the normal
	pGraphicObjBuffer->AddLine3D(v.Pos, v.Pos + 5.0f*v.Tangent, NVECTOR4(0,0, 1.0f, 1.0f), NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));//draw the tangent
	}*/

	//----------------------------------------------------------

	pCamera->SetPosition(2.0f, 0, 0);
	pCamera->SetLookAt(0, 0, 0);
	pCamera->SetViewAngle(MATH_PI / 2.5f, 1.333333333f);
	pCamera->SetViewFrustumPlane(1.0f, 500.f);
	//use bounding box of mesh to init camera pos
	N_Box meshAABB = pMesh1->ComputeBoundingBox();
	float rotateRadius = sqrtf(meshAABB.max.x*meshAABB.max.x + meshAABB.max.z*meshAABB.max.z)*1.2f;
	float rotateY = meshAABB.max.y*1.3f;
	pCamera->SetPosition(rotateRadius*0.7f, rotateY, rotateRadius*0.7f);
	pCamera->SetLookAt(0, 0, 0);

	//！！！！！！菊高！！！！！！！！
	pDirLight1 = pLightMgr->CreateDynamicDirLight("myDirLight1");
	N_DirLightDesc dirLightDesc;
	dirLightDesc.mAmbientColor = NVECTOR3(0.0f, 0.0f, 0.0f);
	dirLightDesc.mDiffuseColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	dirLightDesc.mSpecularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	dirLightDesc.mDirection = NVECTOR3(-1.0f, 1.0f, 0);
	dirLightDesc.mSpecularIntensity = 1.0f;
	dirLightDesc.mDiffuseIntensity = 1.0f;
	pDirLight1->SetDesc(dirLightDesc);


	N_MaterialDesc Mat1;
	Mat1.mBaseAmbientColor = NVECTOR3(0.1f, 0.1f, 0.1f);
	Mat1.mBaseDiffuseColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	Mat1.mBaseSpecularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	Mat1.mSpecularSmoothLevel = 40;
	Mat1.mNormalMapBumpIntensity = 0.2f;
	Mat1.mEnvironmentMapTransparency = 0.05f;
	IMaterial* pMat = pMatMgr->CreateMaterial("meshMat1", Mat1);

	//set material
	pMesh1->SetMaterial("meshMat1");

	return TRUE;
};


void MainLoop()
{
	static float incrNum = 0.0;
	incrNum += 0.001f;
	pDirLight1->SetDirection(NVECTOR3(sin(incrNum), -1, cos(incrNum)));

	//GUIMgr.Update();
	InputProcess();
	pRenderer->ClearBackground();
	NTimer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS << "fps :" << NTimer.GetFPS() << std::endl;
	pMyText_fps->SetTextAscii(tmpS.str());

	//picking : collision test
	std::vector<NVECTOR3> colResult;
	pCT->Picking(pMesh1, { 0.0f,0.0f },colResult);
	if(colResult.size()>0)ERROR_MSG("BENJOE!!");

	//add to render list
	pRenderer->AddObjectToRenderList(pMesh1);
	pRenderer->AddObjectToRenderList(pGraphicObjBuffer);
	pRenderer->AddObjectToRenderList(pMyText_fps);

	//render
	pRenderer->SetBlendingMode(NOISE_BLENDMODE_OPAQUE);
	pRenderer->RenderMeshes();
	pRenderer->SetBlendingMode(NOISE_BLENDMODE_OPAQUE);
	pRenderer->RenderAtmosphere();
	pRenderer->SetBlendingMode(NOISE_BLENDMODE_ADDITIVE);
	pRenderer->RenderGraphicObjects();
	pRenderer->SetBlendingMode(NOISE_BLENDMODE_ALPHA);
	pRenderer->RenderTexts();

	//present
	pRenderer->PresentToScreen();
};

void InputProcess()
{
	inputE.Update();

	if (inputE.IsKeyPressed(Ut::NOISE_KEY_A))
	{
		pCamera->fps_MoveRight(-0.1f, FALSE);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_D))
	{
		pCamera->fps_MoveRight(0.1f, FALSE);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_W))
	{
		pCamera->fps_MoveForward(0.1f, FALSE);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_S))
	{
		pCamera->fps_MoveForward(-0.1f, FALSE);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_SPACE))
	{
		pCamera->fps_MoveUp(0.1f);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_LCONTROL))
	{
		pCamera->fps_MoveUp(-0.1f);
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
