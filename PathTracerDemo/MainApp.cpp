#include "Noise3D.h"
#include "SceneLoader.h"
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
	m_pPathTracerRenderTarget(nullptr)
{
}

MainApp::~MainApp()
{
}

void MainApp::PathTracerStartRender()
{
	mTotalPathTracerRenderTime = 0.0f;
	mTimer.ResetAll();
	mTimer.NextTick();

	//start an independent thread from path tracer render
	struct PathTracerRenderFunctor
	{
		void operator()(GI::PathTracer* p, SceneNode* pNode, GI::IPathTracerSoftShader* pShaders)
		{	
			p->Render(pNode, pShaders);
		}
	};
	PathTracerRenderFunctor functor;
	std::thread renderThread(functor, m_pPathTracer, m_pScene->GetSceneGraph().GetRoot(), &mPathTracerShader_Minimal);
	renderThread.detach();

}

void MainApp::Init_GI()
{
	MainApp::_InitPathTracer();
	MainApp::_InitGraphicsObjectOfPreviewRender();
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

void MainApp::_InitGraphicsObjectOfPreviewRender()
{
	//draw full-screen quad
	m_pGraphicObj_ResultPreview = m_pGraphicObjMgr->CreateGraphicObject("GO_result");
	m_pGraphicObj_ResultPreview->AddRectangle(
		Vec2(0, 0), Vec2(m_pRenderer->GetBackBufferWidth(), m_pRenderer->GetBackBufferHeight()),
		Vec4(0, 0, 0, 1.0f), 
		m_pPathTracerRenderTarget->GetTextureName());
	m_pMyText_fps->SetTextColor(Vec4(1.0f,1.0,1.0f,1.0f));
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
	//Sleep(50);
	mTimer.NextTick();
	m_pRenderer->ClearBackground();
	static bool hasRTLastUpdate = false;	//last update of render target

	bool isFinished = m_pPathTracer->IsRenderFinished();
	if (!isFinished && mTimer.GetTotalTimeElapsed()>500.0f)
	{
		mTotalPathTracerRenderTime += float(mTimer.GetInterval());
		m_pPathTracerRenderTarget->UpdateToVideoMemory();
		mTimer.ResetTotalTime();
	}

	if(!hasRTLastUpdate && isFinished)
	{
		mTotalPathTracerRenderTime += float(mTimer.GetInterval());
		m_pPathTracerRenderTarget->UpdateToVideoMemory();
		hasRTLastUpdate = true;
	}


	//update fps lable
	std::string tmpS;
	tmpS += "Elapsed Time :" + std::to_string(mTotalPathTracerRenderTime) +"ms";// << std::endl;
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
