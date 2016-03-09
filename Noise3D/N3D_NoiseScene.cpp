
/***********************************************************************

                           Àà£ºNOISE SceneManger

			¼òÊö£ºCenter of many manager object£¨MESH,LIGHT,MATERIAL,TEXTURE...£©
					

************************************************************************/

#include "Noise3D.h"

#pragma region HAHAHA
#define CREATE_OBJECT_ADDTOLIST(childObjList) \
	if(refObject.IsInitialized())\
	{\
		DEBUG_MSG1("Object Has Been Initialized!");\
		return NOISE_MACRO_INVALID_ID;\
	}\
	childObjList->push_back(&refObject);\
	refObject.m_pFatherScene = this;\
	refObject.SetStatusToBeInitialized();\
	return (childObjList->size()-1);\

#define CREATE_OBJECT(pChildObj) \
	if(refObject.IsInitialized())\
	{\
		DEBUG_MSG1("Object Has Been Initialized!");\
		return FALSE;\
	}\
	pChildObj=&refObject;\
	refObject.m_pFatherScene = this;\
	refObject.SetStatusToBeInitialized();\
	return TRUE;\

#pragma endregion HAHAHA

NoiseScene::NoiseScene()
{
	m_pChildAtmosphere = nullptr;
	m_pChildCamera			= nullptr;
	m_pChildRenderer		= nullptr;
	m_pChildLightMgr		= nullptr;
	m_pChildTextureMgr	= nullptr;
	m_pChildMaterialMgr	= nullptr;
	m_pChildMeshList					= new std::vector<NoiseMesh*>;
}

void NoiseScene::Destroy()
{
	m_pChildAtmosphere = nullptr;
	m_pChildCamera = nullptr;
	m_pChildRenderer = nullptr;
	m_pChildLightMgr = nullptr;
	m_pChildTextureMgr = nullptr;
	m_pChildMaterialMgr = nullptr;
	ReleaseAllChildObject();
};

void	NoiseScene::ReleaseAllChildObject()
{
#define DESTROY_OBJECT(obj) if(obj)obj->SelfDestruction()

	DESTROY_OBJECT(m_pChildCamera);

	DESTROY_OBJECT(m_pChildRenderer);

	DESTROY_OBJECT(m_pChildLightMgr);

	DESTROY_OBJECT(m_pChildTextureMgr);

	DESTROY_OBJECT(m_pChildMaterialMgr);

	DESTROY_OBJECT(m_pChildAtmosphere);

	if (m_pChildMeshList)
	{
		for (auto refMesh : *m_pChildMeshList)
		{
			refMesh->SelfDestruction();
		}
	}

};

UINT NoiseScene::InitMesh(NoiseMesh& refObject)
{

	CREATE_OBJECT_ADDTOLIST(m_pChildMeshList);
};

BOOL NoiseScene::InitRenderer(NoiseRenderer& refObject)
{
	if(refObject.IsInitialized())
	{
		DEBUG_MSG1("Object Has Been Created!");
		return FALSE;
	}


	m_pChildRenderer = &refObject;
	refObject.m_pFatherScene = this;

	BOOL isSucceeded;
	isSucceeded= refObject.mFunction_Init();
	if (!isSucceeded)
	{
		DEBUG_MSG1("Create Renderer Failed!");
	}
	return isSucceeded;
};

BOOL NoiseScene::InitCamera(NoiseCamera& refObject)
{
	CREATE_OBJECT(m_pChildCamera);
	return TRUE;

}

BOOL NoiseScene::InitLightManager(NoiseLightManager& refObject)
{
	CREATE_OBJECT(m_pChildLightMgr);
}

BOOL NoiseScene::InitTextureManager(NoiseTextureManager& refObject)
{
	CREATE_OBJECT(m_pChildTextureMgr);
}

BOOL NoiseScene::InitMaterialManager(NoiseMaterialManager& refObject)
{
	CREATE_OBJECT(m_pChildMaterialMgr);
}

BOOL NoiseScene::InitAtmosphere(NoiseAtmosphere& refObject)
{
	CREATE_OBJECT(m_pChildAtmosphere);
}

void NoiseScene::BindRenderer(NoiseRenderer & refObject)
{
	m_pChildRenderer = &refObject;
};

void NoiseScene::BindLightManager(NoiseLightManager & refObject)
{
	m_pChildLightMgr = &refObject;
}

void NoiseScene::BindTextureManager(NoiseTextureManager & refObject)
{
	m_pChildTextureMgr = &refObject;
}

void NoiseScene::BindMaterialManager(NoiseMaterialManager & refObject)
{
	m_pChildMaterialMgr = &refObject;
}

void NoiseScene::BindAtmosphere(NoiseAtmosphere & refObject)
{
	m_pChildAtmosphere = &refObject;
}

void	NoiseScene::BindCamera(NoiseCamera& refObject)
{
	m_pChildCamera = &refObject;
}

NoiseRenderer * NoiseScene::GetRenderer()
{
	return m_pChildRenderer;
}

NoiseCamera * NoiseScene::GetCamera()
{
	return m_pChildCamera;
}

NoiseLightManager * NoiseScene::GetLightManager()
{
	return m_pChildLightMgr;
}

NoiseTextureManager * NoiseScene::GetTextureMgr()
{
	return m_pChildTextureMgr;
}

NoiseMaterialManager * NoiseScene::GetMaterialMgr()
{
	return m_pChildMaterialMgr;
}


/************************************************************************
                                          P R I V A T E                       
************************************************************************/

