
/***********************************************************************

                           类：NOISE SceneManger

			简述：主要负责管理3D场景元素（MESH,LIGHT,MATERIAL,TEXTURE）
					

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
	if (m_pChildCamera)			m_pChildCamera->SelfDestruction();

	if (m_pChildRenderer)		m_pChildRenderer->SelfDestruction();

	if (m_pChildLightMgr)		m_pChildLightMgr->SelfDestruction();

	if (m_pChildTextureMgr)	m_pChildTextureMgr->SelfDestruction();

	if (m_pChildMaterialMgr)	m_pChildMaterialMgr->SelfDestruction();

	if (m_pChildAtmosphere)	m_pChildAtmosphere->SelfDestruction();

	if (m_pChildMeshList)
	{
		for (auto pMesh : *m_pChildMeshList)
		{
			pMesh->SelfDestruction();
		}
	}

	if (m_pChildGraphicObjectList)
	{
		for (auto pGraphicObj : *m_pChildGraphicObjectList)
		{
			pGraphicObj->SelfDestruction();
		}
	}

	if (m_pChildGUI)m_pChildGUI->SelfDestruction();
};

UINT NoiseScene::CreateMesh( NoiseMesh* pMesh)
{
	if(pMesh != nullptr)
	{
		m_pChildMeshList->push_back(pMesh);
		pMesh->m_pFatherScene =this;
	}
	else
	{
		return NOISE_MACRO_INVALID_MESH_ID;
	};
	
	return (m_pChildMeshList->size()-1);
};

BOOL NoiseScene::CreateRenderer(NoiseRenderer* pRenderer)
{
	if(pRenderer != nullptr)
	{
		m_pChildRenderer = pRenderer;
		pRenderer->m_pFatherScene =this;
	}
	BOOL isSucceeded;
	isSucceeded=pRenderer->mFunction_Init();

	return isSucceeded;
};

BOOL NoiseScene::CreateCamera(NoiseCamera* pSceneCam)
{

	if(pSceneCam != nullptr)
	{
		m_pChildCamera = pSceneCam;
		pSceneCam->m_pFatherScene = this;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
};

BOOL NoiseScene::CreateLightManager(NoiseLightManager* pLightMgr)
{

	if(pLightMgr != nullptr)
	{
		m_pChildLightMgr = pLightMgr;
		m_pChildLightMgr->m_pFatherScene = this;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

UINT	  NoiseScene::CreateGraphicObject(NoiseGraphicObject * pGraphicObj)
{
	if (pGraphicObj != nullptr)
	{
		m_pChildGraphicObjectList->push_back(pGraphicObj);
		pGraphicObj->m_pFatherScene = this;
	}
	else
	{
		return NOISE_MACRO_INVALID_GRAPHICOBJ_ID;
	}

	return m_pChildGraphicObjectList->size()-1;
}

BOOL NoiseScene::CreateTextureManager(NoiseTextureManager* pTexMgr)
{
	if (pTexMgr != nullptr)
	{
		m_pChildTextureMgr = pTexMgr;
		pTexMgr->m_pFatherScene = this;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL NoiseScene::CreateMaterialManager(NoiseMaterialManager * pMatMgr)
{
	if (pMatMgr != nullptr)
	{
		m_pChildMaterialMgr = pMatMgr;
		pMatMgr->m_pFatherScene = this;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL NoiseScene::CreateAtmosphere(NoiseAtmosphere* pAtmosphere) 
{
	if (pAtmosphere != nullptr)
	{
		m_pChildAtmosphere = pAtmosphere;
		pAtmosphere->m_pFatherScene = this;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL NoiseScene::CreateGUI(NoiseGUIManager * pGUI,NoiseUtInputEngine* pInputE, HWND hwnd)
{
	if (pGUI)
	{
		m_pChildGUI = pGUI;
		pGUI->m_pFatherScene = this;

		//create internal graphic object
		if (CreateGraphicObject(pGUI->m_pChildGraphicObject)==NOISE_MACRO_INVALID_GRAPHICOBJ_ID)
		{
			DEBUG_MSG1("NoiseScene : Create GUI: Create interval Graphic Object Failed!!");
			return FALSE;
		}

		//update the pointer of internal input engine
		if (pInputE)
		{
			pGUI->m_pInputEngine = pInputE;
		}
		else
		{
			DEBUG_MSG1("Create GUI : pInputE ptr invalid!!");
			return FALSE;
		}

		//hwnd is needed for some win API invokation
		if (hwnd)
		{
			pGUI->mWindowHWND = hwnd;
		}
		else
		{
			DEBUG_MSG1("Create GUI : hwnd Invaid!!");
			return FALSE;
		}

	}
	else
	{
		return FALSE;
	}

	return TRUE;
};


void	NoiseScene::SetCamera(NoiseCamera* pSceneCam)
{
	if(pSceneCam != NULL)
	{
		m_pChildCamera = pSceneCam;
	}

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
	

