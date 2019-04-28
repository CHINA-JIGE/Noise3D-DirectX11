#include "Noise3D.h"
#include <sstream>
#include <iostream>

BOOL Init3D(HWND hwnd);
using namespace Noise3D;

Root* pRoot;
Renderer* pRenderer;
SceneManager* pScene;
MeshLoader* pModelLoader;
MeshManager* pMeshMgr;
CollisionTestor* pCT;

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
	pRenderer = pScene->CreateRenderer(640,480,hwnd);
	pCT = pScene->GetCollisionTestor();

	//------------------MESH INITIALIZATION----------------

	//pModelLoader->LoadSphere(pMesh1,5.0f, 30, 30);
	pModelLoader = pScene->GetMeshLoader();
	N_SceneLoadingResult res;

	//mesh
	SceneGraph& sg = pScene->GetSceneGraph();
	SceneNode* pNodeMesh = sg.GetRoot()->CreateChildNode();
	Mesh* pMesh = pMeshMgr->CreateMesh(pNodeMesh, "mesh" + std::to_string(0));
	pMesh->SetCollidable(true);
	//pModelLoader->LoadFile_STL(pMesh, "../media/model/buddha.stl");
	pModelLoader->LoadFile_STL(pMesh, "../media/model/sphere.stl");
	//pModelLoader->LoadFile_OBJ(pMesh, "../media/model/teapot.obj");
	pNodeMesh->GetLocalTransform().SetPosition(0, 0, 0);
	pNodeMesh->GetLocalTransform().SetScale(1.0f, 1.0f, 1.0f);
	pMesh->RebuildBvhTree();

	//-----Generate Rays--------
	std::vector<N_Ray> rayArray;
	const int c_rayCount = 10000;
	for (int rayId = 0; rayId < c_rayCount; ++rayId)
	{
		Vec3 origin = Vec3(-100.0f, 0, 0);
		Vec3 dir = Vec3(200.0f, 0, 0);
		N_Ray r = N_Ray(origin, dir,  0.001f,10.0f);
		rayArray.push_back(r);
	}

	std::cout << "start..." << std::endl;

	timer.ResetAll();
	N_RayHitResult hitRes;//ground truth

	//pRenderer->ClearBackground();
	timer.NextTick();
	for (int i = 0; i < c_rayCount; ++i)
	{
		N_RayHitResult tmpHitRes;
		//bool intersect = pCT->IntersectRayMesh_GpuBased(rayArray.at(i), pMesh, tmpHitRes);
		bool intersect = pCT->IntersectRayMesh(rayArray.at(i), pMesh, tmpHitRes);
		hitRes.Union(tmpHitRes);
	}
	timer.NextTick();
	std::string str0 = "***linear traverse ray-mesh intersection(ground truth)\n";
	std::string str1 = "ray:" + std::to_string(c_rayCount) + "\n";
	std::string str2 = "triangle count:" + std::to_string(pMesh->GetTriangleCount()) + "\n";
	std::string str3 = "hitResult:" + std::to_string(hitRes.hitList.size()) + "\n";
	std::string str4 = "time:" + std::to_string(timer.GetTotalTimeElapsed()) + "\n";
	OutputDebugStringA(str0.c_str());
	OutputDebugStringA(str1.c_str());
	OutputDebugStringA(str2.c_str());
	OutputDebugStringA(str3.c_str());
	OutputDebugStringA(str4.c_str());

	timer.ResetAll();
	timer.NextTick();
	N_RayHitResult hitRes2;

	for (int i = 0; i < c_rayCount; ++i)
	{
		N_RayHitResult tmpHitRes2;
		bool intersect = pCT->IntersectRayMeshWithBvh(rayArray.at(i), pMesh, tmpHitRes2);
		hitRes2.Union(tmpHitRes2);
	}
	timer.NextTick();

	str0 = "***BVH accelerated ray-mesh intersection\n";
	str1 = "ray:" + std::to_string(c_rayCount) +"\n";
	str2 = "triangle count:" + std::to_string(pMesh->GetTriangleCount()) + "\n";
	str3 = "hitResult:" + std::to_string(hitRes.hitList.size()) + "\n";
	str4 = "time:" + std::to_string(timer.GetTotalTimeElapsed()) + "\n";
	OutputDebugStringA(str0.c_str());
	OutputDebugStringA(str1.c_str());
	OutputDebugStringA(str2.c_str());
	OutputDebugStringA(str3.c_str());
	OutputDebugStringA(str4.c_str());

	return true;
};
