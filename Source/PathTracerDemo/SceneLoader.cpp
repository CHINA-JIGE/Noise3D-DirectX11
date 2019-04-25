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

void SceneLoader::LoadScene_DiffuseDemo(Camera * pCam)
{
	SceneGraph& sg = m_pScene->GetSceneGraph();
	_LoadTextures();
	_LoadLambertMaterials();
	_LoadAdvancedMaterials();

	_LoadSphere(sg, Vec3(-20.0f,20.0f,50), 15.0f, "");
	_LoadSphere(sg, Vec3(-50.0f, 15.0f, -30.0f), 20.0f, "");
	_LoadBox(sg, Vec3(30.0f, 12.0, 0), Vec3(50.0f, 30.0f, 60.0f), "");
	_LoadBox(sg, Vec3(-60, 25.0f, 59.0f), Vec3(	10.0f, 50.0f, 70.0f), "");
	_LoadBox(sg, Vec3(50.0, 30.0f, 70.0f), Vec3(30.0f, 60.0f, 60.0f), "");
	_LoadRect(sg, NOISE_RECT_ORIENTATION::RECT_XZ, Vec3(0, 0, 0), Vec2(200.0f, 200.0f),"");

	pCam->SetViewAngle_Radian(Ut::PI / 2.5f, 1.333333333f);
	pCam->SetViewFrustumPlane(1.0f, 500.f);
	pCam->GetWorldTransform().SetPosition(-50.0f, 70.0f, 130.0f);
	pCam->LookAt(0, 0, 0);
}

void SceneLoader::LoadScene_RefractionDemo(Camera * pCam)
{
	SceneGraph& sg = m_pScene->GetSceneGraph();
	_LoadTextures();
	_LoadLambertMaterials();
	_LoadAdvancedMaterials();

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			_LoadSphere(sg, Vec3(-10.0f + i * 10.0f, 10.0f - j * 10.0f, 0.0f), 4.0f, "glass" + std::to_string(j * 3 + i));
		}
	}

	pCam->SetViewAngle_Radian(Ut::PI / 2.5f, 1.333333333f);
	pCam->SetViewFrustumPlane(1.0f, 500.f);
	pCam->GetWorldTransform().SetPosition(0, 0, -25.0f);
	pCam->LookAt(0, 0, 0);
}

void SceneLoader::LoadScene_AreaLightingDemo(Camera * pCam)
{
	SceneGraph& sg = m_pScene->GetSceneGraph();
	_LoadTextures();
	_LoadLambertMaterials();
	_LoadAdvancedMaterials();

	_LoadSphere(sg, Vec3(-20.0f, 25.0f, 50), 15.0f, "albedo_red");
	_LoadSphere(sg, Vec3(50.0f, 20.0f, 30), 10.0f, "albedo_yellow");
	_LoadSphere(sg, Vec3(-50.0f, 15.0f, -30.0f), 20.0f, "albedo_green");
	_LoadBox(sg, Vec3(10.0f, 15.0f, -30.0f), Vec3(30.0f, 30.0f, 30.0f), "albedo_red");
	_LoadBox(sg, Vec3(-60.0f, 15.0f, 59.0f), Vec3(10.0f, 30.0f, 40.0f), "albedo_blue");
	_LoadRect(sg, NOISE_RECT_ORIENTATION::RECT_XZ, Vec3(0, 0, 0), Vec2(200.0f, 200.0f), "ground");
	//light
	_LoadRect(sg, NOISE_RECT_ORIENTATION::RECT_XZ, Vec3(0, 100.0f, 0), Vec2(100.0f, 100.0f), "emissive_white");

	pCam->SetViewAngle_Radian(Ut::PI / 2.5f, 1.333333333f);
	pCam->SetViewFrustumPlane(1.0f, 500.f);
	pCam->GetWorldTransform().SetPosition(-50.0f, 70.0f, 130.0f);
	pCam->LookAt(0, 0, 0);
}

void SceneLoader::LoadScene_StandardShader(Camera * pCam)
{
	SceneGraph& sg = m_pScene->GetSceneGraph();
	_LoadTextures();
	_LoadLambertMaterials();
	_LoadAdvancedMaterials();

	/*_LoadSphere(sg, Vec3(-20.0f, 10.0f, 80), 15.0f, "albedo_red");
	_LoadSphere(sg, Vec3(50.0f, 20.0f, 30), 10.0f, "emissive_yellow");
	_LoadSphere(sg, Vec3(-50.0f, 15.0f, -30.0f), 20.0f, "albedo_green");
	_LoadBox(sg, Vec3(10.0f, 15.0f, -30.0f), Vec3(30.0f, 30.0f, 30.0f), "albedo_red");
	_LoadBox(sg, Vec3(-60.0f, 15.0f, 59.0f), Vec3(10.0f, 30.0f, 40.0f), "albedo_blue");*/
	_LoadBox(sg, Vec3(-80.0f, 20.0f, 3.0f), Vec3(30.0f, 30.0f, 30.0f), "box");
	_LoadSphere(sg, Vec3(0, 20, 0), 40.0f, "centerBall");
	_LoadRect(sg, NOISE_RECT_ORIENTATION::RECT_XZ, Vec3(0, 0, 0), Vec2(200.0f, 200.0f), "ground");
	//light
	//_LoadRect(sg, NOISE_RECT_ORIENTATION::RECT_XZ, Vec3(0, 100.0f, 0), Vec2(100.0f, 100.0f), "emissive_white");

	pCam->SetViewAngle_Radian(Ut::PI / 2.5f, 1.333333333f);
	pCam->SetViewFrustumPlane(1.0f, 500.f);
	pCam->GetWorldTransform().SetPosition(-50.0f, 70.0f, 130.0f);
	pCam->LookAt(0, 0, 0);
}

//---------------------------------------

void SceneLoader::_LoadTextures()
{
	//m_pTexMgr->CreateTextureFromFile("../media/earth.jpg", "Earth", TRUE, 1024, 1024, FALSE);
	m_pTexMgr->CreateTextureFromFile("../media/white.jpg", "Universe", false, 256, 256, false);
	//pTexMgr->CreateCubeMapFromDDS("../media/CubeMap/cube-room.dds", "Universe", FALSE);
	m_pTexMgr->CreateTextureFromFile("../media/noise3d.png", "BottomRightTitle", true, 0, 0, false);
	m_pTexMgr->CreateTextureFromFile("../media/envmap7.jpg", "envmap", false, 1024, 512, true);
	//m_pTexMgr->CreateTextureFromFile("../media/cathedral.jpg", "envmap", false, 1024, 512, true);

}

void SceneLoader::_LoadLambertMaterials()
{
	{
		N_LambertMaterialDesc desc;
		desc.ambientColor = Vec3(0.1f, 0.1f, 0.1f);
		desc.diffuseColor = Vec3(0.5f, 0.5f, 0.5f);
		desc.specularColor = Vec3(1.0f, 1.0f, 1.0f);
		LambertMaterial* pMat1 = m_pMatMgr->CreateLambertMaterial("previewObjMat", desc);
	}

	{
		N_LambertMaterialDesc desc;
		desc.ambientColor = Vec3(1.0f, 0.1f, 0.1f);
		desc.diffuseColor = Vec3(1.0f, 0, 0);
		desc.specularColor = Vec3(1.0f, 0, 0);
		LambertMaterial* pMat1 = m_pMatMgr->CreateLambertMaterial("previewLightMat", desc);
	}
}

void SceneLoader::_LoadAdvancedMaterials()
{
	//perfect glasses
	for (int i = 0; i < 9; ++i)
	{
		GI::N_AdvancedMatDesc desc;
		auto pMat = m_pMatMgr->CreateAdvancedMaterial("glass" +std::to_string(i), desc);
		pMat->Preset_PerfectGlass();
		pMat->SetRefractiveIndex(1.01f + 0.05f * i);
	}

	{
		GI::N_AdvancedMatDesc desc;
		desc.albedo = Color4f(0.5f, 0.8f, 0.8f, 1.0f);
		desc.roughness = 0.4f;
		desc.metallicity = 0.1f;
		m_pMatMgr->CreateAdvancedMaterial("ground", desc);
	}

	{
		GI::N_AdvancedMatDesc desc;
		desc.albedo = Color4f(1.0f, 0, 0, 1.0f);
		desc.roughness = 0;
		desc.metallicity = 0.01;//a strange 'circle' will appear at center if metallicity=0(caused by specular)
		m_pMatMgr->CreateAdvancedMaterial("centerBall", desc);
	}

	{
		GI::N_AdvancedMatDesc desc;
		desc.albedo = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
		desc.roughness = 0.1f;
		desc.metallicity = 0.0f;
		m_pMatMgr->CreateAdvancedMaterial("box", desc);
	}

	{
		GI::N_AdvancedMatDesc desc;
		desc.albedo = Color4f(1.0f, 0.5f, 0.5f, 1.0f);
		desc.roughness = 0.5f;
		m_pMatMgr->CreateAdvancedMaterial("albedo_red", desc);
	}


	{
		GI::N_AdvancedMatDesc desc;
		desc.albedo = Color4f(1.0f, 1.0f, 0.3f, 1.0f);
		desc.roughness = 1.0f;
		desc.emission = Vec3(3.0f, 3.0f, 1.0f);
		m_pMatMgr->CreateAdvancedMaterial("emissive_yellow", desc);
	}

	{
		//light source
		GI::N_AdvancedMatDesc desc;
		//desc.emission = Vec3(150000.0f, 150000.0f, 150000.0f);
		desc.emission = Vec3(3.0f, 3.0f, 3.0f);
		auto pMat = m_pMatMgr->CreateAdvancedMaterial("emissive_white", desc);
	}
}

void SceneLoader::_LoadSphere(SceneGraph& sg, Vec3 pos, float radius, N_UID matUid)
{
	static int id = 0;
	id++;

	SceneNode* pNode = sg.GetRoot()->CreateChildNode();
	pNode->GetLocalTransform().SetPosition(pos);

	Mesh* pMeshSphere = m_pMeshMgr->CreateMesh(pNode, "sphere"+std::to_string(id));
	m_pModelLoader->LoadSphere(pMeshSphere, radius, 15, 15);
	pMeshSphere->SetCollidable(false);
	pMeshSphere->SetMaterial("previewObjMat");

	LogicalSphere* pSphere = m_pShapeMgr->CreateSphere(pNode, "LSph" + std::to_string(id),radius);
	pSphere->SetCollidable(true);
	GI::AdvancedGiMaterial* pMat = m_pMatMgr->GetObjectPtr<GI::AdvancedGiMaterial>(matUid);
	if (pMat != nullptr)pSphere->SetGiMaterial(pMat);

	mRealTimeRenderMeshList.push_back(pMeshSphere);
}

void SceneLoader::_LoadBox(SceneGraph& sg, Vec3 pos, Vec3 size, N_UID matUid)
{
	static int id = 0;
	id++;

	SceneNode* pNode = sg.GetRoot()->CreateChildNode();
	pNode->GetLocalTransform().SetPosition(pos);

	Mesh* pMeshBox = m_pMeshMgr->CreateMesh(pNode, "box" + std::to_string(id));
	m_pModelLoader->LoadBox(pMeshBox, size.x, size.y, size.z);
	pMeshBox->SetCollidable(false);
	pMeshBox->SetMaterial("previewObjMat");

	LogicalBox* pBox = m_pShapeMgr->CreateBox(pNode, "LBox" + std::to_string(id),size);
	pBox->SetCollidable(true);
	GI::AdvancedGiMaterial* pMat = m_pMatMgr->GetObjectPtr<GI::AdvancedGiMaterial>(matUid);
	if (pMat != nullptr)pBox->SetGiMaterial(pMat);

	mRealTimeRenderMeshList.push_back(pMeshBox);
}

void SceneLoader::_LoadRect(SceneGraph & sg, NOISE_RECT_ORIENTATION ori, Vec3 pos, Vec2 size, N_UID matUid)
{
	static int id = 0;
	id++;

	SceneNode* pNode = sg.GetRoot()->CreateChildNode();
	pNode->GetLocalTransform().SetPosition(pos);

	Mesh* pMeshRect = m_pMeshMgr->CreateMesh(pNode, "rect" + std::to_string(id));
	m_pModelLoader->LoadPlane(pMeshRect, ori, size.x, size.y, 3,3);
	pMeshRect->SetCollidable(false);
	pMeshRect->SetMaterial("previewObjMat");

	LogicalRect* pRect = m_pShapeMgr->CreateRect(pNode, "Lrect" + std::to_string(id),size, ori);
	pRect->SetCollidable(true);
	GI::AdvancedGiMaterial* pMat = m_pMatMgr->GetObjectPtr<GI::AdvancedGiMaterial>(matUid);
	if (pMat != nullptr)
	{
		pRect->SetGiMaterial(pMat);

		if (pMat->IsEmissionEnabled())
		{
			pMeshRect->SetMaterial("previewLightMat");
		}
	}
	


	mRealTimeRenderMeshList.push_back(pMeshRect);
}
