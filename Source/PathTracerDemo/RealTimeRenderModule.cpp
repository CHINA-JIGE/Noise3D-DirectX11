#include "Noise3D.h"
#include "RealTimeRenderModule.h"

RealTimeRenderModule::RealTimeRenderModule():
	mTimer(Ut::NOISE_TIMER_TIMEUNIT_MILLISECOND)
{

}

RealTimeRenderModule::~RealTimeRenderModule()
{

}

void RealTimeRenderModule::Init(HINSTANCE hInstance)
{
	//get Root interface
	m_pRoot = GetRoot();

	//create a window (using default window creation function)
	HWND windowHWND;
	windowHWND = m_pRoot->CreateRenderWindow(1080, 720, L"Hahaha Render Window", hInstance);

	//initialize input engine (detection for keyboard and mouse input)
	mInputE.Initialize(hInstance, windowHWND);

	//D3D and scene object init
	RealTimeRenderModule::_InitRealTime3D(windowHWND);

	//register main loop function
	m_pRoot->SetMainloopFunction((IMainloop*)this);

}

void RealTimeRenderModule::RunMainloop()
{
	//enter main loop
	m_pRoot->Mainloop();
}

void RealTimeRenderModule::_InitRealTime3D(HWND hwnd)
{
	if (!m_pRoot->Init())return;
	SceneLoader::Init(m_pRoot->GetSceneMgrPtr());//m_pScene will be init inside
	RealTimeRenderModule::_InitMgrsAndRenderers(hwnd);
	RealTimeRenderModule::_InitScene();
	RealTimeRenderModule::_InitAtmos();
	RealTimeRenderModule::_InitGraphicObjects();
	RealTimeRenderModule::_InitText();
	RealTimeRenderModule::_InitLight_RealTime();
}

void RealTimeRenderModule::_InitMgrsAndRenderers(HWND hwnd)
{
	//query pointer to IScene
	const UINT bufferWidth = 1080;
	const UINT bufferHeight = 720;
	m_pRenderer = m_pScene->CreateRenderer(bufferWidth, bufferHeight, hwnd);
	m_pCamera = m_pScene->GetCamera();
	m_pMatMgr = m_pScene->GetMaterialMgr();
	m_pTexMgr = m_pScene->GetTextureMgr();
	m_pAtmos = m_pScene->GetAtmosphere();
	m_pGraphicObjMgr = m_pScene->GetGraphicObjMgr();
	m_pShapeMgr = m_pScene->GetLogicalShapeMgr();
	m_pLightMgr = m_pScene->GetLightMgr();
}

void RealTimeRenderModule::_InitScene()
{
	//SceneLoader::LoadScene_DiffuseDemo(m_pCamera);
	//SceneLoader::LoadScene_RefractionDemo(m_pCamera);
	//SceneLoader::LoadScene_AreaLightingDemo(m_pCamera);
	//SceneLoader::LoadScene_StandardShader(m_pCamera);
	//SceneLoader::LoadScene_Mesh(m_pCamera);
	//SceneLoader::LoadScene_Porsche(m_pCamera);
	//SceneLoader::LoadScene_Buddha(m_pCamera);
	//SceneLoader::LoadScene_Ironman(m_pCamera);
	SceneLoader::LoadScene_IronmanAndAvenger(m_pCamera);
	//SceneLoader::LoadScene_IronmanCloseUp(m_pCamera);
}

void RealTimeRenderModule::_InitAtmos()
{
	m_pModelLoader->LoadSkyDome(m_pAtmos, "Universe", 4.0f, 2.0f);
	//pModelLoader->LoadSkyBox(pAtmos, "Universe", 1000.0f, 1000.0f, 1000.0f);
	m_pAtmos->SetFogEnabled(false);
	m_pAtmos->SetFogParameter(50.0f, 100.0f, Vec3(0, 0, 1.0f));
}

void RealTimeRenderModule::_InitGraphicObjects()
{
	m_pGraphicObjBuffer =  m_pGraphicObjMgr->CreateGraphicObject("GO");
	m_pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 50.0f,0,0 }, { 1.0f,0,0,1.0f }, { 1.0f,0,0,1.0f });
	m_pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 0,50.0f,0 }, { 0,1.0f,0,1.0f }, { 0,1.0f,0,1.0f });
	m_pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 0,0,50.0f }, { 0,0,1.0f,1.0f }, { 0,0,1.0f,1.0f });

	m_pGraphicObjBuffer->AddRectangle(Vec2(870.0f, 680.0f), Vec2(1080.0f, 720.0f), Vec4(0.3f, 0.3f, 1.0f, 1.0f), "BottomRightTitle");
	m_pGraphicObjBuffer->SetBlendMode(NOISE_BLENDMODE_ALPHA);
}

void RealTimeRenderModule::_InitText()
{
	//create font texture
	m_pTextMgr = m_pScene->GetTextMgr();
	m_pTextMgr->CreateFontFromFile("../media/calibri.ttf", "myFont", 24);
	m_pMyText_fps = m_pTextMgr->CreateDynamicTextA("myFont", "fpsLabel", "fps:000", 300, 100, Vec4(0, 0, 0, 1.0f), 0, 0);
	m_pMyText_fps->SetTextColor(Vec4(0, 0.3f, 1.0f, 0.5f));
	m_pMyText_fps->SetDiagonal(Vec2(20, 20), Vec2(170, 60));
	m_pMyText_fps->SetFont("myFont");
	m_pMyText_fps->SetBlendMode(NOISE_BLENDMODE_ALPHA);
}

void RealTimeRenderModule::_InitLight_RealTime()
{
	DirLight* pDirLight1 = m_pLightMgr->CreateDynamicDirLight(m_pScene->GetSceneGraph().GetRoot(), "myDirLight1");
	N_DirLightDesc dirLightDesc;
	dirLightDesc.ambientColor = Vec3(0.1f, 0.1f, 0.1f);
	dirLightDesc.diffuseColor = Vec3(1.0f, 1.0f, 1.0f);
	dirLightDesc.specularColor = Vec3(1.0f, 1.0f, 1.0f);
	dirLightDesc.direction = Vec3(1.0f, -0.5f, 0);
	dirLightDesc.specularIntensity = 1.0f;
	dirLightDesc.diffuseIntensity = 1.0f;
	pDirLight1->SetDesc(dirLightDesc);
}
