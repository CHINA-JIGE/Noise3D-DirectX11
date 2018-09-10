#include "Main3D.h"

using namespace Noise3D;

Main3DApp::Main3DApp():
	mTimer(Ut::NOISE_TIMER_TIMEUNIT_MILLISECOND)
{

}

Main3DApp::~Main3DApp()
{

}

bool Main3DApp::InitNoise3D(HWND hwnd, UINT canvasWidth, UINT canvasHeight)
{
	//initialize input engine (detection for keyboard and mouse input)
	//https://stackoverflow.com/questions/4547073/c-on-windows-using-directinput-without-a-window
	//it is said that an hInstance/hWnd is not necessary for a dinput device???
	//mInputE.Initialize(::GetModuleHandle(NULL), hwnd);

	m_pRoot = Noise3D::GetRoot();
	if (!m_pRoot->Init())return false;

	//query pointer to IScene
	m_pScene = m_pRoot->GetScenePtr();

	m_pMeshMgr = m_pScene->GetMeshMgr();
	m_pRenderer = m_pScene->CreateRenderer(canvasWidth, canvasHeight, hwnd);
	m_pCamera = m_pScene->GetCamera();
	m_pMatMgr = m_pScene->GetMaterialMgr();
	m_pTexMgr = m_pScene->GetTextureMgr();
	m_pGraphicObjMgr = m_pScene->GetGraphicObjMgr();

	m_pOriginTex = m_pTexMgr->CreateTextureFromFile("../media/chuyin.jpg", "Tex", true, 512, 512, true);
	m_pShTex = m_pTexMgr->CreatePureColorTexture("ShTex", 512, 512, NVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), true);

	//preprocess SH
	mFunction_SHPreprocess();

	//create font texture
	m_pFontMgr = m_pScene->GetFontMgr();
	m_pFontMgr->CreateFontFromFile("../media/calibri.ttf", "myFont", 24);
	m_pMyText_fps = m_pFontMgr->CreateDynamicTextA("myFont", "fpsLabel", "fps:000", 200, 100, NVECTOR4(0, 0, 0, 1.0f), 0, 0);
	m_pMyText_fps->SetTextColor(NVECTOR4(0, 0.3f, 1.0f, 0.5f));
	m_pMyText_fps->SetDiagonal(NVECTOR2(20, 20), NVECTOR2(300, 60));
	m_pMyText_fps->SetFont("myFont");
	m_pMyText_fps->SetBlendMode(NOISE_BLENDMODE_ALPHA);


	//----------------------------------------------------------
	m_pCamera->SetPosition(0, 100.0f, 0);
	m_pCamera->SetLookAt(0, 0, 0);
	m_pCamera->SetViewAngle_Radian(MATH_PI / 3.0f, 1.333333333f);
	m_pCamera->SetViewFrustumPlane(1.0f, 500.f);

	m_pGO_GUI = m_pGraphicObjMgr->CreateGraphicObj("tanList");
	m_pGO_GUI->SetBlendMode(NOISE_BLENDMODE_ALPHA);
	m_pGO_GUI->AddRectangle(NVECTOR2(50.0f, 50.0f), NVECTOR2(50.0f + 512.0f, 50.0f + 512.0f), NVECTOR4(1.0f, 0, 0, 1.0f), "Tex");
	m_pGO_GUI->AddRectangle(NVECTOR2(50.0f + 512.0f + 30.0f, 50.0f), NVECTOR2(50.0f + 512.0f + 30.0f + 512.0f, 50.0f + 512.0f), NVECTOR4(1.0f, 0, 0, 1.0f), "ShTex");

	return true;
}

void Main3DApp::UpdateFrame()
{
	//keyboard & mouse
	mFunction_InputProcess();

	//clear background
	m_pRenderer->ClearBackground(NVECTOR4(0.2f, 0.2f, 0.2f, 1.0f));
	mTimer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS << "fps :" << mTimer.GetFPS();
	m_pMyText_fps->SetTextAscii(tmpS.str());


	//add to render list
	//for (auto& pMesh : meshList)pRenderer->AddToRenderQueue(pMesh);
	m_pRenderer->AddToRenderQueue(m_pGO_Axis);
	m_pRenderer->AddToRenderQueue(m_pGO_GUI);
	m_pRenderer->AddToRenderQueue(m_pMyText_fps);

	//render
	m_pRenderer->Render();

	//present
	m_pRenderer->PresentToScreen();
}

void Main3DApp::Cleanup()
{
}

/*************************************************

								PRIVATE

*************************************************/

void Main3DApp::mFunction_InputProcess()
{
}

void Main3DApp::mFunction_SHPreprocess()
{
}
