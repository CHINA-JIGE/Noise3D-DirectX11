
/***********************************************************************

                           h£ºNoiseScene

************************************************************************/

#pragma once

public class _declspec(dllexport) NoiseScene
{
	friend class NoiseMesh;
	friend class NoiseRenderer;
	friend class NoiseCamera;

public:
	//¹¹Ôìº¯Êý
	NoiseScene();
	~NoiseScene();

	BOOL			CreateMesh(NoiseMesh* pMesh);
	
	BOOL			CreateRenderer(NoiseRenderer* pRenderer);

	BOOL			CreateCamera(NoiseCamera* pSceneCam);

	BOOL			CreateLightManager(NoiseLightManager* pLightMgr);

	void				SetCamera(NoiseCamera* pSceneCam);

	NoiseMesh*					GetMesh(UINT iMeshIndex);

	NoiseRenderer*				GetRenderer();

	NoiseCamera*				GetCamera();

	NoiseLightManager*		GetLightManager();

private:

	NoiseCamera*						m_pChildCamera;

	NoiseRenderer*						m_pChildRenderer;

	NoiseLightManager*				m_pChildLightMgr;

	std::vector<NoiseMesh*> *	m_pChildMeshList;

};

