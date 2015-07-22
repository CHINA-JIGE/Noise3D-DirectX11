
/***********************************************************************

                           类：NOISE SceneManger

			简述：主要负责管理3D场景元素（MESH,LIGHT,MATERIAL,TEXTURE）
					并且主要管理各种对象

************************************************************************/

#include "Noise3D.h"


NoiseScene::NoiseScene()
{
	m_pChildMeshList = new std::vector<NoiseMesh*>;
	m_pRenderList_Mesh = new std::vector<NoiseMesh*>;
};

NoiseScene::~NoiseScene()
{
	
};

BOOL NoiseScene::CreateMesh( NoiseMesh* pMesh)
{
	if(pMesh != NULL)
	{
		m_pChildMeshList->push_back(pMesh);
		pMesh->m_pFatherScene =this;
	};
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
	isSucceeded=pRenderer->m_Function_Init();

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
};

void	NoiseScene::SetCamera(NoiseCamera* pSceneCam)
{
	if(pSceneCam != NULL)
	{
		m_pChildCamera = pSceneCam;
	}

};

/************************************************************************
                                          P R I V A T E                       
************************************************************************/
	

