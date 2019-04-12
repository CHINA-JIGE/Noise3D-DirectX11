//**created in 2019.4.12***

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
std::vector<ISceneObject*> sceneObjectList;
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
SceneNode* snode_a;
SceneNode* snode_aa;
SceneNode* snode_ab;
SceneNode* snode_ac;
SceneNode* snode_aca;
SceneNode* snode_b;
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
	const UINT bufferWidth = 1080;
	const UINT bufferHeight = 720;

	//³õÊ¼»¯Ê§°Ü
	if (!pRoot->Init())return FALSE;

	//query pointer to IScene
	pScene = pRoot->GetSceneMgrPtr();

	//retrieve meshMgr and Create new mesh
	pMeshMgr = pScene->GetMeshMgr();

	pRenderer = pScene->CreateRenderer(bufferWidth, bufferHeight, hwnd);
	pCamera = pScene->GetCamera();
	pLightMgr = pScene->GetLightMgr();
	pMatMgr = pScene->GetMaterialMgr();
	pTexMgr = pScene->GetTextureMgr();
	pAtmos = pScene->GetAtmosphere();
	pGraphicObjMgr = pScene->GetGraphicObjMgr();
	pShapeMgr = pScene->GetLogicalShapeMgr();
	pModelLoader = pScene->GetMeshLoader();

	//diffuse map
	pTexMgr->CreateTextureFromFile("../media/earth.jpg", "Earth", TRUE, 1024, 1024, FALSE);
	//pTexMgr->CreateTextureFromFile("../media/Jade.jpg", "Jade", FALSE, 256, 256, FALSE);
	//pTexMgr->CreateTextureFromFile("../media/universe.jpg", "Universe", FALSE, 256, 256, FALSE);
	pTexMgr->CreateTextureFromFile("../media/white.jpg", "Universe", FALSE, 256, 256, FALSE);
	//pTexMgr->CreateCubeMapFromDDS("../media/CubeMap/cube-room.dds", "Universe", FALSE);
	pTexMgr->CreateTextureFromFile("../media/noise3d.png", "BottomRightTitle", TRUE, 0, 0, FALSE);
	//pTexMgr->CreateCubeMapFromDDS("../media/UniverseEnv.dds", "AtmoTexture");
	ITexture* pNormalMap = pTexMgr->CreateTextureFromFile("../media/earth-normal.png", "EarthNormalMap", FALSE, 512, 512, TRUE);
	//pNormalMap->ConvertTextureToGreyMap();
	//pNormalMap->ConvertHeightMapToNormalMap(10.0f);


	//create font texture
	pTextMgr = pScene->GetTextMgr();
	pTextMgr->CreateFontFromFile("../media/calibri.ttf", "myFont", 24);
	pMyText_fps = pTextMgr->CreateDynamicTextA("myFont", "fpsLabel", "fps:000", 200, 100, Vec4(0, 0, 0, 1.0f), 0, 0);
	pMyText_fps->SetTextColor(Vec4(0, 0.3f, 1.0f, 0.5f));
	pMyText_fps->SetDiagonal(Vec2(20, 20), Vec2(170, 60));
	pMyText_fps->SetFont("myFont");
	pMyText_fps->SetBlendMode(NOISE_BLENDMODE_ALPHA);

	//-----Generate Rays--------
	pGraphicObjBuffer = pGraphicObjMgr->CreateGraphicObject("rays");
	const int c_rayCount = 100;
	const int c_clusterCount = 10;
	for (int i = 0; i < c_clusterCount; ++i)
	{
		Vec3 origin = { float(i) * 30.0f , 0, 0 };
		Vec3 normal = Vec3(std::cosf(i), std::sinf(i), 0);
		N_Ray centerRay = N_Ray(origin, normal * 20.0f, 0.001f, 1.0f);
		pGraphicObjBuffer->AddLine3D(centerRay.origin, centerRay.Eval(1.0f), Vec4(1.0f, 0, 0, 1.0f), Vec4(1.0f, 0, 0, 1.0f));
		GI::RandomSampleGenerator g;

		for (int rayId = 0; rayId < c_rayCount; ++rayId)
		{
			Vec3 dir = g.UniformSphericalVec_Cone(normal, Ut::PI / 4.0f);
			float len = dir.Length();
			if (len != 1.0f)
			{
				WARNING_MSG("?asd");
			}
			N_Ray r = N_Ray(origin, dir * 20.0f, 0.001f, 1.0f);
			pGraphicObjBuffer->AddLine3D(r.origin, r.Eval(1.0f), Vec4(0, 0, 1.0f, 1.0f), Vec4(0, 0, 1.0f, 1.0f));
		}
	}

	//pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 50.0f,0,0 }, { 1.0f,0,0,1.0f }, { 1.0f,0,0,1.0f });
	//pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 0,50.0f,0 }, { 0,1.0f,0,1.0f }, { 0,1.0f,0,1.0f });
	//pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 0,0,50.0f }, { 0,0,1.0f,1.0f }, { 0,0,1.0f,1.0f });

	//----------------------------------------------------------

	pCamera->SetViewAngle_Radian(Ut::PI / 2.5f, 1.333333333f);
	pCamera->SetViewFrustumPlane(1.0f, 500.f);
	//use bounding box of mesh to init camera pos
	pCamera->GetWorldTransform().SetPosition(50.0f, 0, 0);
	pCamera->LookAt(0, 0, 0);


	pModelLoader->LoadSkyDome(pAtmos, "Universe", 4.0f, 2.0f);
	//pModelLoader->LoadSkyBox(pAtmos, "Universe", 1000.0f, 1000.0f, 1000.0f);
	pAtmos->SetFogEnabled(false);
	pAtmos->SetFogParameter(50.0f, 100.0f, Vec3(0, 0, 1.0f));

	//bottom right
	pGraphicObjBuffer->AddRectangle(Vec2(870.0f, 680.0f), Vec2(1080.0f, 720.0f), Vec4(0.3f, 0.3f, 1.0f, 1.0f), "BottomRightTitle");
	pGraphicObjBuffer->SetBlendMode(NOISE_BLENDMODE_ALPHA);

	return TRUE;
};


void MainLoop()
{
	static float incrNum = 0.0;
	incrNum += 0.001f;
	//pDirLight1->SetDirection(Vec3(sin(incrNum),-1,cos(incrNum)));


	//GUIMgr.Update();
	InputProcess();
	pRenderer->ClearBackground();
	timer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS << "fps :" << timer.GetFPS();// << std::endl;
	pMyText_fps->SetTextAscii(tmpS.str());

	//add to render list
	for (auto& pMesh : meshList)pRenderer->AddToRenderQueue(pMesh);
	pRenderer->AddToRenderQueue(pGraphicObjBuffer);
	pRenderer->AddToRenderQueue(pMyText_fps);
	pRenderer->SetActiveAtmosphere(pAtmos);

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
		Vec3 euler = pCamera->GetWorldTransform().GetEulerAngleZXY();
		euler += Vec3((float)inputE.GetMouseDiffY() / 200.0f, (float)inputE.GetMouseDiffX() / 200.0f, 0);
		euler.x = Ut::Clamp(euler.x, -Ut::PI / 2.0f + 0.001f, Ut::PI / 2.0f - 0.001f);
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

};
