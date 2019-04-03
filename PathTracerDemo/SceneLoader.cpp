#include "Noise3D.h"
#include "SceneLoader.h"

void SceneLoader::Init(SceneManager* pMgr)
{
	m_pScene = pMgr;
	m_pMeshMgr = m_pScene->GetMeshMgr();
	m_pModelLoader = m_pScene->GetMeshLoader();
	m_pMatMgr = m_pScene->GetMaterialMgr();
	m_pTexMgr = m_pScene->GetTextureMgr();
	m_pShapeMgr = m_pScene->GetLogicalShapeMgr();
}

void SceneLoader::LoadScene()
{
	SceneGraph& sg = m_pScene->GetSceneGraph();
	_LoadTextures();
	_LoadMaterials();

	_LoadSphere(sg, Vec3(0,0,0), 10.0f);
	_LoadSphere(sg, Vec3(10.0f, 10.0f, 10.0f), 5.0f);
	_LoadBox(sg, Vec3(20.0f, 0, 0), Vec3(10.0f, 10.0f, 10.0f));
}

void SceneLoader::_LoadTextures()
{
	//m_pTexMgr->CreateTextureFromFile("../media/earth.jpg", "Earth", TRUE, 1024, 1024, FALSE);
	m_pTexMgr->CreateTextureFromFile("../media/white.jpg", "Universe", FALSE, 256, 256, FALSE);
	//pTexMgr->CreateCubeMapFromDDS("../media/CubeMap/cube-room.dds", "Universe", FALSE);
	m_pTexMgr->CreateTextureFromFile("../media/noise3d.png", "BottomRightTitle", TRUE, 0, 0, FALSE);
	Texture2D* pTex;

	//ITexture* pNormalMap = m_pTexMgr->CreateTextureFromFile("../media/earth-normal.png", "EarthNormalMap", FALSE, 512, 512, TRUE);
	//pNormalMap->ConvertTextureToGreyMap();
	//pNormalMap->ConvertHeightMapToNormalMap(10.0f);
}

void SceneLoader::_LoadMaterials()
{
	N_MaterialDesc Mat1;
	Mat1.ambientColor = Vec3(0, 0, 0);
	Mat1.diffuseColor = Vec3(1.0f, 1.0f, 1.0f);
	Mat1.specularColor = Vec3(1.0f, 1.0f, 1.0f);
	Mat1.specularSmoothLevel = 40;
	Mat1.normalMapBumpIntensity = 0.2f;
	Mat1.environmentMapTransparency = 0.1f;
	Mat1.diffuseMapName = "Earth";//"Earth");
	//Mat1.normalMapName = "EarthNormalMap";
	//Mat1.environmentMapName = "AtmoTexture";
	m_pMatMgr->CreateMaterial("mat1", Mat1);
}

void SceneLoader::_LoadSphere(SceneGraph& sg, Vec3 pos, float radius)
{
	static int id = 0;
	id++;

	SceneNode* pNode = sg.GetRoot()->CreateChildNode();
	pNode->GetLocalTransform().SetPosition(pos);

	Mesh* pMeshSphere = m_pMeshMgr->CreateMesh(pNode, "sphere"+std::to_string(id));
	m_pModelLoader->LoadSphere(pMeshSphere, radius, 15, 15);
	pMeshSphere->SetCollidable(false);

	LogicalSphere* pSphere = m_pShapeMgr->CreateSphere(pNode, "logicSPH" + std::to_string(id));
	pSphere->SetRadius(radius);
	pSphere->SetCollidable(true);

	mMeshList.push_back(pMeshSphere);
}

void SceneLoader::_LoadBox(SceneGraph& sg, Vec3 pos, Vec3 size)
{
	static int id = 0;
	id++;

	SceneNode* pNode = sg.GetRoot()->CreateChildNode();
	pNode->GetLocalTransform().SetPosition(pos);

	Mesh* pMeshBox = m_pMeshMgr->CreateMesh(pNode, "box" + std::to_string(id));
	m_pModelLoader->LoadBox(pMeshBox, size.x, size.y, size.z);
	pMeshBox->SetCollidable(false);

	LogicalBox* pBox = m_pShapeMgr->CreateBox(pNode, "logicBox" + std::to_string(id));
	pBox->SetCollidable(true);
	pBox->SetSizeXYZ(size);

	mMeshList.push_back(pMeshBox);
}
