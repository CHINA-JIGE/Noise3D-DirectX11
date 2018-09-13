#include "Main3D.h"

using namespace Noise3D;

Main3DApp::Main3DApp():
	mTimer(Ut::NOISE_TIMER_TIMEUNIT_MILLISECOND)
{

}

Main3DApp::~Main3DApp()
{

}

bool Main3DApp::InitNoise3D(HWND renderCanvasHwnd, HWND inputHwnd, UINT canvasWidth, UINT canvasHeight)
{
	//initialize input engine (detection for keyboard and mouse input)
	//https://stackoverflow.com/questions/4547073/c-on-windows-using-directinput-without-a-window
	//it is said that an hInstance/hWnd is not necessary for a dinput device???
	//mInputE.Initialize(::GetModuleHandle(NULL), inputHwnd);

	m_pRoot = Noise3D::GetRoot();
	if (!m_pRoot->Init())return false;

	//query pointer to IScene
	m_pScene = m_pRoot->GetScenePtr();

	m_pMeshMgr = m_pScene->GetMeshMgr();
	m_pRenderer = m_pScene->CreateRenderer(canvasWidth, canvasHeight, renderCanvasHwnd);
	m_pCamera = m_pScene->GetCamera();
	m_pMatMgr = m_pScene->GetMaterialMgr();
	m_pTexMgr = m_pScene->GetTextureMgr();
	m_pLightMgr = m_pScene->GetLightMgr();
	m_pGraphicObjMgr = m_pScene->GetGraphicObjMgr();
	m_pModelLoader = m_pScene->GetModelLoader();

	//2 textures
	m_pOriginTex = m_pTexMgr->CreateTextureFromFile("../media/example.jpg", c_originTexName, true, 512, 512, true);
	m_pShTex = m_pTexMgr->CreatePureColorTexture(c_ShTexName, c_defaultTexWidth, c_defaultTexWidth, NVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), true);
	std::vector<NVECTOR3> tmpShVector;
	mFunction_SHPreprocess_SphericalMap(3,10000, tmpShVector);

	//light to exhibit base color
	IDirLightD* pLight = m_pLightMgr->CreateDynamicDirLight("light");
	pLight->SetAmbientColor(NVECTOR3(1.0f, 1.0f, 1.0f));
	pLight->SetDirection(NVECTOR3(-1.0f, -1.0f, 0));

	//Materials
	N_MaterialDesc matDesc1;
	matDesc1.ambientColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	matDesc1.diffuseColor = NVECTOR3(0, 0, 0);
	matDesc1.specularColor = NVECTOR3(0, 0, 0);
	matDesc1.diffuseMapName = "Tex";
	IMaterial* pMat1 = m_pMatMgr->CreateMaterial("Mat1", matDesc1);

	N_MaterialDesc matDesc2;
	matDesc2.ambientColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	matDesc2.diffuseColor = NVECTOR3(0, 0, 0);
	matDesc2.specularColor = NVECTOR3(0, 0, 0);
	matDesc2.diffuseMapName = "ShTex";
	IMaterial* pMat2 = m_pMatMgr->CreateMaterial("Mat2", matDesc2);


	//2 Spheres for visualizing SH texture
	m_pMeshOriginal = m_pMeshMgr->CreateMesh("sphereOriginal");
	m_pModelLoader->LoadSphere(m_pMeshOriginal, 1.0f, 30, 30);
	m_pMeshOriginal->SetPosition(NVECTOR3(-1.5f, 0, 0));
	m_pMeshOriginal->SetMaterial("Mat1");

	m_pMeshSh = m_pMeshMgr->CreateMesh("sphereSh");
	m_pModelLoader->LoadSphere(m_pMeshSh, 1.0f, 30, 30);
	m_pMeshSh->SetPosition(NVECTOR3(1.5f, 0, 0));
	m_pMeshSh->SetMaterial("Mat2");

	//create font texture and top-left fps label
	m_pFontMgr = m_pScene->GetFontMgr();
	m_pFontMgr->CreateFontFromFile("../media/calibri.ttf", "myFont", 24);
	m_pMyText_fps = m_pFontMgr->CreateDynamicTextA("myFont", "fpsLabel", "fps:000", 200, 100, NVECTOR4(0, 0, 0, 1.0f), 0, 0);
	m_pMyText_fps->SetTextColor(NVECTOR4(0, 0.8f, 0.7f, 0.5f));
	m_pMyText_fps->SetDiagonal(NVECTOR2(canvasWidth-80.0f, 20), NVECTOR2(canvasWidth, 60));
	m_pMyText_fps->SetFont("myFont");
	m_pMyText_fps->SetBlendMode(NOISE_BLENDMODE_ALPHA);

	//Camera
	m_pCamera->SetPosition(0, 0, -5.0f);
	m_pCamera->SetLookAt(0, 0, 0);
	m_pCamera->SetViewAngle_Radian(MATH_PI / 3.0f, 1.333333333f);
	m_pCamera->SetViewFrustumPlane(1.0f, 500.f);

	//draw 2d texture
	m_pGO_GUI = m_pGraphicObjMgr->CreateGraphicObj("tanList");
	m_pGO_GUI->SetBlendMode(NOISE_BLENDMODE_ALPHA);
	m_pGO_GUI->AddRectangle(NVECTOR2(75.0f, 75.0f), 100.0f,100.0f, NVECTOR4(1.0f, 0, 0, 1.0f), "Tex");
	m_pGO_GUI->AddRectangle(NVECTOR2(200.0f, 75.0f),100.0f,100.0f, NVECTOR4(1.0f, 0, 0, 1.0f), "ShTex");

	return true;
}

void Main3DApp::UpdateFrame()
{
	//clear background
	m_pRenderer->ClearBackground(NVECTOR4(0.2f, 0.2f, 0.2f, 1.0f));
	mTimer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS << "fps :" << mTimer.GetFPS();
	m_pMyText_fps->SetTextAscii(tmpS.str());


	//add to render list
	m_pRenderer->AddToRenderQueue(m_pMeshOriginal);
	m_pRenderer->AddToRenderQueue(m_pMeshSh);
	m_pRenderer->AddToRenderQueue(m_pGO_GUI);
	m_pRenderer->AddToRenderQueue(m_pMyText_fps);

	//render
	m_pRenderer->Render();

	//present
	m_pRenderer->PresentToScreen();
}

bool Main3DApp::LoadOriginalTexture(std::string filePath)
{
	//reload texture
	if (m_pOriginTex != nullptr)
	{
		m_pTexMgr->DeleteTexture(c_originTexName);
		m_pOriginTex = m_pTexMgr->CreateTextureFromFile(filePath, c_originTexName, true, c_defaultTexWidth, c_defaultTexWidth, true);
	}

	if (m_pOriginTex == nullptr)
	{
		return false;
	}

	return true;
}

bool Main3DApp::ComputeShTexture(SH_TEXTURE_TYPE texType, int shOrder, int monteCarloSampleCount, std::vector<NVECTOR3>& outShVector)
{
	switch (texType)
	{
	case SH_TEXTURE_TYPE::CUBE_MAP:
	{
		mFunction_SHPreprocess_CubeMap(shOrder, monteCarloSampleCount, outShVector);
		break;
	}
	default:
	case SH_TEXTURE_TYPE::COMMON:
	{
		mFunction_SHPreprocess_SphericalMap(shOrder, monteCarloSampleCount, outShVector);
		break;
	}
	}

	return true;
}

void Main3DApp::RotateBall(int index, float deltaYaw, float deltaPitch)
{
	if (index == 0)
	{
		m_pMeshOriginal->RotateX_Pitch(deltaPitch);
		m_pMeshOriginal->RotateY_Yaw(deltaYaw);
	}
	else if (index == 1)
	{
		m_pMeshSh->RotateX_Pitch(deltaPitch);
		m_pMeshSh->RotateY_Yaw(deltaYaw);
	}

	return;
}

void Main3DApp::Cleanup()
{
	m_pRoot->ReleaseAll();
}


/*************************************************

								PRIVATE

*************************************************/
void Main3DApp::mFunction_SHPreprocess_SphericalMap(int shOrder, int monteCarloSampleCount, std::vector<NVECTOR3>& outShVector)
{
	GI::SHVector shvec;
	GI::ISphericalMappingTextureSampler defaultSphFunc;
	defaultSphFunc.SetTexture(m_pOriginTex);
	shvec.Project(shOrder, monteCarloSampleCount, &defaultSphFunc);
	shvec.GetCoefficients(outShVector);

	uint32_t width = m_pShTex->GetWidth();
	uint32_t height = m_pShTex->GetHeight();
	std::vector<NColor4u> colorBuff(width*height);
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			float normalizedU = float(x) / float(width);//[0,1]
			float normalizedV = float(y) / float(height);//[0,1]
			float yaw = (normalizedU - 0.5f) * 2.0f * MATH_PI;//[-pi,pi]
			float pitch = (normalizedV - 0.5f) * MATH_PI;//[pi/2,-pi/2]
			NVECTOR3 dir = { sinf(yaw)*cosf(pitch),  sinf(pitch) ,cosf(yaw)*cosf(pitch) };
			NVECTOR3 reconstructedColor = shvec.Eval(dir);
			NColor4u color = { uint8_t(reconstructedColor.x * 255.0f), uint8_t(reconstructedColor.y * 255.0f) , uint8_t(reconstructedColor.z * 255.0f),255 };
			colorBuff.at(y*width + x) = color;
		}
	}
	m_pShTex->SetPixelArray(colorBuff);
	m_pShTex->UpdateToVideoMemory();
}

void Main3DApp::mFunction_SHPreprocess_CubeMap(int shOrder, int monteCarloSampleCount, std::vector<Noise3D::NVECTOR3>& outShVector)
{
}
