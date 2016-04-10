
/***********************************************************************

                           Àà£ºNOISE SceneManger

			¼òÊö£ºCenter of many manager object£¨MESH,LIGHT,MATERIAL,TEXTURE...£©
					

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

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

IScene::IScene()
{
	m_pChildAtmosphere = nullptr;
	m_pChildCamera			= nullptr;
	m_pChildRenderer		= nullptr;
	m_pChildLightMgr		= nullptr;
	m_pChildTextureMgr	= nullptr;
	m_pChildMaterialMgr	= nullptr;
	m_pChildMeshList					= new std::vector<IMesh*>;
}

void IScene::Destroy()
{
	m_pChildAtmosphere = nullptr;
	m_pChildCamera = nullptr;
	m_pChildRenderer = nullptr;
	m_pChildLightMgr = nullptr;
	m_pChildTextureMgr = nullptr;
	m_pChildMaterialMgr = nullptr;
	ReleaseAllChildObject();
};

void	IScene::ReleaseAllChildObject()
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

UINT IScene::CreateMesh(IMesh** ppOutInterface)
{

	CREATE_OBJECT_ADDTOLIST(m_pChildMeshList);
};

BOOL IScene::CreateRenderer(IRenderer** ppOutInterface)
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

BOOL IScene::CreateCamera(ICamera** ppOutInterface)
{
	CREATE_OBJECT(m_pChildCamera);
	return TRUE;

}

BOOL IScene::CreateLightManager(ILightManager** ppOutInterface)
{
	CREATE_OBJECT(m_pChildLightMgr);
}

BOOL IScene::CreateTextureManager(ITextureManager** ppOutInterface)
{
	CREATE_OBJECT(m_pChildTextureMgr);
}

BOOL IScene::CreateMaterialManager(IMaterialManager** ppOutInterface)
{
	CREATE_OBJECT(m_pChildMaterialMgr);
}

BOOL IScene::CreateAtmosphere(IAtmosphere** ppOutInterface)
{
	CREATE_OBJECT(m_pChildAtmosphere);
}



IRenderer * IScene::GetRenderer()
{
	return m_pChildRenderer;
}

ICamera * IScene::GetCamera()
{
	return m_pChildCamera;
}

ILightManager * IScene::GetLightManager()
{
	return m_pChildLightMgr;
}

ITextureManager * IScene::GetTextureMgr()
{
	return m_pChildTextureMgr;
}

IMaterialManager * IScene::GetMaterialMgr()
{
	return m_pChildMaterialMgr;
}


/************************************************************************
                                          P R I V A T E                       
************************************************************************/

