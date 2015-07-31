
/***********************************************************************

                           类：NOISE SceneManger

			简述：主要负责管理3D场景元素（MESH,LIGHT,MATERIAL,TEXTURE）
					

************************************************************************/

#include "Noise3D.h"


NoiseScene::NoiseScene()
{
	m_pChildMeshList = new std::vector<NoiseMesh*>;
	m_pChildLineBufferList = new std::vector<NoiseLineBuffer*>;
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

BOOL NoiseScene::CreateLineBuffer(NoiseLineBuffer * pLineBuffer)
{
	if (pLineBuffer != NULL)
	{
		m_pChildLineBufferList->push_back(pLineBuffer);
		pLineBuffer->m_pFatherScene = this;
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

NoiseMesh * NoiseScene::GetMesh(UINT iMeshIndex)
{
	if(iMeshIndex < m_pChildMeshList->size())  return m_pChildMeshList->at(iMeshIndex);
	return nullptr;
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


/************************************************************************
                                          P R I V A T E                       
************************************************************************/
	

