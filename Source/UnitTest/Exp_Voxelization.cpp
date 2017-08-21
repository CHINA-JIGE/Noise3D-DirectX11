#include "Noise3D.h"
#include <sstream>

bool Init3D(HWND hwnd);
void MainLoop();
void Cleanup();
void	InputProcess();

using namespace Noise3D;

IRoot* pRoot;
IRenderer* pRenderer;
IScene* pScene;
ICamera* pCamera;
IAtmosphere* pAtmos;
IModelLoader* pModelLoader;
IMeshManager* pMeshMgr;
IMesh* pMesh1;
IMesh* pMesh2;
IMaterialManager*	pMatMgr;
ITextureManager*	pTexMgr;
IGraphicObjectManager*	pGraphicObjMgr;
IGraphicObject*	pGraphicObjBuffer;
ILightManager* pLightMgr;
IDirLightD*		pDirLight1;
IPointLightD*	pPointLight1;
ISpotLightD*	pSpotLight1;
IFontManager* pFontMgr;
IDynamicText* pMyText_fps;


Ut::ITimer NTimer(NOISE_TIMER_TIMEUNIT_MILLISECOND);
Ut::IInputEngine inputE;

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

bool Init3D(HWND hwnd)
{
	const UINT bufferWidth = 960;
	const UINT bufferHeight = 720;

	//³õÊ¼»¯Ê§°Ü
	if (!pRoot->InitD3D(hwnd))return false;

	//query pointer to IScene
	pScene = pRoot->GetScenePtr();

	//retrieve meshMgr and Create new mesh
	pMeshMgr = pScene->GetMeshMgr();

	//use "myMesh1" string to initialize UID (unique-Identifier)
	pMesh1 = pMeshMgr->CreateMesh("origin");
	pMesh2 = pMeshMgr->CreateMesh("simplified");

	pRenderer = pScene->CreateRenderer(bufferWidth, bufferHeight, true);
	pCamera = pScene->GetCamera();
	pLightMgr = pScene->GetLightMgr();
	pMatMgr = pScene->GetMaterialMgr();
	pTexMgr = pScene->GetTextureMgr();
	pAtmos = pScene->GetAtmosphere();
	pGraphicObjMgr = pScene->GetGraphicObjMgr();

	//Âþ·´ÉäÌùÍ¼
	pTexMgr->CreateTextureFromFile("media/red.jpg", "Red", true, 1024, 1024, false);
	pTexMgr->CreateTextureFromFile("media/Jade.jpg", "Jade", false, 256, 256, false);
	pTexMgr->CreateTextureFromFile("media/universe2.jpg", "Universe", false, 256, 256, false);
	pTexMgr->CreateTextureFromFile("media/bottom-right-conner-title.jpg", "BottomRightTitle", true, 0, 0, false);

	//create font texture
	pFontMgr = pScene->GetFontMgr();
	pFontMgr->CreateFontFromFile("media/STXINWEI.ttf", "myFont", 24);
	pMyText_fps = pFontMgr->CreateDynamicTextA("myFont", "fpsLabel", "fps:000", 200, 100, NVECTOR4(0, 0, 0, 1.0f), 0, 0);
	pMyText_fps->SetTextColor(NVECTOR4(0, 0.3f, 1.0f, 0.5f));
	pMyText_fps->SetDiagonal(NVECTOR2(20, 20), NVECTOR2(150, 60));
	pMyText_fps->SetFont("myFont");
	pMyText_fps->SetBlendMode(NOISE_BLENDMODE_ALPHA);









	//------------------INIT----------------
	Ut::IVoxelizer voxer;
	Ut::IVoxelizedModel voxModel;
	Ut::IMarchingCubeMeshReconstructor mc;
	const float aaa = 64.0f;
	int sampleCount = 128;
	voxer.Init("model/rabbit.stl", sampleCount, sampleCount, sampleCount, aaa/sampleCount,aaa/sampleCount,aaa/sampleCount);

	pModelLoader->LoadFile_STL(pMesh1, "model/rabbit.stl");
	pMesh1->SetPosition(0, 0, 0);
	pMesh1->SetBlendMode(NOISE_BLENDMODE_ALPHA);
	pMesh1->SetFillMode(NOISE_FILLMODE_WIREFRAME); //NOISE_FILLMODE_WIREFRAME
	pMesh1->SetCullMode(NOISE_CULLMODE_NONE);


	Ut::ITimer tmpTimer(NOISE_TIMER_TIMEUNIT_MILLISECOND);
	std::ofstream logFile("VoxelizationExp.txt", std::ios::app);
	//***********************************************************************
	IModelProcessor* pModelProc = pScene->GetModelProcessor();
	tmpTimer.ResetAll();
	tmpTimer.NextTick();
	voxer.Voxelize();
	tmpTimer.NextTick();
	double interval1 = tmpTimer.GetInterval();
	//************************************************************************
	voxer.GetVoxelizedModel(voxModel);
	int downSampleRate = 30;
	tmpTimer.NextTick();
	mc.Compute(voxModel, downSampleRate, downSampleRate, downSampleRate);
	tmpTimer.NextTick();
	double interval2 = tmpTimer.GetInterval();
	//************************************************************************

	//save voxel model
	//voxModel.SaveToFile_TXT("voxel.txt");

	//visualize result 
	std::vector<NVECTOR3>	vertexList;
	mc.GetResult(vertexList);
	IFileManager fm;
	fm.ExportFile_STL_Binary("out.stl", "MyMCTest", vertexList);
	
	//load the output model (so that normals, tangents are automatically calculated)
	pModelLoader->LoadFile_STL(pMesh2,"out.stl");
	pMesh2->SetPosition(0, 0, 0);
	pMesh2->SetBlendMode(NOISE_BLENDMODE_ALPHA);
	pMesh2->SetFillMode(NOISE_FILLMODE_WIREFRAME); //NOISE_FILLMODE_WIREFRAME
	pMesh2->SetCullMode(NOISE_CULLMODE_NONE);
	pModelProc->WeldVertices(pMesh2, 0.01f);

	logFile << std::endl << "****** " << std::endl
		<< "Input Triangle Count : " << pMesh1->GetTriangleCount() << std::endl
		<< "Voxelize Resolution:" << voxModel.GetVoxelCountX()
		<< "x" << voxModel.GetVoxelCountY()
		<< "x" << voxModel.GetVoxelCountZ() << std::endl
		<< "Time 1(ms) : " << interval1 << std::endl
		<< "DownSample Resolution:" << downSampleRate
		<< "x" << downSampleRate
		<< "x" << downSampleRate << std::endl
		<< "Time 2(ms) : " << interval2 << std::endl
		<< "Output Triangle Count : " << vertexList.size()/3 << std::endl
		<< "Total Time(ms) : " << interval1 + interval2 << std::endl;
	logFile.close();











	const std::vector<N_DefaultVertex>* pTmpVB;
	pTmpVB = pMesh1->GetVertexBuffer();
	pGraphicObjBuffer = pGraphicObjMgr->CreateGraphicObj("Axis");
	pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 20.0f,0,0 }, { 1.0f,1.0f,1.0f,1.0f }, { 1.0f,0,0,0 });
	pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 0,20.0f,0 }, { 1.0f,1.0f,1.0f,1.0f }, { 0,1.0f,0,0 });
	pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 0,0,20.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0,0,1.0f,0 });
	pGraphicObjBuffer->SetBlendMode(NOISE_BLENDMODE_ALPHA);
	/*for (auto v : tmpVB)
	{
	pGraphicObjBuffer->AddLine3D(v.Pos, v.Pos + 5.0f*v.Normal, NVECTOR4(1.0f, 0, 0, 1.0f), NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));//draw the normal
	pGraphicObjBuffer->AddLine3D(v.Pos, v.Pos + 5.0f*v.Tangent, NVECTOR4(0,0, 1.0f, 1.0f), NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));//draw the tangent
	}*/

	//----------------------------------------------------------

	pCamera->SetPosition(2.0f, 0, 0);
	pCamera->SetLookAt(0, 0, 0);
	pCamera->SetViewAngle(MATH_PI / 2.5f, 1.333333333f);
	pCamera->SetViewFrustumPlane(1.0f, 500.f);
	//use bounding box of mesh to init camera pos
	N_Box meshAABB = pMesh1->ComputeBoundingBox();
	float rotateRadius = sqrtf(meshAABB.max.x*meshAABB.max.x + meshAABB.max.z*meshAABB.max.z)*1.2f;
	float rotateY = meshAABB.max.y*1.3f;
	pCamera->SetPosition(rotateRadius*0.7f, rotateY, rotateRadius*0.7f);
	pCamera->SetLookAt(0, 0, 0);

	pAtmos->SetFogEnabled(true);
	//pAtmos->SetFogEnabled(false);
	pAtmos->SetFogParameter(1.0f, 2.0f, NVECTOR3(0, 0, 1.0f));
	pAtmos->CreateSkyDome(4.0f, 4.0f, "Universe");


	//------------------µÆ¹â-----------------
	pDirLight1 = pLightMgr->CreateDynamicDirLight("myDirLight1");
	N_DirLightDesc dirLightDesc;
	dirLightDesc.ambientColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	dirLightDesc.diffuseColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	dirLightDesc.specularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	dirLightDesc.mDirection = NVECTOR3(0, 0, 0);//just init
	dirLightDesc.specularIntensity = 1.0f;
	dirLightDesc.diffuseIntensity = 1.0f;
	pDirLight1->SetDesc(dirLightDesc);

	//-------------------²ÄÖÊ----------------

	N_MaterialDesc mat;
	mat.ambientColor = NVECTOR3(0.3f, 0.3f, 0.3f);
	mat.diffuseColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	mat.specularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	mat.specularSmoothLevel = 40;
	mat.normalMapBumpIntensity = 0.2f;
	mat.environmentMapTransparency = 0.05f;
	mat.transparency = 1.0f;
	mat.diffuseMapName = "Red";
	pMatMgr->CreateMaterial("meshMat1", mat);
	//set material
	pMesh1->SetMaterial("meshMat1");
	pMesh2->SetMaterial("meshMat1");



	//bottom right
	pGraphicObjBuffer->AddRectangle(NVECTOR2(800.0, 680.0f), NVECTOR2(960.0f, 720.0f), NVECTOR4(0.3f, 0.3f, 1.0f, 1.0f), "BottomRightTitle");
	pGraphicObjBuffer->SetBlendMode(NOISE_BLENDMODE_ADDITIVE);

	return true;
};


void MainLoop()
{
	static float incrNum = 0.0;
	incrNum += 0.001f;
	pDirLight1->SetDirection(NVECTOR3(sin(incrNum), -1.0f, cos(incrNum)));

	//GUIMgr.Update();
	InputProcess();
	pRenderer->ClearBackground();
	NTimer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS << "fps :" << NTimer.GetFPS() << std::endl;
	pMyText_fps->SetTextAscii(tmpS.str());


	//add to render list
	//pRenderer->AddObjectToRenderList(pMesh1);
	pRenderer->AddObjectToRenderList(pMesh2);
	pRenderer->AddObjectToRenderList(pGraphicObjBuffer);
	pRenderer->AddObjectToRenderList(pAtmos);
	pRenderer->AddObjectToRenderList(pMyText_fps);

	//render
	pRenderer->RenderAtmosphere();
	pRenderer->RenderMeshes();
	pRenderer->RenderGraphicObjects();
	pRenderer->RenderTexts();

	//present
	pRenderer->PresentToScreen();
};

void InputProcess()
{
	inputE.Update();

	if (inputE.IsKeyPressed(Ut::NOISE_KEY_A))
	{
		pCamera->fps_MoveRight(-0.1f, false);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_D))
	{
		pCamera->fps_MoveRight(0.1f, false);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_W))
	{
		pCamera->fps_MoveForward(0.1f, false);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_S))
	{
		pCamera->fps_MoveForward(-0.1f, false);
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
		pCamera->RotateY_Yaw((float)inputE.GetMouseDiffX() / 200.0f);
		pCamera->RotateX_Pitch((float)inputE.GetMouseDiffY() / 200.0f);
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
