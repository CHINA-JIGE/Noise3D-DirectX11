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
LogicalShapeManager* pShapeMgr;
CollisionTestor* pCT;

Ut::Timer timer(Ut::NOISE_TIMER_TIMEUNIT_MILLISECOND);
Ut::InputEngine inputE;


//Main Entry
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
int main()
{

	//get Root interface
	pRoot = GetRoot();

	//create a window (using default window creation function)
	HWND windowHWND = NULL;
	//windowHWND = pRoot->CreateRenderWindow(960, 720, L"Hahaha Render Window", hInstance);

	//initialize input engine (detection for keyboard and mouse input)
	//inputE.Initialize(hInstance, windowHWND);

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
	//pRenderer = pScene->CreateRenderer(640,480,hwnd);
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
	pModelLoader->LoadFile_STL(pMesh, "../media/model/buddha.stl");
	//pModelLoader->LoadFile_STL(pMesh, "../media/model/sphere.stl");
	//pModelLoader->LoadFile_OBJ(pMesh, "../media/model/teapot.obj");
	pNodeMesh->GetLocalTransform().SetPosition(0, 0, 0);
	pNodeMesh->GetLocalTransform().SetScale(1.0f, 1.0f, 1.0f);
	pMesh->RebuildBvhTree();


	pShapeMgr = pScene->GetLogicalShapeMgr();
	LogicalSphere* pLogicalSphere = pShapeMgr->CreateSphere(pNodeMesh, "sphere", 100.0f);
	pLogicalSphere->SetCollidable(false);

	//-----Generate Rays 2--------
	std::vector<N_Ray> rayArray;
	GI::RandomSampleGenerator g;
	const int c_rayCount = 20000;
	for (int rayId = 0; rayId < c_rayCount; ++rayId)
	{
		Vec3 origin = Vec3(-100.0f, 0, 0);
		Vec3 dir = Vec3(200.0f, 22.0f * g.NormalizedReal(), 22.0f * g.NormalizedReal());
		N_Ray r = N_Ray(origin, dir, 0.001f, 10.0f);
		rayArray.push_back(r);
	}

	std::cout << "start..." << std::endl;

	N_RayHitResult hitRes;//ground truth
	std::ofstream file("intersection_profile.txt", std::ios::out | std::ios::trunc);
	//file.open("debugInfo.txt", std::ios::out);

#define STREAM file

	//method1
	timer.ResetAll();
	timer.NextTick();
	for (int i = 0; i < c_rayCount; ++i)
	{
		N_RayHitResult tmpHitRes;
		const N_Ray& ray = rayArray.at(i);
		bool intersect1 = pCT->IntersectRayMeshWithBvh(ray, pMesh, tmpHitRes);
		hitRes.Union(tmpHitRes);
	}
	timer.NextTick();
	STREAM << "method: BVH" << '\n';
	STREAM << "triCount:" << pMesh->GetTriangleCount() << '\n';
	STREAM << "rayCount:" << c_rayCount << '\n';
	STREAM << "time:" << timer.GetTotalTimeElapsed() << '\n';

	//method2
	timer.ResetAll();
	timer.NextTick();
	for (int i = 0; i < c_rayCount; ++i)
	{
		N_RayHitResult tmpHitRes;
		const N_Ray& ray = rayArray.at(i);
		bool intersect1 = pCT->IntersectRayMesh(ray, pMesh, tmpHitRes);
		hitRes.Union(tmpHitRes);
	}
	timer.NextTick();
	STREAM << "method: linear" << '\n';
	STREAM << "triCount:" << pMesh->GetTriangleCount() << '\n';
	STREAM << "rayCount:" << c_rayCount << '\n';
	STREAM << "time:" << timer.GetTotalTimeElapsed() << '\n';

	//method3
	timer.ResetAll();
	timer.NextTick();
	for (int i = 0; i < c_rayCount; ++i)
	{
		N_RayHitResult tmpHitRes;
		const N_Ray& ray = rayArray.at(i);
		bool intersect1 = pCT->IntersectRayMesh_GpuBased(ray, pMesh, tmpHitRes);
		hitRes.Union(tmpHitRes);
	}
	timer.NextTick();
	STREAM << "method: GPU-based" << '\n';
	STREAM << "triCount:" << pMesh->GetTriangleCount() << '\n';
	STREAM << "rayCount:" << c_rayCount << '\n';
	STREAM << "time:" << timer.GetTotalTimeElapsed() << '\n';

	//method4-ray sphere
	timer.ResetAll();
	timer.NextTick();
	for (int i = 0; i < c_rayCount; ++i)
	{
		N_RayHitResult tmpHitRes;
		const N_Ray& ray = rayArray.at(i);
		bool intersect1 = pCT->IntersectRaySphere(ray, pLogicalSphere, tmpHitRes);
		hitRes.Union(tmpHitRes);
	}
	timer.NextTick();
	STREAM << "extra : ray-sphere" << '\n';
	STREAM << "rayCount:" << c_rayCount << '\n';
	STREAM << "time:" << timer.GetTotalTimeElapsed() << '\n';

	if (file.is_open())file.close();

	return true;
};
