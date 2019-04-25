#include "Noise3D.h"
#include "SceneLoader.h"
#include "RealTimeRenderModule.h"
#include "MainApp.h"

using namespace Noise3D;

MainApp::MainApp():
	mTotalPathTracerRenderTime(0.0f),
	mMainloopState(MAINLOOP_STATE::PREVIEW),
	m_pPathTracer(nullptr),
	m_pGraphicObj_ResultPreview(nullptr),
	m_pPathTracerRenderTarget(nullptr)
{
}

MainApp::~MainApp()
{

}

void MainApp::Init_GI()
{
	MainApp::_InitPathTracer();
	MainApp::_InitGraphicsObjectOfPreviewRender();
	MainApp::_InitSoftShader();

	if (!mCmdLine.empty())
	{
		std::stringstream ss;
		ss << mCmdLine;
		std::string str_mat; float m, r;
		ss >> str_mat >> m >> r;
		auto pMat = m_pMatMgr->GetObjectPtr<GI::AdvancedGiMaterial>(str_mat);
		if (pMat != nullptr)
		{
			pMat->SetMetallicity(m);
			pMat->SetRoughness(r);
		}
	}
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
	mRenderThread = std::thread(
		functor,
		m_pPathTracer,
		m_pScene->GetSceneGraph().GetRoot(),
		&mPathTracerShader_Standard);
		//&mPathTracerShader_AreaLightingDemo);
		//&mPathTracerShader_RefractionDemo);
		//&mPathTracerShader_DiffuseDemo);
		//&mPathTracerShader_ReflectionDemo);
		//&mPathTracerShader_Minimal);
	//mRenderThread.detach();

}

void MainApp::InitCmdLine(std::string cmdLine)
{
	mCmdLine = cmdLine;
}


/**************************************

					PROTECTED

*****************************************/

void MainApp::_InitPathTracer()
{
	m_pPathTracer = m_pScene->CreatePathTracer(640, 480);
	m_pPathTracerRenderTarget = m_pPathTracer->GetRenderTarget();
	m_pPathTracer->SetMaxBounces(2);
	m_pPathTracer->SetMaxDiffuseSampleCount(64);
	m_pPathTracer->SetMaxSpecularScatterSample(16);
	m_pPathTracer->SetRayMaxTravelDist(100000.0f);
	m_pPathTracer->SetExposure(2.0f);
}

void MainApp::_InitGraphicsObjectOfPreviewRender()
{
	//draw full-screen quad
	m_pGraphicObj_ResultPreview = m_pGraphicObjMgr->CreateGraphicObject("GO_result");
	m_pGraphicObj_ResultPreview->AddRectangle(
		Vec2(0, 0), 
		Vec2(float(m_pRenderer->GetBackBufferWidth()), float(m_pRenderer->GetBackBufferHeight())),
		Vec4(0, 0, 0, 1.0f), 
		m_pPathTracerRenderTarget->GetTextureName());
	m_pMyText_fps->SetTextColor(Vec4(1.0f,1.0,1.0f,1.0f));
}

void MainApp::_InitSoftShader()
{
	//Minimal
	//....

	//reflection demo
	mPathTracerShader_ReflectionDemo.SetSkyTexture(m_pTexMgr->GetObjectPtr<Texture2D>("envmap"));

	//Diffuse demo
	mPathTracerShader_DiffuseDemo.SetSkyTexture(m_pTexMgr->GetObjectPtr<Texture2D>("envmap"));

	//refraction demo
	mPathTracerShader_RefractionDemo.SetSkyTexture(m_pTexMgr->GetObjectPtr<Texture2D>("envmap"));

	//area lighting
	//...

	//standard
	mPathTracerShader_Standard.SetSkyDomeTexture(m_pTexMgr->GetObjectPtr<Texture2D>("envmap"));
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

void MainApp::Callback_Cleanup()
{
	if(m_pPathTracer)m_pPathTracer->TerminateRenderTask();
	if(mRenderThread.joinable())mRenderThread.join();
}

void MainApp::Mainloop_RealTimePreview()
{
	m_pRenderer->ClearBackground();
	mTimer.NextTick();
	InputProcess_Preview();

	//update fps lable
	std::string tmpS;
	tmpS  += "fps :" + std::to_string(mTimer.GetFPS());// << std::endl;
	m_pMyText_fps->SetTextAscii(tmpS);

	//add to render list
	for (auto& pMesh : mRealTimeRenderMeshList)m_pRenderer->AddToRenderQueue(pMesh);
	m_pRenderer->AddToRenderQueue(m_pGraphicObjBuffer);
	m_pRenderer->AddToRenderQueue(m_pMyText_fps);
	m_pRenderer->SetActiveAtmosphere(m_pAtmos);


	m_pRenderer->Render();
	m_pRenderer->PresentToScreen();
}

void MainApp::Mainloop_RenderPathTracedResult()
{
	Sleep(200);
	mTimer.NextTick();
	m_pRenderer->ClearBackground();
	InputProcess_PathTracerRendering();


	static bool hasRTLastUpdate = false;	//last update of render target
	bool isFinished = m_pPathTracer->IsRenderFinished();

	if (!isFinished)mTotalPathTracerRenderTime += float(mTimer.GetInterval());

	if (!isFinished && mTimer.GetTotalTimeElapsed()>1000.0f)
	{
		m_pPathTracerRenderTarget->UpdateToVideoMemory();
		mTimer.ResetTotalTime();
	}

	if(!hasRTLastUpdate && isFinished)
	{
		m_pPathTracerRenderTarget->UpdateToVideoMemory();
		m_pPathTracerRenderTarget->SaveTexture2DToFile("output.bmp", NOISE_IMAGE_FILE_FORMAT_BMP);
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
		m_pPathTracer->TerminateRenderTask();
	}

}

void MainApp::InputProcess_PathTracerRendering()
{
	if (mInputE.IsKeyPressed(Ut::NOISE_KEY::NOISE_KEY_SYSRQ))
	{
		m_pPathTracerRenderTarget->SaveTexture2DToFile("output.bmp", NOISE_IMAGE_FILE_FORMAT_BMP);
	}

	//quit main loop and terminate program
	if (mInputE.IsKeyPressed(Ut::NOISE_KEY_ESCAPE))
	{
		m_pRoot->SetMainLoopStatus(NOISE_MAINLOOP_STATUS_QUIT_LOOP);
	}
}
