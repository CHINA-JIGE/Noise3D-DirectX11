
/***********************************************************************

                           ¿‡£∫NOISE SceneManger

			ºÚ ˆ£∫Center of many manager object£®MESH,LIGHT,MATERIAL,TEXTURE...£©
					

************************************************************************/

#include "Noise3D.h"


NoiseScene::NoiseScene()
{
	m_pChildCamera			= nullptr;
	m_pChildRenderer		= nullptr;
	m_pChildLightMgr		= nullptr;
	m_pChildTextureMgr	= nullptr;
	m_pChildMaterialMgr	= nullptr;
	m_pChildMeshList					= new std::vector<NoiseMesh*>;
	m_pChildGraphicObjectList	= new std::vector<NoiseGraphicObject*>;
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

UINT NoiseScene::CreateMesh(NoiseMesh& refMesh)
{
	m_pChildMeshList->push_back(&refMesh);
	refMesh.m_pFatherScene =this;

	return (m_pChildMeshList->size()-1);
};

BOOL NoiseScene::CreateRenderer(NoiseRenderer& refRenderer)
{

	m_pChildRenderer = &refRenderer;
	refRenderer.m_pFatherScene =this;
	BOOL isSucceeded;
	isSucceeded=refRenderer.mFunction_Init();

	return isSucceeded;
};

BOOL NoiseScene::CreateCamera(NoiseCamera& refCamera)
{
	m_pChildCamera = &refCamera;
	refCamera.m_pFatherScene = this;
	return TRUE;

}

BOOL NoiseScene::CreateLightManager(NoiseLightManager& refLightMgr)
{
	m_pChildLightMgr = &refLightMgr;
	m_pChildLightMgr->m_pFatherScene = this;
	return TRUE;

}

UINT	  NoiseScene::CreateGraphicObject(NoiseGraphicObject& refGraphicObj)
{

	m_pChildGraphicObjectList->push_back(&refGraphicObj);
	refGraphicObj.m_pFatherScene = this;
	return m_pChildGraphicObjectList->size()-1;
}

BOOL NoiseScene::CreateTextureManager(NoiseTextureManager& refTexMgr)
{
	m_pChildTextureMgr = &refTexMgr;
	refTexMgr.m_pFatherScene = this;
	return TRUE;
}

BOOL NoiseScene::CreateMaterialManager(NoiseMaterialManager& refMatMgr)
{
	m_pChildMaterialMgr = &refMatMgr;
	refMatMgr.m_pFatherScene = this;

	return TRUE;
}

BOOL NoiseScene::CreateAtmosphere(NoiseAtmosphere& refAtmosphere) 
{
	m_pChildAtmosphere = &refAtmosphere;
	refAtmosphere.m_pFatherScene = this;
	return TRUE;
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

BOOL NoiseScene::CreateFontManager(NoiseFontManager& refFontMgr)
{
	if (!refFontMgr.mFunction_InitFreeType())return FALSE;
	m_pChildFontMgr = &refFontMgr;
	refFontMgr.m_pFatherScene = this;
	return TRUE;
}

void NoiseScene::BindRenderer(NoiseRenderer & refRenderer)
{
	m_pChildRenderer = &refRenderer;
};

void NoiseScene::BindLightManager(NoiseLightManager & refLightMgr)
{
	m_pChildLightMgr = &refLightMgr;
}

void NoiseScene::BindTextureManager(NoiseTextureManager & refTexMgr)
{
	m_pChildTextureMgr = &refTexMgr;
}

void NoiseScene::BindMaterialManager(NoiseMaterialManager & refMatMgr)
{
	m_pChildMaterialMgr = &refMatMgr;
}

void NoiseScene::BindAtmosphere(NoiseAtmosphere & refAtmosphere)
{
	m_pChildAtmosphere = &refAtmosphere;
}

void NoiseScene::BindGUI(NoiseGUIManager & refGUI)
{
	m_pChildGUIMgr = &refGUI;
}

void NoiseScene::BindFontManager(NoiseFontManager & refFMgr)
{
	m_pChildFontMgr = &refFMgr;
};

void	NoiseScene::BindCamera(NoiseCamera& refCamera)
{
	m_pChildCamera = &refCamera;
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
	

