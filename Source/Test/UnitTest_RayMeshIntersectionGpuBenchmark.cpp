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
	pModelLoader->LoadFile_OBJ(pMesh, "../media/model/teapot.obj");
	meshList.push_back(pMesh);
	sceneObjectList.push_back(pMesh); // renderable
	pNodeMesh->GetLocalTransform().SetPosition(150.0f, 0, 0);
	pNodeMesh->GetLocalTransform().SetScale(0.3f, 0.3f, 0.3f);
	pNodeMesh->GetLocalTransform().SetRotation(1.0f, 1.0f, 0.5f);

	const int c_shapeCount = 10;
	for (int i = 0; i < c_shapeCount; ++i)
	{
		SceneNode* pNodeSphere = sg.GetRoot()->CreateChildNode();
		//bind sphere and visualization mesh
		Mesh* pMeshSphere = pMeshMgr->CreateMesh(pNodeSphere, "sphere" + std::to_string(i));
		pMeshSphere->SetCollidable(false);

		LogicalSphere* pSphere = pShapeMgr->CreateSphere(pNodeSphere, "logicSPH" + std::to_string(i), 1.0f);
		pSphere->SetCollidable(true);

		//bind box and visualization mesh
		SceneNode* pNodeBox = sg.GetRoot()->CreateChildNode();
		Mesh* pMeshBox = pMeshMgr->CreateMesh(pNodeBox, "box" + std::to_string(i));
		pMeshBox->SetCollidable(false);

		LogicalBox* pBox = pShapeMgr->CreateBox(pNodeBox, "logicBox" + std::to_string(i), Vec3(1.0f, 1.0f, 1.0f));
		pBox->SetCollidable(true);


		const float objectRandomRangeScale = 100.0f;
		pModelLoader->LoadBox(pMeshBox, 10.0f, 10.0f, 5.0f);
		pBox->SetSizeXYZ(Vec3(10.0f, 10.0f, 5.0f));
		pNodeBox->GetLocalTransform().SetPosition(15.0f * i, 0.0f, 0.0f);
		// **************************************************

		meshList.push_back(pMeshBox);
		//meshList.push_back(pMeshSphere);

		sceneObjectList.push_back(pBox);//not render
										//sceneObjectList.push_back(pSphere); // not render
		sceneObjectList.push_back(pMeshBox);//not collidable ,renderable
											//sceneObjectList.push_back(pMeshSphere);//not collidable ,renderable

	}

	for (auto& pMesh : meshList)
	{
		pMesh->SetCullMode(NOISE_CULLMODE_NONE);
		//pMesh->SetFillMode(NOISE_FILLMODE_WIREFRAME);
		pMesh->SetFillMode(NOISE_FILLMODE_SOLID);
	}

	//-----Generate Rays--------
	std::vector<N_Ray> rayArray;
	pGraphicObjBuffer = pGraphicObjMgr->CreateGraphicObject("rays");
	const int c_rayCount = 500;
	for (int rayId = 0; rayId < c_rayCount; ++rayId)
	{
		// **************************************************
		/*GI::RandomSampleGenerator g;
		const float randomRangeScale = 150.0f;
		Vec3 origin = { g.NormalizedReal() * randomRangeScale,g.NormalizedReal() * randomRangeScale,g.NormalizedReal() * randomRangeScale };
		Vec3 dir = { g.CanonicalReal() * (-origin.x),g.CanonicalReal() * (-origin.y),g.CanonicalReal() * (-origin.z) };
		N_Ray r = N_Ray(origin, dir, 0.001f,1.0f);*/
		//**************************************************
		/*Vec3 origin = { rayId * 3.35f , 0, 50.0f };
		Vec3 dir = { 0,0,-100.0f };
		N_Ray r = N_Ray(origin, dir, 0.001f,1.0f);*/
		// **************************************************
		/*Vec3 origin = { -10.0f , -10.0f, -10.0f };
		Vec3 dir = { 20.0f,5.0f,50.0f };
		N_Ray r = N_Ray(origin, dir,  0.001f,1.0f);*/
		// **************************************************
		//rayArray.push_back(r);
	}

	GI::RandomSampleGenerator g;
	Vec3 origin = { 0,0,-40.0f };
	std::vector<Vec3> dirList;
	std::vector<float> pdfList;
	ISceneObject* pObj = pShapeMgr->GetObjectPtr<LogicalBox>("logicBox5");
	//g.UniformSphericalVec_ShadowRays(origin, pObj, c_rayCount, dirList);
	g.CosinePdfSphericalVec_Cone(Vec3(0, 1, 0), Ut::PI / 2.0f, c_rayCount, dirList, pdfList);
	for (int rayId = 0; rayId < c_rayCount; ++rayId)
	{
		N_Ray r = N_Ray(origin, dirList.at(rayId)*100.0f, 0.001f, 1.0f);
		rayArray.push_back(r);
	}




	std::vector<bool> anyHit_List(c_rayCount, false);
	for (int rayId = 0; rayId < c_rayCount; ++rayId)
	{
		N_Ray r = rayArray.at(rayId);
		for (int i = 0; i < sceneObjectList.size(); ++i)
		{
			N_RayHitResult hitRes;

			ISceneObject* pShape = sceneObjectList.at(i);
			switch (pShape->GetObjectType())
			{
			case NOISE_SCENE_OBJECT_TYPE::LOGICAL_SPHERE:
			{
				LogicalSphere* pSphere = dynamic_cast<LogicalSphere*>(sceneObjectList.at(i));
				if (CollisionTestor::IntersectRaySphere(r, pSphere, hitRes))anyHit_List.at(rayId) = true;
				for (auto h : hitRes.hitList)pGraphicObjBuffer->AddLine3D(h.pos, h.pos + h.normal *5.0f, Vec4(1.0f, 0, 1.0f, 1.0f), Vec4(1.0f, 0, 1.0f, 1.0f));
				break;
			}
			case NOISE_SCENE_OBJECT_TYPE::LOGICAL_BOX:
			{
				LogicalBox* pBox = dynamic_cast<LogicalBox*>(sceneObjectList.at(i));
				if (CollisionTestor::IntersectRayBox(r, pBox, hitRes))anyHit_List.at(rayId) = true;
				for (auto h : hitRes.hitList)pGraphicObjBuffer->AddLine3D(h.pos, h.pos + h.normal *5.0f, Vec4(1.0f, 0, 1.0f, 1.0f), Vec4(1.0f, 0, 1.0f, 1.0f));
				break;
			}
			case NOISE_SCENE_OBJECT_TYPE::MESH:
			{
				Mesh* pMesh = dynamic_cast<Mesh*>(sceneObjectList.at(i));
				if (CollisionTestor::IntersectRayMesh(r, pMesh, hitRes))anyHit_List.at(rayId) = true;
				for (auto h : hitRes.hitList)pGraphicObjBuffer->AddLine3D(h.pos, h.pos + h.normal *5.0f, Vec4(1.0f, 0, 1.0f, 1.0f), Vec4(1.0f, 0, 1.0f, 1.0f));
				break;
			}
			}

		}
	}

	//add ray's line3D
	for (int rayId = 0; rayId<c_rayCount; ++rayId)
	{
		N_Ray r = rayArray.at(rayId);
		if (anyHit_List.at(rayId))
		{
			pGraphicObjBuffer->AddLine3D(r.origin, r.Eval(1.0f), Vec4(1.0f, 0, 0, 1.0f), Vec4(1.0f, 0, 0, 1.0f));
		}
		else
		{
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
