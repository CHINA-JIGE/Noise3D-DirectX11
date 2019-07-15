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
IMesh* pScreenDescriptor;
IMaterialManager*	pMatMgr;
ITextureManager*	pTexMgr;
IGraphicObjectManager*	pGraphicObjMgr;
IGraphicObject*	pGraphicObjBuffer;
ILightManager* pLightMgr;
IDirLightD*		pDirLight1;
IPointLightD*	pPointLight1;
IPointLightD*	pPointLight2;
ISpotLightD*	pSpotLight1;
IFontManager* pFontMgr;
IDynamicText* pMyText_fps;

N_PostProcessGreyScaleDesc greyScaleDesc;
N_PostProcesQwertyDistortionDesc qwertyDesc;

Ut::ITimer NTimer(Ut::NOISE_TIMER_TIMEUNIT_MILLISECOND);
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
	pRoot->SetMainloopFunction(MainLoop);

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

	//兜兵晒払移
	if (!pRoot->Init())return FALSE;

	//query pointer to IScene
	pScene = pRoot->GetScenePtr();

	//retrieve meshMgr and Create new mesh
	pMeshMgr = pScene->GetMeshMgr();

	//use "myMesh1" string to initialize UID (unique-Identifier)
	//pMesh1= pMeshMgr->CreateMesh("myMesh1");

	pRenderer = pScene->CreateRenderer(bufferWidth, bufferHeight, hwnd);
	//pRenderer->SwitchToFullScreenMode();
	pCamera = pScene->GetCamera();
	pLightMgr = pScene->GetLightMgr();
	pMatMgr = pScene->GetMaterialMgr();
	pTexMgr = pScene->GetTextureMgr();
	pAtmos = pScene->GetAtmosphere();
	pGraphicObjMgr = pScene->GetGraphicObjMgr();

	//diffuse map
	pTexMgr->CreateTextureFromFile("../../../media/Earth.jpg", "Earth", true, 1024, 1024, false);
	//pTexMgr->CreateTextureFromFile("../../../media/jade.jpg", "Jade", FALSE, 256, 256, FALSE);
	pTexMgr->CreateTextureFromFile("../../../model/qwerty/arc18_filtered.PNG", "brick", false, 256, 256, false);
	//pTexMgr->CreateTextureFromFile("../../../model/qwerty/xxxCompleteMap.tga", "brick", FALSE, 512, 512, FALSE);

	pTexMgr->CreateTextureFromFile("../../../media/sky.jpg", "Universe", false, 256, 256, false);
	//pTexMgr->CreateTextureFromFile("../../../media/white.jpg", "Universe", FALSE, 128, 128, FALSE);
	pTexMgr->CreateTextureFromFile("../../../media/bottom-right-conner-title.jpg", "BottomRightTitle", true, 0, 0, false);
	pTexMgr->CreateCubeMapFromDDS("../../../media/UniverseEnv.dds", "AtmoTexture");
	//ITexture* pNormalMap = pTexMgr->CreateTextureFromFile("../../../media/Earth.jpg", "EarthNormalMap", FALSE, 512, 512, TRUE);
	//pNormalMap->ConvertTextureToGreyMap();
	//pNormalMap->ConvertHeightMapToNormalMap(0.03f);
	//pNormalMap->SaveTexture2DToFile("hahahNormal.tga", NOISE_IMAGE_FILE_FORMAT_TGA);

	//create font texture
	pFontMgr = pScene->GetFontMgr();
	pFontMgr->CreateFontFromFile("../../../media/STXINWEI.ttf", "myFont", 24);
	pMyText_fps = pFontMgr->CreateDynamicTextA("myFont", "fpsLabel", "fps:000", 200, 100, Vec4(0, 0, 0, 1.0f), 0, 0);
	pMyText_fps->SetTextColor(Vec4(0, 0.3f, 1.0f, 0.5f));
	pMyText_fps->SetDiagonal(Vec2(20, 20), Vec2(170, 60));
	pMyText_fps->SetFont("myFont");
	pMyText_fps->SetBlendMode(NOISE_BLENDMODE_ALPHA);

	//---------------------INIT QWERTY----------------
	pScreenDescriptor = pMeshMgr->CreateMesh("screen");
	//pModelLoader->LoadFile_OBJ(pScreenDescriptor,"../../../model/qwerty/AsusZenbook-Screen-XY.obj");
	//meshList.push_back(pScreenDescriptor);
	pScreenDescriptor->SetPosition(0, 0, 0);
	pScreenDescriptor->SetCullMode(NOISE_CULLMODE_NONE);
	pScreenDescriptor->SetShadeMode(NOISE_SHADEMODE_PHONG);

	//------------------MESH INITIALIZATION----------------
	pModelLoader = pScene->GetModelLoader();
	N_SceneLoadingResult res;
	pModelLoader->LoadFile_FBX("../../../model/qwerty/testRoom-177x100.FBX", res);
	//pModelLoader->LoadFile_FBX("../../../model/treeScene/treeScene.FBX", res);
	//IMesh* pMesh = pMeshMgr->CreateMesh("testModel");
	//pModelLoader->LoadFile_OBJ(pMesh, "../../../model/qwerty/qwertyScene1.obj");
	//pMesh->SetPosition(0, 0, 0);
	//pMesh->SetCullMode(NOISE_CULLMODE_BACK);
	//pMesh->SetShadeMode(NOISE_SHADEMODE_PHONG);
	//meshList.push_back(pMesh);
	//meshList.push_back(pScreenDescriptor);
	for (auto & name : res.meshNameList)
	{
		IMesh* pMesh = pMeshMgr->GetMesh(name);
		meshList.push_back(pMesh);
		pMesh->SetCullMode(NOISE_CULLMODE_BACK);
		pMesh->SetShadeMode(NOISE_SHADEMODE_PHONG);
	}


	pGraphicObjBuffer = pGraphicObjMgr->CreateGraphicObj("normalANDTangent");
	for (auto mesh : meshList)
	{
		const std::vector<N_DefaultVertex>* pTmpVB;
		pTmpVB = mesh->GetVertexBuffer();
		Vec3 modelPos = mesh->GetPosition();
		for (auto v : *pTmpVB)
		{
			//pGraphicObjBuffer->AddLine3D(modelPos + v.Pos, modelPos + v.Pos + 10.0f * v.Normal, Vec4(1.0f, 0, 0, 1.0f), Vec4(0, 0, 0, 1.0f));//draw the normal
			//pGraphicObjBuffer->AddLine3D(modelPos + v.Pos, modelPos + v.Pos + 10.0f* v.Tangent, Vec4(0, 0, 1.0f, 1.0f), Vec4(1.0f, 1.0f, 1.0f, 1.0f));//draw the tangent
		}
	}
	
	pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 50.0f,0,0 }, { 1.0f,0,0,1.0f }, { 1.0f,0,0,1.0f });
	pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 0,50.0f,0 }, { 0,1.0f,0,1.0f }, { 0,1.0f,0,1.0f });
	pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 0,0,50.0f }, { 0,0,1.0f,1.0f }, { 0,0,1.0f,1.0f });

	//----------------------------------------------------------

	pCamera->SetViewAngle_Radian(MATH_PI *0.4f, float(1.5f));
	pCamera->SetViewFrustumPlane(1.0f, 3000.f);
	pCamera->SetPosition(170.0f,10.0f, -170.0f);
	pCamera->SetLookAt(0, 0, 0);


	pModelLoader->LoadSkyDome(pAtmos, "Universe", 4.0f, 2.0f);
	pAtmos->SetFogEnabled(false);
	pAtmos->SetFogParameter(50.0f, 100.0f, Vec3(0, 0, 1.0f));

	//！！！！！！菊高！！！！！！！！
	/*pDirLight1 = pLightMgr->CreateDynamicDirLight("myDirLight1");
	N_DirLightDesc dirLightDesc;
	dirLightDesc.ambientColor = Vec3(0.1f, 0.1f, 0.1f);
	dirLightDesc.diffuseColor = Vec3(1.0f, 1.0f, 1.0f);
	dirLightDesc.specularColor = Vec3(1.0f, 1.0f, 1.0f);
	dirLightDesc.direction = Vec3(-1.0f, -1.0f, 1.0f);
	dirLightDesc.specularIntensity = 0.5f;
	dirLightDesc.diffuseIntensity = 1.0f;
	pDirLight1->SetDesc(dirLightDesc);*/

	pPointLight1 = pLightMgr->CreateDynamicPointLight("myPointLight1");
	N_PointLightDesc pointLightDesc;
	pointLightDesc.ambientColor = Vec3(1.0, 1.0, 1.0);
	pointLightDesc.diffuseColor = Vec3(0, 0, 0);
	pointLightDesc.specularColor = Vec3(0, 0, 0);
	pointLightDesc.mAttenuationFactor = 0.0001f;
	pointLightDesc.mLightingRange = 1000.0f;
	pointLightDesc.mPosition = Vec3(0, 0, -40.0f);
	pointLightDesc.specularIntensity = 1.0f;
	pointLightDesc.diffuseIntensity = 1.0f;
	pPointLight1->SetDesc(pointLightDesc);


	N_MaterialDesc Mat1;
	Mat1.ambientColor = Vec3(1.0f, 1.0f, 1.0f);
	Mat1.diffuseColor = Vec3(1.0f, 1.0f, 1.0f);
	Mat1.specularColor = Vec3(1.0f, 1.0f, 1.0f);
	Mat1.specularSmoothLevel = 100;
	Mat1.normalMapBumpIntensity = 0.5f;
	Mat1.environmentMapTransparency = 0.1f;
	Mat1.diffuseMapName = "brick";//"Earth");
	//Mat1.normalMapName = "EarthNormalMap";
	//Mat1.environmentMapName = "AtmoTexture";
	IMaterial* pMat = pMatMgr->CreateMaterial("meshMat1", Mat1);

	//set material
	//pMesh->SetMaterial("meshMat1");
	for (auto mesh : meshList)
	{
		//mesh->SetMaterial("meshMat1");
	}


	//bottom right
	pGraphicObjBuffer->AddRectangle(Vec2(1100.0f, 750.0f), Vec2(1280.0f, 800.0f), Vec4(0.3f, 0.3f, 1.0f, 1.0f), "BottomRightTitle");
	pGraphicObjBuffer->SetBlendMode(NOISE_BLENDMODE_ALPHA);

	pGraphicObjBuffer->AddPoint3D({ 1.0f,1.0f,1.0f });
	pGraphicObjBuffer->AddPoint3D({ 2.0f,1.0f,1.0f });
	pGraphicObjBuffer->AddPoint3D({ 3.0f,1.0f,1.0f });
	pGraphicObjBuffer->AddPoint3D({ 4.0f,1.0f,1.0f });


	//post process description init
	greyScaleDesc.factorR = 0.3f;
	greyScaleDesc.factorG = 0.59f;
	greyScaleDesc.factorB = 0.1f;

	qwertyDesc.pCamera = pCamera;
	qwertyDesc.pScreenDescriptor = pScreenDescriptor;

	return TRUE;
};


void MainLoop()
{
	//static double incrNum = 0;
	//incrNum += 0.0001;
	//pCamera->SetPosition(450.0f, 0, 500.0f * cos(incrNum));
	InputProcess();
	pRenderer->ClearBackground();
	NTimer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS << "fps :" << NTimer.GetFPS() << std::endl;
	pMyText_fps->SetTextAscii(tmpS.str());


	//add to render list
	for (auto& pMesh : meshList)pRenderer->AddToRenderQueue(pMesh);
	pRenderer->AddToRenderQueue(pGraphicObjBuffer);
	//pRenderer->AddToRenderQueue(pMyText_fps);
	pRenderer->SetActiveAtmosphere(pAtmos);

	//pRenderer->AddToPostProcessList_QwertyDistortion(qwertyDesc);

	//render
	pRenderer->Render();

	//present
	pRenderer->PresentToScreen();
};

void InputProcess()
{
	inputE.Update();

	static float t = 0.0f;
	t += 0.0006f;
	pCamera->SetPosition(300.0f * sinf(t), 0, -300.0f);
	pCamera->OptimizeForQwertyPass1(pScreenDescriptor);

	if (inputE.IsKeyPressed(Ut::NOISE_KEY_A))
	{
		pCamera->fps_MoveRight(-0.5f, FALSE);
		//pCamera->SetPosition(-500.0f, 0, -700.0f);
		//pCamera->OptimizeForQwertyPass1(pScreenDescriptor);

		//pCamera->SetPosition(-21.4182, 68.8494, -332.617);
		//pCamera->SetRotation(0.180487, 6.26345, -0.521821);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_D))
	{
		pCamera->fps_MoveRight(0.5f, FALSE);
		//pCamera->SetPosition(300.0f, 0, -200.0f);
		//pCamera->OptimizeForQwertyPass1(pScreenDescriptor);
		

		//pCamera->SetPosition(2.86963, 137.022, -227.278);
		//pCamera->SetRotation(0.692715, 0.00986241, 0.0255206);

	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_W))
	{
		pCamera->fps_MoveForward(0.5f, FALSE);
		//pCamera->SetPosition(0, 100.0f, -200.0f);
		//pCamera->OptimizeForQwertyPass1(pScreenDescriptor);

		//pCamera->SetPosition(-203.681, 13.3611, -172.219);
		//pCamera->SetLookAt(0.122438, 0.867768, -0.0174778);

	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_S))
	{
		pCamera->fps_MoveForward(-0.5f, FALSE);
		//pCamera->OptimizeForQwertyPass1(pScreenDescriptor);

		//pCamera->SetPosition(170.0f, -100.0f, 0);
		//pCamera->SetLookAt(0, 0, 0);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_SPACE))
	{
		pCamera->fps_MoveUp(0.5f);
		//pCamera->OptimizeForQwertyPass1(pScreenDescriptor);
		
		//pCamera->SetPosition(170.0f, 0, 0);
		//pCamera->SetLookAt(0, 0, 0);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_LCONTROL))
	{
		pCamera->fps_MoveUp(-0.5f);
		//pCamera->OptimizeForQwertyPass1(pScreenDescriptor);
		
		//pCamera->SetPosition(300.0f, 0, 0);
		//pCamera->SetLookAt(0, 0, 0);
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
