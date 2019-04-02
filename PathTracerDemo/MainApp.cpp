#include "Noise3D.h"
#include "RealTimeRenderModule.h"
#include "MainApp.h"

using namespace Noise3D;

MainApp::MainApp():
	mTotalPathTracerRenderTime(0.0f),
	c_PathTracerBufferWidth(480),
	c_PathTracerBufferHeight(320),
	mMainloopState(MAINLOOP_STATE::PREVIEW),
	m_pPathTracer(nullptr),
	m_pGraphicObj_ResultPreview(nullptr),
	m_pPathTracerRenderTarget(nullptr),
	mIsPathTracerRenderFinished(false)
{
}

MainApp::~MainApp()
{
}

void MainApp::PathTracerStartRender()
{
	mTotalPathTracerRenderTime = 0.0f;
	mIsPathTracerRenderFinished = false;
	mTimer.ResetAll();
	mTimer.NextTick();
	m_pPathTracer->Render(m_pScene->GetSceneGraph().GetRoot(), &mPathTracerShader_Minimal);
	mIsPathTracerRenderFinished = true;
	//update PathTracer render target data to GPU
	m_pPathTracerRenderTarget->UpdateToVideoMemory();
}

void MainApp::Init_GI()
{
	MainApp::_InitPathTracer();
	MainApp::_InitGraphicsObjectOfPreviewRender();
	MainApp::_InitPathTracingSceneObject();
	MainApp::_InitAreaLight();
}


/**************************************

					PROTECTED

*****************************************/

void MainApp::_InitPathTracer()
{
	m_pPathTracer = m_pScene->CreatePathTracer(c_PathTracerBufferWidth, c_PathTracerBufferHeight);
	m_pPathTracerRenderTarget = m_pPathTracer->GetRenderTarget();
}

void MainApp::_InitPathTracingSceneObject()
{
	LogicalSphere* pSphere = m_pShapeMgr->CreateSphere(m_pSnode_Sphere, "logicSPH0");
	pSphere->SetCollidable(true);
	pSphere->SetRadius(10.0f);
	m_pSnode_Sphere->GetLocalTransform().SetPosition(0,0,0);

	LogicalBox* pBox = m_pShapeMgr->CreateBox(m_pSnode_Box, "logicBox0");
	pBox->SetCollidable(true);
	pBox->SetSizeXYZ(Vec3(10.0f, 10.0f, 10.0f));
	m_pSnode_Box->GetLocalTransform().SetPosition(20.0f, 0, 0);
}

void MainApp::_InitGraphicsObjectOfPreviewRender()
{
	//draw full-screen quad
	m_pGraphicObj_ResultPreview = m_pGraphicObjMgr->CreateGraphicObject("GO_result");
	m_pGraphicObj_ResultPreview->AddRectangle(
		Vec2(0, 0), Vec2(m_pRenderer->GetBackBufferWidth(), m_pRenderer->GetBackBufferHeight()),
		Vec4(0, 0, 0, 1.0f), 
		m_pPathTracerRenderTarget->GetTextureName());
}

void MainApp::_InitAreaLight()
{
}

/**************************************

					PROTECTED

*****************************************/

void MainApp::Callback_Mainloop()
{
	switch(mMainloopState)
	{
	case MAINLOOP_STATE::PREVIEW:
		Mainloop_RealTimePreview();
		break;
	case MAINLOOP_STATE::PATH_TRACING_RESULT:
		Mainloop_RenderPathTracedResult();
		break;
	default:
		break;
	}
}

void MainApp::Mainloop_RealTimePreview()
{
	InputProcess_Preview();
	m_pRenderer->ClearBackground();
	mTimer.NextTick();

	//update fps lable
	std::string tmpS;
	tmpS  += "fps :" + std::to_string(mTimer.GetFPS());// << std::endl;
	m_pMyText_fps->SetTextAscii(tmpS);

	//add to render list
	for (auto& pMesh : mMeshList)m_pRenderer->AddToRenderQueue(pMesh);
	m_pRenderer->AddToRenderQueue(m_pGraphicObjBuffer);
	m_pRenderer->AddToRenderQueue(m_pMyText_fps);
	m_pRenderer->SetActiveAtmosphere(m_pAtmos);


	m_pRenderer->Render();
	m_pRenderer->PresentToScreen();
}

void MainApp::Mainloop_RenderPathTracedResult()
{
	Sleep(500);
	m_pRenderer->ClearBackground();
	mTimer.NextTick();
	if (!mIsPathTracerRenderFinished)
	{
		mTotalPathTracerRenderTime += float(mTimer.GetInterval());
	}

	//update fps lable
	std::string tmpS;
	tmpS += "Elapsed Time :" + std::to_string(mTotalPathTracerRenderTime);// << std::endl;
	m_pMyText_fps->SetTextAscii(tmpS);


	//add to render list
	m_pRenderer->AddToRenderQueue(m_pGraphicObj_ResultPreview);
	m_pRenderer->AddToRenderQueue(m_pMyText_fps);

	m_pRenderer->Render();
	m_pRenderer->PresentToScreen();
}

void MainApp::InputProcess_Preview()
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

	//start path tracer render
	if (mInputE.IsKeyPressed(Ut::NOISE_KEY_RETURN))
	{
		mMainloopState = MAINLOOP_STATE::PATH_TRACING_RESULT;
		MainApp::Init_GI();
		MainApp::PathTracerStartRender();
	}

	//quit main loop and terminate program
	if (mInputE.IsKeyPressed(Ut::NOISE_KEY_ESCAPE))
	{
		m_pRoot->SetMainLoopStatus(NOISE_MAINLOOP_STATUS_QUIT_LOOP);
	}

}
