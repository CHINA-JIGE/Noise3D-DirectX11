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
IMesh* pMesh1WF;
IMesh* pMeshSimplified;
IMesh* pMeshVox;
IMesh* pMeshVoxFrame;
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


Ut::ITimer myTimer(NOISE_TIMER_TIMEUNIT_MILLISECOND);
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
	pMesh1WF = pMeshMgr->CreateMesh("originWF");
	pMeshSimplified = pMeshMgr->CreateMesh("simplified");
	pMeshVox = pMeshMgr->CreateMesh("voxelized");
	pMeshVoxFrame = pMeshMgr->CreateMesh("voxFrame");

	pRenderer = pScene->CreateRenderer(bufferWidth, bufferHeight, true);
	pCamera = pScene->GetCamera();
	pLightMgr = pScene->GetLightMgr();
	pMatMgr = pScene->GetMaterialMgr();
	pTexMgr = pScene->GetTextureMgr();
	pAtmos = pScene->GetAtmosphere();
	pGraphicObjMgr = pScene->GetGraphicObjMgr();

	//Âþ·´ÉäÌùÍ¼
	pTexMgr->CreateTextureFromFile("../media/wood.jpg", "wood", true, 1024, 1024, false);
	pTexMgr->CreateTextureFromFile("../media/Jade.jpg", "Jade", false, 256, 256, false);
	pTexMgr->CreateTextureFromFile("../media/white.jpg", "Universe", false, 256, 256, false);
	pTexMgr->CreateTextureFromFile("../media/bottom-right-conner-title.jpg", "BottomRightTitle", true, 0, 0, false);

	//create font texture
	pFontMgr = pScene->GetFontMgr();
	pFontMgr->CreateFontFromFile("../media/STXINWEI.ttf", "myFont", 24);
	pMyText_fps = pFontMgr->CreateDynamicTextA("myFont", "fpsLabel", "fps:000", 200, 100, Vec4(0, 0, 0, 1.0f), 0, 0);
	pMyText_fps->SetTextColor(Vec4(0, 0.3f, 1.0f, 0.5f));
	pMyText_fps->SetDiagonal(Vec2(20, 20), Vec2(150, 60));
	pMyText_fps->SetFont("myFont");
	pMyText_fps->SetBlendMode(NOISE_BLENDMODE_ALPHA);









	//*********************PARAM SETTING**************************
	Ut::IVoxelizer voxer;
	Ut::IVoxelizedModel voxModel;
	Ut::IMarchingCubeMeshReconstructor mc;
	int sampleCountX = 300;
	int sampleCountY = 300;
	int sampleCountZ = 300;
	std::string modelFile = "../model/dragon.stl";
	voxer.Init(modelFile, sampleCountX, sampleCountY, sampleCountZ);

	pModelLoader->LoadFile_STL(pMesh1, modelFile);
	pMesh1->SetPosition(-200.0f, 0, 0);
	pMesh1->SetBlendMode(NOISE_BLENDMODE_ALPHA);
	pMesh1->SetFillMode(NOISE_FILLMODE_SOLID); //NOISE_FILLMODE_WIREFRAME
	pMesh1->SetCullMode(NOISE_CULLMODE_NONE);
	/*pModelLoader->LoadFile_STL(pMesh1WF, modelFile);
	pMesh1WF->SetPosition(-200.0f, 0, 0);
	pMesh1WF->SetBlendMode(NOISE_BLENDMODE_ALPHA);
	pMesh1WF->SetFillMode(NOISE_FILLMODE_WIREFRAME); 
	pMesh1WF->SetCullMode(NOISE_CULLMODE_NONE);*/

	Ut::ITimer tmpTimer(NOISE_TIMER_TIMEUNIT_MILLISECOND);
	std::ofstream logFile("VoxelizationExp.txt", std::ios::app);
	//***********************************************************************
	IModelProcessor* pModelProc = pScene->GetModelProcessor();
	//pModelProc->WeldVertices(pMesh1, 0.5f);
	tmpTimer.ResetAll();
	voxer.Voxelize();
	tmpTimer.NextTick();
	double interval1 = tmpTimer.GetInterval();
	//************************************************************************

	tmpTimer.ResetAll();
	voxer.GetVoxelizedModel(voxModel);
	int downSampleRateX = 100;
	int downSampleRateY = 100;
	int downSampleRateZ = 100;
	mc.Compute(voxModel, downSampleRateX, downSampleRateY, downSampleRateZ);
	tmpTimer.NextTick();
	double interval2 = tmpTimer.GetInterval();
	//************************************************************************


	//visualize result 
	std::vector<Vec3>	vertexList;
	mc.GetResult(vertexList);
	IFileIO fm;
	fm.ExportFile_STL_Binary("out.stl", "MyMCTest", vertexList);
	
	//load the output model (so that normals, tangents are automatically calculated)
	pModelLoader->LoadFile_STL(pMeshSimplified,"out.stl");
	pMeshSimplified->SetPosition(-0, 0, 0);
	pMeshSimplified->SetBlendMode(NOISE_BLENDMODE_ALPHA);
	pMeshSimplified->SetFillMode(NOISE_FILLMODE_SOLID); //NOISE_FILLMODE_WIREFRAME
	pMeshSimplified->SetCullMode(NOISE_CULLMODE_NONE);
	//pModelProc->WeldVertices(pMeshSimplified, 0.3f);

	//save voxel model
	/*voxModel.SaveToFile_STL("voxelizedMesh.stl");
	pModelLoader->LoadFile_STL(pMeshVox, "voxelizedMesh.stl");
	pMeshVox->SetPosition(0.0f, 0, 0);
	pMeshVox->SetScale(1.0f, 1.0f, 1.0f);
	pMeshVox->SetBlendMode(NOISE_BLENDMODE_ALPHA);
	pMeshVox->SetFillMode(NOISE_FILLMODE_SOLID); //NOISE_FILLMODE_WIREFRAME
	pMeshVox->SetCullMode(NOISE_CULLMODE_NONE);

	pModelLoader->LoadFile_STL(pMeshVoxFrame, "voxelizedMesh.stl");
	pMeshVoxFrame->SetPosition(0, 0, 0);
	pMeshVoxFrame->SetScale(1.5f, 1.0f, 1.0f);
	pMeshVoxFrame->SetBlendMode(NOISE_BLENDMODE_ALPHA);
	pMeshVoxFrame->SetFillMode(NOISE_FILLMODE_WIREFRAME); //NOISE_FILLMODE_WIREFRAME
	pMeshVoxFrame->SetCullMode(NOISE_CULLMODE_NONE);*/

#define ENABLE_LOG
#ifdef ENABLE_LOG
	logFile << std::endl << "****** " << std::endl
		<<"Model File : " << modelFile<<std::endl
		<< "Input Triangle Count : " << pMesh1->GetTriangleCount() << std::endl
		<< "Voxelize Resolution: X:" << voxModel.GetVoxelCountX()
		<< " Y:" << voxModel.GetVoxelCountY()
		<< " Z:" << voxModel.GetVoxelCountZ() << std::endl
		<< "Time 1(ms) : " << interval1 << std::endl
		<< "DownSample Resolution: X:" << downSampleRateX
		<< " Y:" << downSampleRateY
		<< " Z:" << downSampleRateZ << std::endl
		<< "Time 2(ms) : " << interval2 << std::endl
		<< "Output Triangle Count : " << vertexList.size()/3 << std::endl
		<<"Simplified Percentage: " << (vertexList.size() / 3.0f)/float(pMesh1->GetTriangleCount())<<std::endl
		<< "Total Time(ms) : " << interval1 + interval2 << std::endl;
	logFile.close();
#endif












	const std::vector<N_DefaultVertex>* pTmpVB;
	pTmpVB = pMeshSimplified->GetVertexBuffer();
	pGraphicObjBuffer = pGraphicObjMgr->CreateGraphicObj("Axis");
	/*pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 20.0f,0,0 }, { 1.0f,1.0f,1.0f,1.0f }, { 1.0f,0,0,0 });
	pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 0,20.0f,0 }, { 1.0f,1.0f,1.0f,1.0f }, { 0,1.0f,0,0 });
	pGraphicObjBuffer->AddLine3D({ 0,0,0 }, { 0,0,20.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0,0,1.0f,0 });*/

	/*Vec3 modelPos = pMeshSimplified->GetPosition();
	for (auto v : *pTmpVB)
	{
	pGraphicObjBuffer->AddLine3D(modelPos + v.Pos, modelPos+ v.Pos + 5.0f * v.Normal, Vec4(1.0f, 0, 0, 1.0f), Vec4(1.0f, 1.0f, 1.0f, 1.0f));//draw the normal
	pGraphicObjBuffer->AddLine3D(modelPos + v.Pos, modelPos + v.Pos + 5.0f* v.Tangent, Vec4(0,0, 1.0f, 1.0f), Vec4(1.0f, 1.0f, 1.0f, 1.0f));//draw the tangent
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

	//pAtmos->SetFogEnabled(true);
	pAtmos->SetFogEnabled(false);
	pAtmos->SetFogParameter(1.0f, 2.0f, Vec3(0, 0, 1.0f));
	pAtmos->CreateSkyDome(4.0f, 4.0f, "Universe");


	//------------------µÆ¹â-----------------
	pDirLight1 = pLightMgr->CreateDynamicDirLight("myDirLight1");
	N_DirLightDesc dirLightDesc;
	dirLightDesc.ambientColor = Vec3(1.0f, 1.0f, 1.0f);
	dirLightDesc.diffuseColor = Vec3(1.0f, 1.0f, 1.0f);
	dirLightDesc.specularColor = Vec3(1.0f,1.0f,1.0f);
	dirLightDesc.direction = Vec3(0, 0, 0);//just init
	dirLightDesc.specularIntensity = 1.0f;
	dirLightDesc.diffuseIntensity = 1.0f;
	pDirLight1->SetDesc(dirLightDesc);

	//-------------------²ÄÖÊ----------------

	N_MaterialDesc mat;
	mat.ambientColor = Vec3(0.3f, 0.3f, 0.3f);
	mat.diffuseColor = Vec3(0.8f,0.8f,0.8f);
	mat.specularColor = Vec3(1.0f, 1.0f, 1.0f);
	mat.specularSmoothLevel = 30;
	mat.normalMapBumpIntensity = 0.2f;
	mat.environmentMapTransparency = 0.05f;
	mat.transparency = 1.0f;
	//mat.diffuseMapName = "Jade";
	pMatMgr->CreateMaterial("meshMat1", mat);
	//set material
	pMesh1->SetMaterial("meshMat1");
	pMeshSimplified->SetMaterial("meshMat1");
	pMeshVox->SetMaterial("meshMat1");

	//bottom right
	pGraphicObjBuffer->AddRectangle(Vec2(800.0, 680.0f), Vec2(960.0f, 720.0f), Vec4(0.3f, 0.3f, 1.0f, 1.0f), "BottomRightTitle");
	pGraphicObjBuffer->SetBlendMode(NOISE_BLENDMODE_ALPHA);

	return true;
};


void MainLoop()
{
	static float incrNum = 0.0;
	incrNum += 0.001f;
	//pDirLight1->SetDirection(Vec3(sin(incrNum), -1.0f, 1.0f));
	pDirLight1->SetDirection(Vec3(-1.0f, -1.0f, 1.0f));

	//GUIMgr.Update();
	InputProcess();
	pRenderer->ClearBackground();
	myTimer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS << "fps :" << myTimer.GetFPS() << std::endl;
	pMyText_fps->SetTextAscii(tmpS.str());


	//add to render list
	pRenderer->AddObjectToRenderList(pMesh1);
	//pRenderer->AddObjectToRenderList(pMesh1WF);
	pRenderer->AddObjectToRenderList(pMeshSimplified);
	//pRenderer->AddObjectToRenderList(pMeshVox);
	//pRenderer->AddObjectToRenderList(pMeshVoxFrame);
	pRenderer->AddObjectToRenderList(pGraphicObjBuffer);
	pRenderer->AddObjectToRenderList(pAtmos);
	pRenderer->AddObjectToRenderList(pMyText_fps);

	//render
	pRenderer->RenderAtmosphere();
	pRenderer->RenderMeshes();
	pRenderer->RenderGraphicObjects();
	pRenderer->RenderTexts();

	pRenderer->PresentToScreen();
};

void InputProcess()
{
	inputE.Update();

	float moveSpeed = float(0.06 * myTimer.GetInterval());
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_A))
	{
		pCamera->fps_MoveRight(-moveSpeed, false);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_D))
	{
		pCamera->fps_MoveRight(moveSpeed, false);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_W))
	{
		pCamera->fps_MoveForward(moveSpeed, false);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_S))
	{
		pCamera->fps_MoveForward(-moveSpeed, false);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_SPACE))
	{
		pCamera->fps_MoveUp(moveSpeed);
	}
	if (inputE.IsKeyPressed(Ut::NOISE_KEY_LCONTROL))
	{
		pCamera->fps_MoveUp(-moveSpeed);
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
