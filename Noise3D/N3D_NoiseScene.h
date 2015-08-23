
/***********************************************************************

                           h£ºNoiseScene

************************************************************************/

#pragma once

public class _declspec(dllexport) NoiseScene
{
	friend class NoiseMesh;
	friend class NoiseRenderer;
	friend class NoiseCamera;
	friend class NoiseLineBuffer;
	friend class NoiseMaterialManager;
	friend class NoiseTextureManager;

public:
	//¹¹Ôìº¯Êý
	NoiseScene();
	~NoiseScene();

	BOOL			CreateMesh(NoiseMesh* pMesh);
	
	BOOL			CreateRenderer(NoiseRenderer* pRenderer);

	BOOL			CreateCamera(NoiseCamera* pSceneCam);

	BOOL			CreateLightManager(NoiseLightManager* pLightMgr);

	BOOL			CreateLineBuffer(NoiseLineBuffer* pLineBuffer);

	BOOL			CreateTextureManager(NoiseTextureManager* pTexMgr);

	BOOL			CreateMaterialManager(NoiseMaterialManager* pMatMgr);

	void				SetCamera(NoiseCamera* pSceneCam);

	NoiseMesh*					GetMesh(UINT iMeshIndex);

	NoiseRenderer*				GetRenderer();

	NoiseCamera*				GetCamera();

	NoiseLightManager*		GetLightManager();

private:

	NoiseCamera*							m_pChildCamera;

	NoiseRenderer*							m_pChildRenderer;

	NoiseLightManager*					m_pChildLightMgr;

	NoiseTextureManager*				m_pChildTextureMgr;

	NoiseMaterialManager*			m_pChildMaterialMgr;

	std::vector<NoiseMesh*> *		m_pChildMeshList;

	std::vector<NoiseLineBuffer*>* m_pChildLineBufferList;

};

