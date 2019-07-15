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
	//pModelLoader->LoadFile_STL(pMesh, "../media/model/buddha.stl");
	pModelLoader->LoadFile_STL(pMesh, "../media/model/sphere.stl");
	//pModelLoader->LoadFile_OBJ(pMesh, "../media/model/teapot.obj");
	pNodeMesh->GetLocalTransform().SetPosition(0, 0, 0);
	pNodeMesh->GetLocalTransform().SetScale(1.0f, 1.0f, 1.0f);
	pMesh->RebuildBvhTree();

	//test if BVH has included all triangle id
	BvhTreeForTriangularMesh& bvh = pMesh->GetBvhTree();
	std::vector<BvhNodeForTriangularMesh*> bvhNodeList;
	std::vector<uint32_t> triList;
	bvh.Traverse_PreOrder(bvh.GetRoot(), bvhNodeList);
	for (auto pNode : bvhNodeList)
	{
		auto& list = pNode->GetTriangleIndexList();
		if (!list.empty())
		{
			triList.insert(triList.end(),pNode->GetTriangleIndexList().begin(), pNode->GetTriangleIndexList().end());
		}
	}
	if (triList.size() != pMesh->GetTriangleCount())
	{
		int fucked = 1;
	}

	//-----Generate Rays--------
	/*std::vector<N_Ray> rayArray;
	const int c_rayCount = 10000;
	for (int rayId = 0; rayId < c_rayCount; ++rayId)
	{
		Vec3 origin = Vec3(-100.0f, 0, 0);
		Vec3 dir = Vec3(200.0f, 0, 0);
		N_Ray r = N_Ray(origin, dir,  0.001f,10.0f);
		rayArray.push_back(r);
	}

	std::cout << "start..." << std::endl;*/

	//**************************************************************
	/*timer.ResetAll();
	N_RayHitResult hitRes;//ground truth
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
	*/

	//**************************************************************
	/*
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
	OutputDebugStringA(str4.c_str());*/

	//-----Generate Rays 2--------
	std::vector<N_Ray> rayArray;
	GI::RandomSampleGenerator g;
	const int c_rayCount = 10000;
	for (int rayId = 0; rayId < c_rayCount; ++rayId)
	{
	Vec3 origin = Vec3(-100.0f, 0, 0);
	Vec3 dir = Vec3(200.0f, 22.0f * g.NormalizedReal(), 22.0f * g.NormalizedReal());
	N_Ray r = N_Ray(origin, dir,  0.001f,10.0f);
	rayArray.push_back(r);

	/*Vec3 origin = Vec3(-100.0f, 0, 0);
	Vec3 dir = Vec3(200, -17.1661, 20.1153);
	N_Ray r = N_Ray(origin, dir, 0.001f, 10.0f);
	rayArray.push_back(r);

	origin = Vec3(-100.0f, 0, 0);
	dir = Vec3(200, -20.3966, 12.9888);
	r = N_Ray(origin, dir, 0.001f, 10.0f);
	rayArray.push_back(r);

	origin = Vec3(-100.0f, 0, 0);
	dir = Vec3(200, -21.876, 7.90692);
	r = N_Ray(origin, dir, 0.001f, 10.0f);
	rayArray.push_back(r);*/
	}

	std::cout << "start..." << std::endl;

	N_RayHitResult hitRes;//ground truth
	N_RayHitResult hitRes2;
	std::ofstream file("debugInfo.txt",std::ios::out|std::ios::trunc);
	//file.open("debugInfo.txt", std::ios::out);
#define STREAM file

	for (int i = 0; i < c_rayCount; ++i)
	{
		N_RayHitResult tmpHitRes;
		N_RayHitResult tmpHitRes2;
		const N_Ray& ray = rayArray.at(i);
		bool intersect1 = pCT->IntersectRayMesh(ray, pMesh, tmpHitRes);
		bool intersect2 = pCT->IntersectRayMeshWithBvh(ray, pMesh, tmpHitRes2);
		if (intersect1 != intersect2)
		{
			STREAM << "**************************" << std::endl;
			STREAM << "intersect bool not eq: 1:" <<intersect1 <<"  2:"<<intersect2<< std::endl;
			STREAM << "ray id: " << i << std::endl;
			STREAM << "ray: o(" << ray.origin.x <<"," <<ray.origin.y<< "," << ray.origin.z <<")"
				<<"  d("<< ray.dir.x<< "," << ray.dir.y<< "," << ray.dir.z<< ")" << std::endl;
		}
		if (tmpHitRes.hitList.size() != tmpHitRes2.hitList.size())
		{
			STREAM << "**************************" << std::endl;
			STREAM << "hit result not identical: size1:" << tmpHitRes.hitList.size() << "  size2:" << tmpHitRes2.hitList.size() << std::endl;
			STREAM << "ray id: " << i << std::endl;
			STREAM << "ray: o(" << ray.origin.x << "," << ray.origin.y << "," << ray.origin.z << ")"
				<< "  d(" << ray.dir.x << "," << ray.dir.y << "," << ray.dir.z << ")" << std::endl;
			STREAM << "hit result 1:" << std::endl;
			for (auto tri : tmpHitRes.hitList)
			{
				STREAM << "--- t:" << tri.t << std::endl;
				STREAM << "--- pos:" << tri.pos.x <<"," << tri.pos.y << "," << tri.pos.z << "," << std::endl;
			}
			STREAM << "hit result 2:" << std::endl;
			for (auto tri : tmpHitRes2.hitList)
			{
				STREAM << "--- t:" << tri.t << std::endl;
				STREAM << "--- pos:" << tri.pos.x << "," << tri.pos.y << "," << tri.pos.z << "," << std::endl;
			}

		}

		hitRes.Union(tmpHitRes);
		hitRes2.Union(tmpHitRes2);
	}

	STREAM << "final hit list size" <<hitRes.hitList.size()<<" & "<< hitRes2.hitList.size() << std::endl;
	if (hitRes.hitList.size() != hitRes2.hitList.size())
	{
		int fucked = 1;
	}

	if (file.is_open())file.close();

	return true;
};
