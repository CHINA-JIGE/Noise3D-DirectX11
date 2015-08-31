
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
};

NoiseScene::~NoiseScene()
{

};

BOOL NoiseScene::CreateMesh( NoiseMesh* pMesh)
{
	if(pMesh != NULL)
	{
		//m_pChildMeshList->push_back(pMesh);
		pMesh->m_pFatherScene =this;
	}
	else
	{
		return FALSE;
	}
	;
	return TRUE;
};

BOOL NoiseScene::CreateRenderer(NoiseRenderer* pRenderer)
{
	if(pRenderer != NULL)
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

	if(pSceneCam != NULL)
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

	if(pLightMgr != NULL)
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

BOOL NoiseScene::CreateGraphicObject(NoiseGraphicObject * pLineBuffer)
{
	if (pLineBuffer != NULL)
	{
		//m_pChildLineBufferList->push_back(pLineBuffer);
		pLineBuffer->m_pFatherScene = this;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL NoiseScene::CreateTextureManager(NoiseTextureManager* pTexMgr)
{
	if (pTexMgr != NULL)
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
	if (pMatMgr != NULL)
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
	

