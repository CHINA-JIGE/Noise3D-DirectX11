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
	snode_a = sceneRoot->CreateChildNode();
	snode_aa = snode_a->CreateChildNode();
	snode_ab = snode_a->CreateChildNode();
	snode_ac = snode_a->CreateChildNode();
	snode_aca = snode_ac->CreateChildNode();
	snode_b = sceneRoot->CreateChildNode();

	//Âþ·´ÉäÌùÍ¼
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


	//------------------MESH INITIALIZATION----------------

	//pModelLoader->LoadSphere(pMesh1,5.0f, 30, 30);
	pModelLoader = pScene->GetMeshLoader();
	N_SceneLoadingResult res;


	//mesh
	SceneNode* pNodeMesh = sg.GetRoot()->CreateChildNode();
	Mesh* pMesh = pMeshMgr->CreateMesh(pNodeMesh, "mesh" + std::to_string(0));
	pMesh->SetCollidable(true);
	//pModelLoader->LoadFile_OBJ(pMesh, "../media/model/teapot.obj");
	pModelLoader->LoadFile_STL(pMesh, "../media/model/sphere.stl");
	meshList.push_back(pMesh);
	sceneObjectList.push_back(pMesh); // renderable
	pNodeMesh->GetLocalTransform().SetPosition(0, 0, 0);
	//pNodeMesh->GetLocalTransform().SetScale(0.6f, 0.6f, 0.6f);
	//pNodeMesh->GetLocalTransform().SetRotation(1.0f, 1.0f, 0.5f);

	const int c_shapeCount = 0;
	for (int i = 0; i < c_shapeCount; ++i)
	{
		SceneNode* pNodeSphere = sg.GetRoot()->CreateChildNode();
		//bind sphere and visualization mesh
		Mesh* pMeshSphere = pMeshMgr->CreateMesh(pNodeSphere, "sphere" + std::to_string(i));
		pMeshSphere->SetCollidable(false);

		LogicalSphere* pSphere = pShapeMgr->CreateSphere(pNodeSphere, "logicSPH" + std::to_string(i),1.0f);
		pSphere->SetCollidable(true);

		//bind box and visualization mesh
		SceneNode* pNodeBox = sg.GetRoot()->CreateChildNode();
		Mesh* pMeshBox = pMeshMgr->CreateMesh(pNodeBox, "box" + std::to_string(i));
		pMeshBox->SetCollidable(false);

		LogicalBox* pBox = pShapeMgr->CreateBox(pNodeBox, "logicBox" + std::to_string(i),Vec3(1.0f,1.0f,1.0f));
		pBox->SetCollidable(true);


		const float objectRandomRangeScale = 70.0f;
		// **************************************************
		GI::RandomSampleGenerator g1;
		float randomRadius = g1.CanonicalReal() * 10.0f + 3.0f;
		pSphere->SetRadius(randomRadius);
		pModelLoader->LoadSphere(pMeshSphere, randomRadius, 15, 15);
		float randomX = g1.NormalizedReal() * objectRandomRangeScale;
		float randomY = g1.NormalizedReal() * objectRandomRangeScale;
		float randomZ = g1.NormalizedReal() * objectRandomRangeScale;
		pNodeSphere->GetLocalTransform().SetPosition(randomX, randomY, randomZ);
		// **************************************************
		/*pModelLoader->LoadSphere(pMesh, 5.0f, 10, 10);
		pSphere->SetRadius(5.0f);
		pNode->GetLocalTransform().SetPosition(15.0f * i, 0.0f, 0.0f);*/
		// **************************************************
		GI::RandomSampleGenerator g;
		float randomWidthX = g.CanonicalReal() * 10.0f + 3.0f;
		float randomWidthY = g.CanonicalReal() * 10.0f + 3.0f;
		float randomWidthZ = g.CanonicalReal() * 10.0f + 3.0f;
		pBox->SetSizeXYZ(Vec3(randomWidthX, randomWidthY, randomWidthZ));
		pModelLoader->LoadBox(pMeshBox, randomWidthX, randomWidthY, randomWidthZ);

		float randomBoxX = g.NormalizedReal() * objectRandomRangeScale;
		float randomBoxY = g.NormalizedReal() * objectRandomRangeScale;
		float randomBoxZ = g.NormalizedReal() * objectRandomRangeScale;
		pNodeBox->GetLocalTransform().SetPosition(randomBoxX, randomBoxY, randomBoxZ);
		// **************************************************
		/*pModelLoader->LoadBox(pMeshBox, 10.0f, 10.0f, 5.0f);
		pBox->SetSizeXYZ(Vec3(10.0f,10.0f,5.0f));
		pNodeBox->GetLocalTransform().SetPosition(15.0f * i, 0.0f, 0.0f);*/
		// **************************************************

		meshList.push_back(pMeshBox);
		meshList.push_back(pMeshSphere);

		sceneObjectList.push_back(pBox);//not render
		sceneObjectList.push_back(pSphere); // not render
		sceneObjectList.push_back(pMeshBox);//not collidable ,renderable
		sceneObjectList.push_back(pMeshSphere);//not collidable ,renderable

	}

	for (auto& pMesh : meshList)
	{
		pMesh->SetCullMode(NOISE_CULLMODE_NONE);
		//pMesh->SetFillMode(NOISE_FILLMODE_WIREFRAME);
		pMesh->SetFillMode(NOISE_FILLMODE_SOLID);
	}

	//-------Build BVH----------
	//BvhTreeForScene bvh;
	//bvh.Construct(sg);
	//std::vector<BvhNodeForScene*> bvhNodeList;
	pMesh->RebuildBvhTree();
	BvhTreeForTriangularMesh& bvh = pMesh->GetBvhTree();
	std::vector<BvhNodeForTriangularMesh*> bvhNodeList;
	bvh.Traverse_PreOrder(bvhNodeList);

	//-----add Debug AABB--------
	pGraphicObjBuffer = pGraphicObjMgr->CreateGraphicObject("aabb");
	for (auto pNode : bvhNodeList)
	{
		N_AABB aabb= pNode->GetAABB();
		pGraphicObjBuffer->AddLine3D_AABB(aabb.min, aabb.max, Vec4(1.0f, 0.7f, 0.7f, 1.0f));
	}

	//----------------------------------------------------------
	//pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 50.0f,0,0 }, { 1.0f,0,0,1.0f }, { 1.0f,0,0,1.0f });
	//pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 0,50.0f,0 }, { 0,1.0f,0,1.0f }, { 0,1.0f,0,1.0f });
	//pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 0,0,50.0f }, { 0,0,1.0f,1.0f }, { 0,0,1.0f,1.0f });

	pCamera->SetViewAngle_Radian(Ut::PI / 2.5f, 1.333333333f);
	pCamera->SetViewFrustumPlane(1.0f, 500.f);
	//use bounding box of mesh to init camera pos
	pCamera->GetWorldTransform().SetPosition(50.0f, 0, 0);
	pCamera->LookAt(0, 0, 0);


	pModelLoader->LoadSkyDome(pAtmos, "Universe", 4.0f, 2.0f);
	//pModelLoader->LoadSkyBox(pAtmos, "Universe", 1000.0f, 1000.0f, 1000.0f);
	pAtmos->SetFogEnabled(false);
	pAtmos->SetFogParameter(50.0f, 100.0f, Vec3(0, 0, 1.0f));

	//¡ª¡ª¡ª¡ª¡ª¡ªµÆ¹â¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª
	pDirLight1 = pLightMgr->CreateDynamicDirLight(pScene->GetSceneGraph().GetRoot(), "myDirLight1");
	N_DirLightDesc dirLightDesc;
	dirLightDesc.ambientColor = Vec3(0.1f, 0.1f, 0.1f);
	dirLightDesc.diffuseColor = Vec3(1.0f, 1.0f, 1.0f);
	dirLightDesc.specularColor = Vec3(1.0f, 1.0f, 1.0f);
	dirLightDesc.direction = Vec3(1.0f, -1.0f, 0);
	dirLightDesc.specularIntensity = 1.0f;
	dirLightDesc.diffuseIntensity = 1.0f;
	pDirLight1->SetDesc(dirLightDesc);

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
	snode_a->GetLocalTransform().Rotate(Vec3(1.0f, 1.0f, 1.0f), 0.001f * timer.GetInterval());
	snode_ac->GetLocalTransform().Rotate(Vec3(1.0f, 1.0f, 1.0f), 0.005f * timer.GetInterval());
	snode_aca->GetLocalTransform().SetScale(Vec3(0.6f + 0.5f* sinf(0.001f * timer.GetTotalTimeElapsed()), 1.0f, 1.0f));
	snode_aca->GetLocalTransform().SetPosition(Vec3(0, 0, 30.0f));

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
