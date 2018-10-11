#include "Main3D.h"

using namespace Noise3D;

Main3DApp::Main3DApp():
	mTimer(Ut::NOISE_TIMER_TIMEUNIT_MILLISECOND),
	mOrbitPitch(0),
	mOrbitYaw(0),
	mSourceTextureType(Main3DApp::TEXTURE2D)
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
	DirLightD* pLight = m_pLightMgr->CreateDynamicDirLight("light");
	pLight->SetAmbientColor(NVECTOR3(1.0f, 1.0f, 1.0f));
	pLight->SetDirection(NVECTOR3(-1.0f, -1.0f, 0));

	//Materials
	N_MaterialDesc matDesc1;
	matDesc1.ambientColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	matDesc1.diffuseColor = NVECTOR3(0, 0, 0);
	matDesc1.specularColor = NVECTOR3(0, 0, 0);
	matDesc1.diffuseMapName = "Tex";
	Material* pMat1 = m_pMatMgr->CreateMaterial("Mat1", matDesc1);

	N_MaterialDesc matDesc2;
	matDesc2.ambientColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	matDesc2.diffuseColor = NVECTOR3(0, 0, 0);
	matDesc2.specularColor = NVECTOR3(0, 0, 0);
	matDesc2.diffuseMapName = "ShTex";
	Material* pMat2 = m_pMatMgr->CreateMaterial("Mat2", matDesc2);

	N_MaterialDesc matDesc3;
	matDesc3.ambientColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	matDesc3.diffuseColor = NVECTOR3(1.0, 1.0, 1.0);
	matDesc3.specularColor = NVECTOR3(0, 0, 0);
	Material* pMat3 = m_pMatMgr->CreateMaterial("MatSolid", matDesc3);


	//2 Spheres and 1 cube(frame) for visualizing SH texture
	m_pMeshSourceSphere = m_pMeshMgr->CreateMesh("srcSphere");
	m_pModelLoader->LoadSphere(m_pMeshSourceSphere, 1.0f, 30, 30);
	m_pMeshSourceSphere->SetPosition(c_ballPos1);
	m_pMeshSourceSphere->SetMaterial("Mat1");

	m_pMeshShSphere = m_pMeshMgr->CreateMesh("sphereSh");
	m_pModelLoader->LoadSphere(m_pMeshShSphere, 1.0f, 30, 30);
	m_pMeshShSphere->SetPosition(c_ballPos2);
	m_pMeshShSphere->SetMaterial("Mat2");

	m_pMeshSourceCube = m_pMeshMgr->CreateMesh("cubeFrame");
	m_pModelLoader->LoadBox(m_pMeshSourceCube, 2.1f, 2.1f, 2.1f);
	m_pMeshSourceCube->SetPosition(c_ballPos1);
	m_pMeshSourceCube->SetFillMode(NOISE_FILLMODE::NOISE_FILLMODE_WIREFRAME);
	m_pMeshSourceCube->SetMaterial("MatSolid");

	//create font texture and top-left fps label
	m_pTextMgr = m_pScene->GetTextMgr();
	m_pTextMgr->CreateFontFromFile("../media/calibri.ttf", "myFont", 24);
	m_pMyText_fps = m_pTextMgr->CreateDynamicTextA("myFont", "fpsLabel", "fps:000", 200, 100, NVECTOR4(0, 0, 0, 1.0f), 0, 0);
	m_pMyText_fps->SetTextColor(NVECTOR4(0, 0.8f, 0.7f, 0.5f));
	m_pMyText_fps->SetSpacePixelWidth(4);//width of space
	m_pMyText_fps->SetDiagonal(NVECTOR2(canvasWidth-80.0f, 20), NVECTOR2(canvasWidth, 60));
	m_pMyText_fps->SetBlendMode(NOISE_BLENDMODE_ALPHA);

	m_pMyText_camProjType = m_pTextMgr->CreateDynamicTextA("myFont", "camTypeLabel", "Camera Mode: Perspective", 300, 100, NVECTOR4(0, 0, 0, 1.0f), 1, 0);
	m_pMyText_camProjType->SetTextColor(NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	m_pMyText_camProjType->SetSpacePixelWidth(4);//width of space
	m_pMyText_camProjType->SetDiagonal(NVECTOR2(canvasWidth - 400.0f, 20), NVECTOR2(canvasWidth-100.0f, 60));
	m_pMyText_camProjType->SetBlendMode(NOISE_BLENDMODE_ALPHA);

	//Camera
	m_pCamera->SetProjectionType(true);
	m_pCamera->SetOrthoViewSize(6.0f, 6.0f * canvasHeight / float(canvasWidth));//orthographic proj
	m_pCamera->SetViewAngle_Radian(Ut::PI / 3.0f, 1.333333333f);//perspective proj
	m_pCamera->SetViewFrustumPlane(1.0f, 500.f);//perspective proj
	m_pCamera->SetPosition(0, 0, -5.0f);
	m_pCamera->LookAt(0, 0, 0);

	//draw 2d texture
	m_pGO_GUI = m_pGraphicObjMgr->CreateGraphicObj("tanList");
	m_pGO_GUI->SetBlendMode(NOISE_BLENDMODE_ALPHA);
	m_pGO_GUI->AddRectangle(NVECTOR2(75.0f, 75.0f), 100.0f,100.0f, NVECTOR4(1.0f, 0, 0, 1.0f), "Tex");
	m_pGO_GUI->AddRectangle(NVECTOR2(200.0f, 75.0f),100.0f,100.0f, NVECTOR4(1.0f, 0, 0, 1.0f), "ShTex");

	m_pGO_Axis = m_pGraphicObjMgr->CreateGraphicObj("Axis");
	m_pGO_Axis->SetBlendMode(NOISE_BLENDMODE_ALPHA);
	m_pGO_Axis->AddLine3D(NVECTOR3(-1.5f, 0, 0), NVECTOR3(-1.5f+1.5f, 0, 0), NVECTOR4(1.0f, 0, 0, 1.0f), NVECTOR4(1.0f, 0, 0, 1.0f));
	m_pGO_Axis->AddLine3D(NVECTOR3(-1.5f, 0, 0), NVECTOR3(-1.5f, 1.5f, 0), NVECTOR4(0, 1.0f, 0, 1.0f), NVECTOR4(0, 1.0f, 0, 1.0f));
	m_pGO_Axis->AddLine3D(NVECTOR3(-1.5f, 0, 0), NVECTOR3(-1.5f, 0, 1.5f), NVECTOR4(0, 0, 1.0f, 1.0f), NVECTOR4(0, 0, 1.0f, 1.0f));
	m_pGO_Axis->AddLine3D(NVECTOR3(1.5f, 0, 0), NVECTOR3(1.5f + 1.5f, 0, 0), NVECTOR4(1.0f, 0, 0, 1.0f), NVECTOR4(1.0f, 0, 0, 1.0f));
	m_pGO_Axis->AddLine3D(NVECTOR3(1.5f, 0, 0), NVECTOR3(1.5f, 1.5f, 0), NVECTOR4(0, 1.0f, 0, 1.0f), NVECTOR4(0, 1.0f, 0, 1.0f));
	m_pGO_Axis->AddLine3D(NVECTOR3(1.5f, 0, 0), NVECTOR3(1.5f, 0, 1.5f), NVECTOR4(0, 0, 1.0f, 1.0f), NVECTOR4(0, 0, 1.0f, 1.0f));

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
	if (mSourceTextureType == Main3DApp::SOURCE_TEXTURE_TYPE::TEXTURE2D)
	{
		m_pRenderer->AddToRenderQueue(m_pMeshSourceSphere);
	}
	else if(mSourceTextureType== Main3DApp::SOURCE_TEXTURE_TYPE::CUBEMAP)
	{
		m_pRenderer->AddToRenderQueue(m_pMeshSourceSphere);
		m_pRenderer->AddToRenderQueue(m_pMeshSourceCube);
	}
	m_pRenderer->AddToRenderQueue(m_pMeshShSphere);
	m_pRenderer->AddToRenderQueue(m_pGO_GUI);
	m_pRenderer->AddToRenderQueue(m_pGO_Axis);
	m_pRenderer->AddToRenderQueue(m_pMyText_fps);
	m_pRenderer->AddToRenderQueue(m_pMyText_camProjType);

	//render
	m_pRenderer->Render();

	//present
	m_pRenderer->PresentToScreen();
}

bool Main3DApp::LoadOriginalTexture2D(std::string filePath)
{
	//reload texture
	if (m_pOriginTex != nullptr)
	{
		m_pTexMgr->DeleteTexture2D(c_originTexName);
		m_pOriginTex = m_pTexMgr->CreateTextureFromFile(filePath, c_originTexName, true, c_defaultTexWidth, c_defaultTexWidth, true);
	}

	if (m_pOriginTex == nullptr)
	{
		return false;
	}

	mSourceTextureType = Main3DApp::TEXTURE2D;
	return true;
}

bool Main3DApp::LoadOriginalTextureCubeMap(std::string filePath)
{
	//reload texture
	if (m_pOriginTex != nullptr)
	{
		//delete previous existed texture
		m_pTexMgr->DeleteTexture2D(c_originTexName);
		m_pTexMgr->DeleteTextureCubeMap(c_originCubeMapTex);

		//load cube map and manually map the cube map into a texture2d
		m_pOriginCubeMap = m_pTexMgr->CreateCubeMapFromDDS(filePath, c_originCubeMapTex, true);//load cube map
		m_pOriginTex = m_pTexMgr->CreatePureColorTexture(c_originTexName, c_defaultTexWidth, c_defaultTexWidth, NVECTOR4(1.0f, 0, 0, 1.0f),true);//map the cube map to texture 2d

		//map the cubemap into 2d 'longitude-altitude map'
		uint32_t width = m_pOriginTex->GetWidth();
		uint32_t height = m_pOriginTex->GetHeight();
		std::vector<NColor4u> colorBuff(width*height);
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				//fill every pixel with a texel of cube map in certain direction
				NVECTOR3 dir = Ut::GetDirFromPixelUVCoord(x, y, width, height);
				NColor4u cubeMapColor = m_pOriginCubeMap->GetPixel(dir, TextureCubeMap::N_TEXTURE_CPU_SAMPLE_MODE::POINT);
				colorBuff.at(y*width + x) = cubeMapColor;
			}
		}
		m_pOriginTex->SetPixelArray(colorBuff);
		m_pOriginTex->UpdateToVideoMemory();

	}

	if (m_pOriginCubeMap == nullptr || m_pOriginTex==nullptr)
	{
		return false;
	}

	mSourceTextureType = Main3DApp::CUBEMAP;
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
	//object fixed, camera's orbit rotation is easy; 
	//but camera fixed, object's orbit rotation are the inverse rotation
	RigidTransform t;
	mOrbitPitch += deltaPitch;
	mOrbitYaw += deltaYaw;
	mOrbitPitch = Ut::Clamp(mOrbitPitch, -Ut::PI / 2.0f, Ut::PI / 2.0f);

	//rotate coord frame
	t.SetRotation(mOrbitPitch, mOrbitYaw, 0);
	NVECTOR3 axisDirX = t.TransformVector(NVECTOR3(1.5f, 0, 0));
	NVECTOR3 axisDirY = t.TransformVector(NVECTOR3(0, 1.5f, 0));
	NVECTOR3 axisDirZ = t.TransformVector(NVECTOR3(0, 0, 1.5f));

	m_pGO_Axis->SetLine3D(0, c_ballPos1, c_ballPos1 + axisDirX, NVECTOR4(1.0f, 0, 0, 1.0f), NVECTOR4(1.0f, 0, 0, 1.0f));
	m_pGO_Axis->SetLine3D(1, c_ballPos1, c_ballPos1 + axisDirY, NVECTOR4(0, 1.0f, 0, 1.0f), NVECTOR4(0, 1.0f, 0, 1.0f));
	m_pGO_Axis->SetLine3D(2, c_ballPos1, c_ballPos1 + axisDirZ, NVECTOR4(0, 0, 1.0f, 1.0f), NVECTOR4(0, 0, 1.0f, 1.0f));

	m_pGO_Axis->SetLine3D(3, c_ballPos2, c_ballPos2 + axisDirX, NVECTOR4(1.0f, 0, 0, 1.0f), NVECTOR4(1.0f, 0, 0, 1.0f));
	m_pGO_Axis->SetLine3D(4, c_ballPos2, c_ballPos2 + axisDirY, NVECTOR4(0, 1.0f, 0, 1.0f), NVECTOR4(0, 1.0f, 0, 1.0f));
	m_pGO_Axis->SetLine3D(5, c_ballPos2, c_ballPos2 + axisDirZ, NVECTOR4(0, 0, 1.0f, 1.0f), NVECTOR4(0, 0, 1.0f, 1.0f));

	//the original version of orbit rotation should be a camera rotation
	t.SetRotation(mOrbitPitch, mOrbitYaw, 0);
	t.InvertRotation();
	if (index == 0)
	{
		m_pMeshSourceSphere->SetRotation(t.GetQuaternion());
		m_pMeshSourceCube->SetRotation(t.GetQuaternion());
	}
	else if (index == 1)
	{
		m_pMeshShSphere->SetRotation(t.GetQuaternion());
	}

	return;
}

void Main3DApp::Cleanup()
{
	m_pRoot->ReleaseAll();
}

void Main3DApp::SetCamProjType(bool isPerspective)
{
	m_pCamera->SetProjectionType(isPerspective);
	if (isPerspective)
	{
		m_pMyText_camProjType->SetTextAscii("Camera Mode: Perspective");
	}
	else
	{
		m_pMyText_camProjType->SetTextAscii("Camera Mode: Orthographic");

	}
}

/*************************************************

								PRIVATE

*************************************************/
/*void Main3DApp::mFunction_GenSpecialCube(std::vector<Noise3D::N_DefaultVertex>& outVertexList, std::vector<UINT>& outIndexList)
{
	float r = 1.0f;
	N_DefaultVertex v[24];
	//+x
	v[0].TexCoord = { 0,0 };
	v[0].Pos = { 1,1,-1 };
	v[1].TexCoord = { 0.333333f, 0 };
	v[1].Pos = { 1, 1, 1 };
	v[2].TexCoord = { 0, 0.333333f };
	v[2].Pos = { 1, -1 ,1 };
	v[3].TexCoord = { 0.333333f, 0.333333f };
	v[3].Pos = { 1, -1 ,1 };


}*/

void Main3DApp::mFunction_SHPreprocess_SphericalMap(int shOrder, int monteCarloSampleCount, std::vector<NVECTOR3>& outShVector)
{
	GI::SHVector shvec;
	GI::ISphericalMappingTextureSampler defaultSphFunc;
	defaultSphFunc.SetTexturePtr(m_pOriginTex);
	shvec.Project(shOrder, monteCarloSampleCount, &defaultSphFunc);
	shvec.GetCoefficients(outShVector);

	uint32_t width = m_pShTex->GetWidth();
	uint32_t height = m_pShTex->GetHeight();
	std::vector<NColor4u> colorBuff(width*height);
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			NVECTOR3 dir = Ut::GetDirFromPixelUVCoord(x, y, width, height);
			NVECTOR3 reconstructedColor = shvec.Eval(dir);
			NColor4u color = { uint8_t(reconstructedColor.x * 255.0f), uint8_t(reconstructedColor.y * 255.0f) , uint8_t(reconstructedColor.z * 255.0f),255 };
			colorBuff.at(y*width + x) = color;
		}
	}
	m_pShTex->SetPixelArray(colorBuff);
	m_pShTex->UpdateToVideoMemory();
}

void Main3DApp::mFunction_SHPreprocess_CubeMap(int shOrder, int monteCarloSampleCount, std::vector<NVECTOR3>& outShVector)
{
}
