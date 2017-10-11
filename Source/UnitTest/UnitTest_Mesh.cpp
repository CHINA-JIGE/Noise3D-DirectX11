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
	windowHWND = pRoot->CreateRenderWindow(960, 720, L"Hahaha Render Window", hInstance);

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
	const UINT bufferWidth = 1080;
	const UINT bufferHeight = 720;

	//³õÊ¼»¯Ê§°Ü
	if (!pRoot->InitD3D(hwnd))return FALSE;
	
	//query pointer to IScene
	pScene = pRoot->GetScenePtr();

	//retrieve meshMgr and Create new mesh
	pMeshMgr = pScene->GetMeshMgr();

	//use "myMesh1" string to initialize UID (unique-Identifier)
	//pMesh1= pMeshMgr->CreateMesh("myMesh1");


	pRenderer = pScene->CreateRenderer(bufferWidth,bufferHeight,TRUE);
	pCamera = pScene->GetCamera();
	pLightMgr=pScene->GetLightMgr();
	pMatMgr = pScene->GetMaterialMgr();
	pTexMgr = pScene->GetTextureMgr();
	pAtmos = pScene->GetAtmosphere();
	pGraphicObjMgr = pScene->GetGraphicObjMgr();


	//Âþ·´ÉäÌùÍ¼
	pTexMgr->CreateTextureFromFile("../media/Earth.jpg", "Earth", TRUE, 1024, 1024, FALSE);
	pTexMgr->CreateTextureFromFile("../media/Jade.jpg", "Jade", FALSE, 256, 256, FALSE);
	pTexMgr->CreateTextureFromFile("../media/universe2.jpg", "Universe", FALSE, 256, 256, FALSE);
	//pTexMgr->CreateTextureFromFile("../media/white.jpg", "Universe", FALSE, 128, 128, FALSE);
	pTexMgr->CreateTextureFromFile("../media/bottom-right-conner-title.jpg", "BottomRightTitle", TRUE, 0, 0, FALSE);
	pTexMgr->CreateCubeMapFromDDS("../media/UniverseEnv.dds", "AtmoTexture", NOISE_CUBEMAP_SIZE_256x256);
	ITexture* pNormalMap = pTexMgr->CreateTextureFromFile("../media/Earth.jpg", "EarthNormalMap", FALSE, 512, 512, TRUE);
	pNormalMap->ConvertTextureToGreyMap();
	pNormalMap->ConvertHeightMapToNormalMap(10.0f);


	//create font texture
	pFontMgr = pScene->GetFontMgr();
	pFontMgr->CreateFontFromFile("../media/STXINWEI.ttf", "myFont", 24);
	pMyText_fps= pFontMgr->CreateDynamicTextA("myFont","fpsLabel","fps:000", 200, 100, NVECTOR4(0, 0, 0, 1.0f), 0, 0);
	pMyText_fps->SetTextColor(NVECTOR4(0, 0.3f, 1.0f, 0.5f));
	pMyText_fps->SetDiagonal(NVECTOR2(20, 20), NVECTOR2(170, 60));
	pMyText_fps->SetFont("myFont");
	pMyText_fps->SetBlendMode(NOISE_BLENDMODE_ALPHA);

	
	//------------------MESH INITIALIZATION----------------

	//pModelLoader->LoadSphere(pMesh1,5.0f, 30, 30);
	//Mesh1.CreateBox(10.0f, 10.0f, 10.0f);
	//Mesh1.CreatePlane(50.0f, 50.0f);
	//pMesh1->CreateCylinder(20.0f, 30.0f,10,10);
	//Mesh1.SetPosition(0, 0, 0);
	//Mesh1.SetScale(0.2f, 0.2f, 0.2f);
	pModelLoader = pScene->GetModelLoader();
	N_SceneLoadingResult res;
	pModelLoader->LoadFile_FBX("../model/treeScene-fbx/geometries2.FBX", res);
	for (auto & name : res.meshNameList)
	{
		IMesh* pMesh = pMeshMgr->GetMesh(name);
		meshList.push_back(pMesh);
		pMesh->SetCullMode(NOISE_CULLMODE_BACK);
	}

	const std::vector<N_DefaultVertex>* pTmpVB;
	pTmpVB =	meshList.at(0)->GetVertexBuffer();
	pGraphicObjBuffer = pGraphicObjMgr->CreateGraphicObj("normalANDTangent");
	NVECTOR3 modelPos = meshList.at(3)->GetPosition();
	for (auto v : *pTmpVB)
	{
	pGraphicObjBuffer->AddLine3D(modelPos + v.Pos, modelPos+ v.Pos + 5.0f * v.Normal, NVECTOR4(1.0f, 0, 0, 1.0f), NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));//draw the normal
	pGraphicObjBuffer->AddLine3D(modelPos + v.Pos, modelPos + v.Pos + 5.0f* v.Tangent, NVECTOR4(0,0, 1.0f, 1.0f), NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));//draw the tangent
	}

	
	//----------------------------------------------------------

	pCamera->SetPosition(2.0f, 0, 0);
	pCamera->SetLookAt(0, 0, 0);
	pCamera->SetViewAngle(MATH_PI / 2.5f, 1.333333333f);
	pCamera->SetViewFrustumPlane(1.0f, 500.f);
	//use bounding box of mesh to init camera pos
	N_Box meshAABB = meshList.at(0)->ComputeBoundingBox();
	float rotateRadius = sqrtf(meshAABB.max.x*meshAABB.max.x + meshAABB.max.z*meshAABB.max.z)*1.2f;
	float rotateY = meshAABB.max.y*1.3f;
	pCamera->SetPosition(rotateRadius*0.7f, rotateY, rotateRadius*0.7f);
	pCamera->SetLookAt(0, 0, 0);

	pAtmos->SetFogEnabled(false);
	pAtmos->SetFogParameter(7.0f, 8.0f, NVECTOR3(0, 0, 1.0f));
	pAtmos->SetSkyDomeTexture("Universe");
	pModelLoader->LoadSkyDome(pAtmos, 4.0f, 4.0f);


	//¡ª¡ª¡ª¡ª¡ª¡ªµÆ¹â¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª
	pDirLight1 = pLightMgr->CreateDynamicDirLight("myDirLight1");
	N_DirLightDesc dirLightDesc;
	dirLightDesc.ambientColor = NVECTOR3(0.1f,0.1f, 0.1f);
	dirLightDesc.diffuseColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	dirLightDesc.specularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	dirLightDesc.direction = NVECTOR3(-1.0f,1.0f, 0);
	dirLightDesc.specularIntensity = 1.0f;
	dirLightDesc.diffuseIntensity =1.0f;
	pDirLight1->SetDesc(dirLightDesc);


	N_MaterialDesc Mat1;
	Mat1.ambientColor = NVECTOR3(0.1f, 0.1f, 0.1f);
	Mat1.diffuseColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	Mat1.specularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	Mat1.specularSmoothLevel = 40;
	Mat1.normalMapBumpIntensity = 0.2f;
	Mat1.environmentMapTransparency = 0.05f;
	Mat1.diffuseMapName = "Earth";//"Earth");
	Mat1.normalMapName ="EarthNormalMap";
	IMaterial* pMat= pMatMgr->CreateMaterial("meshMat1",Mat1);

	//set material
	//pMesh1->SetMaterial("meshMat1");

	//bottom right
	pGraphicObjBuffer->AddRectangle(NVECTOR2(960.0f, 680.0f), NVECTOR2(1080.0f, 720.0f), NVECTOR4(0.3f, 0.3f, 1.0f, 1.0f),"BottomRightTitle");
	pGraphicObjBuffer->SetBlendMode(NOISE_BLENDMODE_ADDITIVE);

	return TRUE;
};


void MainLoop()
{
	static float incrNum = 0.0;
	incrNum += 0.001f;
	pDirLight1->SetDirection(NVECTOR3(sin(incrNum),-1,cos(incrNum)));

	//GUIMgr.Update();
	InputProcess();
	pRenderer->ClearBackground();
	NTimer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS << "fps :" << NTimer.GetFPS() << std::endl;
	pMyText_fps->SetTextAscii(tmpS.str());


	//add to render list
	for (auto& pMesh : meshList)pRenderer->AddObjectToRenderList(pMesh);
	pRenderer->AddObjectToRenderList(pGraphicObjBuffer);
	pRenderer->AddObjectToRenderList(pAtmos);
	pRenderer->AddObjectToRenderList(pMyText_fps);

	//render
	pRenderer->RenderMeshes();
	pRenderer->RenderAtmosphere();
	pRenderer->RenderGraphicObjects();
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
