#include "Noise3D.h"
#include "MainApp.h"

MainApp::MainApp():
	mTimer(Ut::NOISE_TIMER_TIMEUNIT_MILLISECOND),
	mMainloopState(MAINLOOP_STATE::PREVIEW)
{

}

MainApp::~MainApp()
{
	m_pRoot->ReleaseAll();
}

void MainApp::Init(HINSTANCE hInstance)
{
	//get Root interface
	m_pRoot = GetRoot();

	//create a window (using default window creation function)
	HWND windowHWND;
	windowHWND = m_pRoot->CreateRenderWindow(960, 720, L"Hahaha Render Window", hInstance);

	//initialize input engine (detection for keyboard and mouse input)
	mInputE.Initialize(hInstance, windowHWND);

	//D3D and scene object init
	MainApp::InitRealTime3D(windowHWND);

	//register main loop function
	m_pRoot->SetMainLoopFunction(Mainloop);

}

void MainApp::RunMainloop()
{
	//enter main loop
	m_pRoot->Mainloop();
}

void MainApp::InitRealTime3D(HWND hwnd)
{
	if (!m_pRoot->Init())return;
	MainApp::InitMgrsAndRenderers(hwnd);
	MainApp::InitTextures();
	MainApp::InitMaterials();
	MainApp::InitCamera();
	MainApp::InitSceneNodes();
	MainApp::InitSceneObjects();
	MainApp::InitGraphicObjects();
	MainApp::InitText();
	MainApp::InitLight_RealTime();
}

void MainApp::InitMgrsAndRenderers(HWND hwnd)
{
	//query pointer to IScene
	m_pScene = m_pRoot->GetSceneMgrPtr();
	m_pMeshMgr = m_pScene->GetMeshMgr();

	const UINT bufferWidth = 1080;
	const UINT bufferHeight = 720;
	m_pRenderer = m_pScene->CreateRenderer(bufferWidth, bufferHeight, hwnd);
	m_pCamera = m_pScene->GetCamera();
	m_pMatMgr = m_pScene->GetMaterialMgr();
	m_pTexMgr = m_pScene->GetTextureMgr();
	m_pAtmos = m_pScene->GetAtmosphere();
	m_pGraphicObjMgr = m_pScene->GetGraphicObjMgr();
	m_pShapeMgr = m_pScene->GetLogicalShapeMgr();
}

void MainApp::InitTextures()
{
	//m_pTexMgr->CreateTextureFromFile("../media/earth.jpg", "Earth", TRUE, 1024, 1024, FALSE);
	m_pTexMgr->CreateTextureFromFile("../media/white.jpg", "Universe", FALSE, 256, 256, FALSE);
	//pTexMgr->CreateCubeMapFromDDS("../media/CubeMap/cube-room.dds", "Universe", FALSE);
	m_pTexMgr->CreateTextureFromFile("../media/noise3d.png", "BottomRightTitle", TRUE, 0, 0, FALSE);
	
	//ITexture* pNormalMap = m_pTexMgr->CreateTextureFromFile("../media/earth-normal.png", "EarthNormalMap", FALSE, 512, 512, TRUE);
	//pNormalMap->ConvertTextureToGreyMap();
	//pNormalMap->ConvertHeightMapToNormalMap(10.0f);
}

void MainApp::InitMaterials()
{
	N_MaterialDesc Mat1;
	Mat1.ambientColor = Vec3(0, 0, 0);
	Mat1.diffuseColor = Vec3(1.0f, 1.0f, 1.0f);
	Mat1.specularColor = Vec3(1.0f, 1.0f, 1.0f);
	Mat1.specularSmoothLevel = 40;
	Mat1.normalMapBumpIntensity = 0.2f;
	Mat1.environmentMapTransparency = 0.1f;
	Mat1.diffuseMapName = "Earth";//"Earth");
	//Mat1.normalMapName = "EarthNormalMap";
	//Mat1.environmentMapName = "AtmoTexture";
	m_pMatMgr->CreateMaterial("mat1", Mat1);
}

void MainApp::InitCamera()
{
	m_pCamera->SetViewAngle_Radian(Ut::PI / 2.5f, 1.333333333f);
	m_pCamera->SetViewFrustumPlane(1.0f, 500.f);
	m_pCamera->GetWorldTransform().SetPosition(0, 0, 200.0f);
	m_pCamera->LookAt(0, 0, 0);
}

void MainApp::InitAtmos()
{
	m_pModelLoader->LoadSkyDome(m_pAtmos, "Universe", 4.0f, 2.0f);
	//pModelLoader->LoadSkyBox(pAtmos, "Universe", 1000.0f, 1000.0f, 1000.0f);
	m_pAtmos->SetFogEnabled(false);
	m_pAtmos->SetFogParameter(50.0f, 100.0f, Vec3(0, 0, 1.0f));
}

void MainApp::InitSceneNodes()
{
	SceneGraph& sg = m_pScene->GetSceneGraph();

	m_pSnode_Sphere = sg.GetRoot()->CreateChildNode();
	m_pSnode_Sphere->GetLocalTransform().SetPosition(Vec3(0, 0, 0));

	m_pSnode_Box = sg.GetRoot()->CreateChildNode();
	m_pSnode_Box->GetLocalTransform().SetPosition(Vec3(20.0f, 0, 0));
}

void MainApp::InitSceneObjects()
{

	Mesh* pMeshSphere = m_pMeshMgr->CreateMesh(m_pSnode_Sphere, "sphere0");
	m_pModelLoader->LoadSphere(pMeshSphere, 10.0f, 10, 10);
	pMeshSphere->SetCollidable(false);

	Mesh* pMeshBox = m_pMeshMgr->CreateMesh(m_pSnode_Box, "box0");
	m_pModelLoader->LoadBox(pMeshBox, 10.0f, 10.0f, 10.0f);
	pMeshBox->SetCollidable(false);


	//add to list
	mMeshList.push_back(pMeshBox);
	mMeshList.push_back(pMeshSphere);

}

void MainApp::InitGraphicObjects()
{
	m_pGraphicObjBuffer =  m_pGraphicObjMgr->CreateGraphicObject("GO");
	m_pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 50.0f,0,0 }, { 1.0f,0,0,1.0f }, { 1.0f,0,0,1.0f });
	m_pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 0,50.0f,0 }, { 0,1.0f,0,1.0f }, { 0,1.0f,0,1.0f });
	m_pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 0,0,50.0f }, { 0,0,1.0f,1.0f }, { 0,0,1.0f,1.0f });

	m_pGraphicObjBuffer->AddRectangle(Vec2(870.0f, 680.0f), Vec2(1080.0f, 720.0f), Vec4(0.3f, 0.3f, 1.0f, 1.0f), "BottomRightTitle");
	m_pGraphicObjBuffer->SetBlendMode(NOISE_BLENDMODE_ALPHA);
}

void MainApp::InitText()
{
	//create font texture
	m_pTextMgr = m_pScene->GetTextMgr();
	m_pTextMgr->CreateFontFromFile("../media/calibri.ttf", "myFont", 24);
	m_pMyText_fps = m_pTextMgr->CreateDynamicTextA("myFont", "fpsLabel", "fps:000", 200, 100, Vec4(0, 0, 0, 1.0f), 0, 0);
	m_pMyText_fps->SetTextColor(Vec4(0, 0.3f, 1.0f, 0.5f));
	m_pMyText_fps->SetDiagonal(Vec2(20, 20), Vec2(170, 60));
	m_pMyText_fps->SetFont("myFont");
	m_pMyText_fps->SetBlendMode(NOISE_BLENDMODE_ALPHA);
}

void MainApp::InitLight_RealTime()
{
	DirLight* pDirLight1 = m_pLightMgr->CreateDynamicDirLight(m_pScene->GetSceneGraph().GetRoot(), "myDirLight1");
	N_DirLightDesc dirLightDesc;
	dirLightDesc.ambientColor = Vec3(0.1f, 0.1f, 0.1f);
	dirLightDesc.diffuseColor = Vec3(1.0f, 1.0f, 1.0f);
	dirLightDesc.specularColor = Vec3(1.0f, 1.0f, 1.0f);
	dirLightDesc.direction = Vec3(1.0f, -1.0f, 0);
	dirLightDesc.specularIntensity = 1.0f;
	dirLightDesc.diffuseIntensity = 1.0f;
	pDirLight1->SetDesc(dirLightDesc);
}

void MainApp::InitPathTracingSceneObject()
{
	LogicalSphere* pSphere = m_pShapeMgr->CreateSphere(m_pSnode_Sphere, "logicSPH0");
	pSphere->SetCollidable(true);
	pSphere->SetRadius(10.0f);

	LogicalBox* pBox = m_pShapeMgr->CreateBox(m_pSnode_Box, "logicBox0");
	pBox->SetCollidable(true);
	pBox->SetSizeXYZ(Vec3(10.0f, 10.0f, 10.0f));

	mShapeList.push_back(pSphere);
	mShapeList.push_back(pBox);
}

void MainApp::Mainloop()
{
	//switch(xxx)
	Mainloop_RealTimePreview();
}

void MainApp::Mainloop_RealTimePreview()
{
	InputProcess();
	m_pRenderer->ClearBackground();
	mTimer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS << "fps :" << mTimer.GetFPS();// << std::endl;
	m_pMyText_fps->SetTextAscii(tmpS.str());

	//add to render list
	for (auto& pMesh : mMeshList)m_pRenderer->AddToRenderQueue(pMesh);
	m_pRenderer->AddToRenderQueue(m_pGraphicObjBuffer);
	m_pRenderer->AddToRenderQueue(m_pMyText_fps);
	m_pRenderer->SetActiveAtmosphere(m_pAtmos);

	//render
	m_pRenderer->Render();

	//present
	m_pRenderer->PresentToScreen();
}

void MainApp::InputProcess()
{
	mInputE.Update();

	if (mInputE.IsKeyPressed(Ut::NOISE_KEY_A))
	{
		m_pCamera->fps_MoveRight(-0.1f, FALSE);
	}
	if (mInputE.IsKeyPressed(Ut::NOISE_KEY_D))
	{
		m_pCamera->fps_MoveRight(0.1f, FALSE);
	}
	if (mInputE.IsKeyPressed(Ut::NOISE_KEY_W))
	{
		m_pCamera->fps_MoveForward(0.1f, FALSE);
	}
	if (mInputE.IsKeyPressed(Ut::NOISE_KEY_S))
	{
		m_pCamera->fps_MoveForward(-0.1f, FALSE);
	}
	if (mInputE.IsKeyPressed(Ut::NOISE_KEY_SPACE))
	{
		m_pCamera->fps_MoveUp(0.1f);
	}
	if (mInputE.IsKeyPressed(Ut::NOISE_KEY_LCONTROL))
	{
		m_pCamera->fps_MoveUp(-0.1f);
	}

	if (mInputE.IsMouseButtonPressed(Ut::NOISE_MOUSEBUTTON_LEFT))
	{
		Vec3 euler = m_pCamera->GetWorldTransform().GetEulerAngleZXY();
		euler += Vec3((float)mInputE.GetMouseDiffY() / 200.0f, (float)mInputE.GetMouseDiffX() / 200.0f, 0);
		euler.x = Ut::Clamp(euler.x, -Ut::PI / 2.0f + 0.001f, Ut::PI / 2.0f - 0.001f);
		m_pCamera->GetWorldTransform().SetRotation(euler.x, euler.y, euler.z);
	}

	//quit main loop and terminate program
	if (mInputE.IsKeyPressed(Ut::NOISE_KEY_ESCAPE))
	{
		m_pRoot->SetMainLoopStatus(NOISE_MAINLOOP_STATUS_QUIT_LOOP);
	}

}
