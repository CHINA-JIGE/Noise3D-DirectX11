
/***********************************************************************

                           Àà£ºNOISE SceneManger

			¼òÊö£ºCenter of many manager object£¨MESH,LIGHT,MATERIAL,TEXTURE...£©
					

************************************************************************/

#include "Noise3D.h"

#pragma region HAHAHA
#define CREATE_OBJECT_ADDTOLIST(childObjList) \
	try\
	{\
		refObject.Initialize();\
	}\
	catch (std::runtime_error)\
	{\
		DEBUG_MSG1("Object Has Been Created!");\
		return NOISE_MACRO_INVALID_ID;\
	}\
	childObjList->push_back(&refObject);\
	refObject.m_pFatherScene = this;\
	return (childObjList->size()-1);\

#define CREATE_OBJECT(pChildObj) \
	try\
	{\
		refObject.Initialize();\
	}\
	catch (std::runtime_error)\
	{\
		DEBUG_MSG1("Object Has Been Created!");\
		return FALSE;\
	}\
	pChildObj=&refObject;\
	refObject.m_pFatherScene = this;\
	return TRUE;\

#pragma endregion HAHAHA

NoiseScene::NoiseScene()
{
	m_pChildAtmosphere = nullptr;
	m_pChildFontMgr = nullptr;
	m_pChildGUIMgr = nullptr;
	m_pChildCamera			= nullptr;
	m_pChildRenderer		= nullptr;
	m_pChildLightMgr		= nullptr;
	m_pChildTextureMgr	= nullptr;
	m_pChildMaterialMgr	= nullptr;
	m_pChildMeshList					= new std::vector<NoiseMesh*>;
	m_pChildGraphicObjectList	= new std::vector<NoiseGraphicObject*>;
}

void NoiseScene::Destroy()
{
	m_pChildAtmosphere = nullptr;
	m_pChildFontMgr = nullptr;
	m_pChildGUIMgr = nullptr;
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

	DESTROY_OBJECT(m_pChildGUIMgr);

	DESTROY_OBJECT(m_pChildFontMgr);

	if (m_pChildMeshList)
	{
		for (auto refMesh : *m_pChildMeshList)
		{
			refMesh->SelfDestruction();
		}
	}

	if (m_pChildGraphicObjectList)
	{
		for (auto refGraphicObj : *m_pChildGraphicObjectList)
		{
			refGraphicObj->SelfDestruction();
		}
	}

};

UINT NoiseScene::CreateMesh(NoiseMesh& refObject)
{
	CREATE_OBJECT_ADDTOLIST(m_pChildMeshList);
};

BOOL NoiseScene::CreateRenderer(NoiseRenderer& refObject)
{
	try
	{
		refObject.Initialize();
	}
		catch (std::runtime_error)
	{
		DEBUG_MSG1("Object Has Been Created!");
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

BOOL NoiseScene::CreateCamera(NoiseCamera& refObject)
{
	CREATE_OBJECT(m_pChildCamera);
	return TRUE;

}

BOOL NoiseScene::CreateLightManager(NoiseLightManager& refObject)
{
	CREATE_OBJECT(m_pChildLightMgr);
}

UINT	  NoiseScene::CreateGraphicObject(NoiseGraphicObject& refObject)
{
	CREATE_OBJECT_ADDTOLIST(m_pChildGraphicObjectList);
}

BOOL NoiseScene::CreateTextureManager(NoiseTextureManager& refObject)
{
	CREATE_OBJECT(m_pChildTextureMgr);
}

BOOL NoiseScene::CreateMaterialManager(NoiseMaterialManager& refObject)
{
	CREATE_OBJECT(m_pChildMaterialMgr);
}

BOOL NoiseScene::CreateAtmosphere(NoiseAtmosphere& refObject)
{
	CREATE_OBJECT(m_pChildAtmosphere);
}

BOOL NoiseScene::CreateGUI(NoiseGUIManager& refGUI,NoiseUtInputEngine& refInputE, HWND hwnd)
{

	m_pChildGUIMgr = &refGUI;
	refGUI.m_pFatherScene = this;
	refGUI.m_pInputEngine =&refInputE;

	//hwnd is needed for some win API invokation
	if (hwnd)
	{
		refGUI.mWindowHWND = hwnd;
	}
	else
	{
		DEBUG_MSG1("Create GUI : hwnd Invaid!!");
		return FALSE;
	}

	return TRUE;
}

BOOL NoiseScene::CreateFontManager(NoiseFontManager& refObject)
{
	if (!refObject.mFunction_InitFreeType())return FALSE;
	CREATE_OBJECT(m_pChildFontMgr);
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

void NoiseScene::BindGUI(NoiseGUIManager & refObject)
{
	m_pChildGUIMgr = &refObject;
}

void NoiseScene::BindFontManager(NoiseFontManager & refObject)
{
	m_pChildFontMgr = &refObject;
};

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

NoiseFontManager * NoiseScene::GetFontMgr()
{
	return m_pChildFontMgr;
}

NoiseGUIManager * NoiseScene::GetGUIManager()
{
	return m_pChildGUIMgr;
}


/************************************************************************
                                          P R I V A T E                       
************************************************************************/

