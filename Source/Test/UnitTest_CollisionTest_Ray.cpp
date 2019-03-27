#include "Noise3D.h"
#include <sstream>

BOOL Init3D(HWND hwnd);
void MainLoop();
void Cleanup();
void	InputProcess();

using namespace Noise3D;

Root* pRoot;
Renderer* pRenderer;
SceneManager* pScene;
Camera* pCamera;
Atmosphere* pAtmos;
MeshLoader* pModelLoader;
MeshManager* pMeshMgr;
std::vector<Mesh*> meshList;
std::vector<LogicalSphere*> logicalSphereList;
MaterialManager*	pMatMgr;
TextureManager*	pTexMgr;
GraphicObjectManager*	pGraphicObjMgr;
GraphicObject*	pGraphicObjBuffer;
LogicalShapeManager* pShapeMgr;
LightManager* pLightMgr;
DirLight*		pDirLight1;
PointLight*	pPointLight1;
SpotLight*	pSpotLight1;
TextManager* pTextMgr;
DynamicText* pMyText_fps;

SceneNode* sceneRoot;
SceneNode* a;
SceneNode* aa;
SceneNode* ab;
SceneNode* ac;
SceneNode* aca;
SceneNode* b;
SceneNode* fbxNode;

Ut::Timer timer(Ut::NOISE_TIMER_TIMEUNIT_MILLISECOND);
Ut::InputEngine inputE;

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
	if (!pRoot->Init())return FALSE;

	//query pointer to IScene
	pScene = pRoot->GetSceneMgrPtr();

	//retrieve meshMgr and Create new mesh
	pMeshMgr = pScene->GetMeshMgr();

	//use "myMesh1" string to initialize UID (unique-Identifier)
	//pMesh1= pMeshMgr->CreateMesh("myMesh1");


	pRenderer = pScene->CreateRenderer(bufferWidth, bufferHeight, hwnd);
	pCamera = pScene->GetCamera();
	pLightMgr = pScene->GetLightMgr();
	pMatMgr = pScene->GetMaterialMgr();
	pTexMgr = pScene->GetTextureMgr();
	pAtmos = pScene->GetAtmosphere();
	pGraphicObjMgr = pScene->GetGraphicObjMgr();
	pShapeMgr = pScene->GetLogicalShapeMgr();

	SceneGraph& sg = pScene->GetSceneGraph();
	sceneRoot = sg.GetRoot();
	a = sceneRoot->CreateChildNode();
	aa = a->CreateChildNode();
	ab = a->CreateChildNode();
	ac = a->CreateChildNode();
	aca = ac->CreateChildNode();
	b = sceneRoot->CreateChildNode();

	//Âþ·´ÉäÌùÍ¼
	pTexMgr->CreateTextureFromFile("../media/earth.jpg", "Earth", TRUE, 1024, 1024, FALSE);
	//pTexMgr->CreateTextureFromFile("../media/Jade.jpg", "Jade", FALSE, 256, 256, FALSE);
	//pTexMgr->CreateTextureFromFile("../media/universe.jpg", "Universe", FALSE, 256, 256, FALSE);
	pTexMgr->CreateTextureFromFile("../media/white.jpg", "Universe", FALSE, 256, 256, FALSE);
	//pTexMgr->CreateCubeMapFromDDS("../media/CubeMap/cube-room.dds", "Universe", FALSE);
	//pTexMgr->CreateTextureFromFile("../media/bottom-right-conner-title.jpg", "BottomRightTitle", TRUE, 0, 0, FALSE);
	//pTexMgr->CreateCubeMapFromDDS("../media/UniverseEnv.dds", "AtmoTexture");
	ITexture* pNormalMap = pTexMgr->CreateTextureFromFile("../media/earth-normal.png", "EarthNormalMap", FALSE, 512, 512, TRUE);
	//pNormalMap->ConvertTextureToGreyMap();
	//pNormalMap->ConvertHeightMapToNormalMap(10.0f);


	//create font texture
	pTextMgr = pScene->GetTextMgr();
	pTextMgr->CreateFontFromFile("../media/calibri.ttf", "myFont", 24);
	pMyText_fps = pTextMgr->CreateDynamicTextA("myFont", "fpsLabel", "fps:000", 200, 100, NVECTOR4(0, 0, 0, 1.0f), 0, 0);
	pMyText_fps->SetTextColor(NVECTOR4(0, 0.3f, 1.0f, 0.5f));
	pMyText_fps->SetDiagonal(NVECTOR2(20, 20), NVECTOR2(170, 60));
	pMyText_fps->SetFont("myFont");
	pMyText_fps->SetBlendMode(NOISE_BLENDMODE_ALPHA);


	//------------------MESH INITIALIZATION----------------

	//pModelLoader->LoadSphere(pMesh1,5.0f, 30, 30);
	pModelLoader = pScene->GetModelLoader();
	N_SceneLoadingResult res;

	const int c_shapeCount = 10;
	for (int i = 0; i < c_shapeCount; ++i)
	{
		SceneNode* pNode = sg.GetRoot()->CreateChildNode();
		//bind mesh
		Mesh* pMesh = pMeshMgr->CreateMesh(pNode, "sphere"+ std::to_string(i));
		pMesh->SetCollidable(false);

		LogicalSphere* pSphere = pShapeMgr->CreateSphere(pNode, "logicSPH" + std::to_string(i));
		pSphere->SetCollidable(true);

		GI::RandomSampleGenerator g;
		float randomRadius = g.CanonicalReal() * 20.0f + 3.0f;
		pModelLoader->LoadSphere(pMesh, randomRadius, 15, 15);
		float randomX = g.NormalizedReal() * 50.0f;
		float randomY = g.NormalizedReal() * 50.0f;
		float randomZ = g.NormalizedReal() * 50.0f;
		pNode->GetLocalTransform().SetPosition(randomX, randomY, randomZ);

		pSphere->SetRadius(randomRadius);

		meshList.push_back(pMesh);
		logicalSphereList.push_back(pSphere);
	}

	for (auto & name : res.meshNameList)
	{
		Mesh* pMesh = pMeshMgr->GetObjectPtr(name);
		pMesh->SetCullMode(NOISE_CULLMODE_NONE);
		//pMesh->SetShadeMode(NOISE_SHADEMODE_GOURAUD);
		pMesh->SetFillMode(NOISE_FILLMODE_WIREFRAME);
		meshList.push_back(pMesh);
	}

	//-----Generate Rays--------
	std::vector<N_Ray> rayArray;
	pGraphicObjBuffer = pGraphicObjMgr->CreateGraphicObject("rays");
	const int c_rayCount = 100;
	for (int rayId = 0; rayId < c_rayCount; ++rayId)
	{
		GI::RandomSampleGenerator g;
		NVECTOR3 origin = { g.NormalizedReal() * 100.0f,g.NormalizedReal() * 100.0f,g.NormalizedReal() * 100.0f };
		//dir heads approximately the origin
		NVECTOR3 dir = { g.CanonicalReal() * (-2.0f*origin.x),g.CanonicalReal() * (-2.0f*origin.y),g.CanonicalReal() * (-2.0f*origin.z) };
		//NVECTOR3 dir = { g.CanonicalReal() * (-origin.x),g.CanonicalReal() * (-origin.y),g.CanonicalReal() * (-origin.z) };
		N_Ray r = N_Ray(origin, dir, 1.0f);
		rayArray.push_back(r);
	}

	for (int rayId = 0; rayId < c_rayCount; ++rayId)
	{
		N_Ray r = rayArray.at(rayId);
		for (int i = 0; i < logicalSphereList.size(); ++i)
		{
			N_RayHitResult hitRes;
			LogicalSphere* pSphere = logicalSphereList.at(i);
			if (CollisionTestor::IntersectRaySphere(r, pSphere, hitRes))
			{
				pGraphicObjBuffer->AddLine3D(r.origin, r.Eval(1.0f), NVECTOR4(1.0f, 0, 0, 1.0f), NVECTOR4(1.0, 0, 0, 1.0f));
			}
			else
			{
				pGraphicObjBuffer->AddLine3D(r.origin, r.Eval(1.0f), NVECTOR4(0, 0, 1.0f, 1.0f), NVECTOR4(0, 0, 1.0f, 1.0f));
			}
		}
	}


	pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 50.0f,0,0 }, { 1.0f,0,0,1.0f }, { 1.0f,0,0,1.0f });
	pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 0,50.0f,0 }, { 0,1.0f,0,1.0f }, { 0,1.0f,0,1.0f });
	pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 0,0,50.0f }, { 0,0,1.0f,1.0f }, { 0,0,1.0f,1.0f });

	//----------------------------------------------------------

	pCamera->LookAt(0, 0, 0);
	pCamera->SetViewAngle_Radian(Ut::PI / 2.5f, 1.333333333f);
	pCamera->SetViewFrustumPlane(1.0f, 500.f);
	//use bounding box of mesh to init camera pos
	N_AABB meshAABB = meshList.at(0)->ComputeWorldAABB_Fast();
	float rotateRadius = sqrtf(meshAABB.max.x*meshAABB.max.x + meshAABB.max.z*meshAABB.max.z)*1.2f;
	float rotateY = meshAABB.max.y*1.3f;
	pCamera->GetWorldTransform().SetPosition(rotateRadius*0.7f, rotateY, rotateRadius*0.7f);
	pCamera->LookAt(0, 0, 0);


	pModelLoader->LoadSkyDome(pAtmos,"Universe", 4.0f, 2.0f);
	//pModelLoader->LoadSkyBox(pAtmos, "Universe", 1000.0f, 1000.0f, 1000.0f);
	pAtmos->SetFogEnabled(false);
	pAtmos->SetFogParameter(50.0f, 100.0f, NVECTOR3(0, 0, 1.0f));

	//¡ª¡ª¡ª¡ª¡ª¡ªµÆ¹â¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª
	pDirLight1 = pLightMgr->CreateDynamicDirLight(pScene->GetSceneGraph().GetRoot(), "myDirLight1");
	N_DirLightDesc dirLightDesc;
	dirLightDesc.ambientColor = NVECTOR3(0.1f, 0.1f, 0.1f);
	dirLightDesc.diffuseColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	dirLightDesc.specularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	dirLightDesc.direction = NVECTOR3(1.0f, -1.0f, 0);
	dirLightDesc.specularIntensity = 1.0f;
	dirLightDesc.diffuseIntensity = 1.0f;
	pDirLight1->SetDesc(dirLightDesc);

	//bottom right
	pGraphicObjBuffer->AddRectangle(NVECTOR2(960.0f, 680.0f), NVECTOR2(1080.0f, 720.0f), NVECTOR4(0.3f, 0.3f, 1.0f, 1.0f), "BottomRightTitle");
	pGraphicObjBuffer->SetBlendMode(NOISE_BLENDMODE_ALPHA);

	return TRUE;
};


void MainLoop()
{
	static float incrNum = 0.0;
	incrNum += 0.001f;
	//pDirLight1->SetDirection(NVECTOR3(sin(incrNum),-1,cos(incrNum)));


	//GUIMgr.Update();
	InputProcess();
	pRenderer->ClearBackground();
	timer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS << "fps :" << timer.GetFPS();// << std::endl;
	pMyText_fps->SetTextAscii(tmpS.str());
	a->GetLocalTransform().Rotate(NVECTOR3(1.0f, 1.0f, 1.0f), 0.001f * timer.GetInterval());
	ac->GetLocalTransform().Rotate(NVECTOR3(1.0f, 1.0f, 1.0f), 0.005f * timer.GetInterval());
	aca->GetLocalTransform().SetScale(NVECTOR3(0.6f + 0.5f* sinf(0.001f * timer.GetTotalTimeElapsed()), 1.0f, 1.0f));
	aca->GetLocalTransform().SetPosition(NVECTOR3(0, 0, 30.0f));

	//add to render list
	for (auto& pMesh : meshList)pRenderer->AddToRenderQueue(pMesh);
	pRenderer->AddToRenderQueue(pGraphicObjBuffer);
	pRenderer->AddToRenderQueue(pMyText_fps);
	pRenderer->SetActiveAtmosphere(pAtmos);
	static N_PostProcessGreyScaleDesc desc;
	desc.factorR = 0.3f;
	desc.factorG = 0.59f;
	desc.factorB = 0.11f;
	//pRenderer->AddToPostProcessList_GreyScale(desc);

	//render
	pRenderer->Render();

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
		NVECTOR3 euler = pCamera->GetWorldTransform().GetEulerAngleZXY();
		euler += NVECTOR3((float)inputE.GetMouseDiffY() / 200.0f, (float)inputE.GetMouseDiffX() / 200.0f, 0);
		pCamera->GetWorldTransform().SetRotation(euler.x, euler.y, euler.z);
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
