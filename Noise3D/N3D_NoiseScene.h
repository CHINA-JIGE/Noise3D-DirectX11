
/***********************************************************************

                           h��NoiseScene

************************************************************************/

#pragma once

public class _declspec(dllexport) NoiseScene
{
	friend class NoiseMesh;
	friend class NoiseRenderer;
	friend class NoiseCamera;
	friend class NoiseLineBuffer;

public:
	//���캯��
	NoiseScene();
	~NoiseScene();

	BOOL			CreateMesh(NoiseMesh* pMesh);
	
	BOOL			CreateRenderer(NoiseRenderer* pRenderer);

	BOOL			CreateCamera(NoiseCamera* pSceneCam);

	BOOL			CreateLightManager(NoiseLightManager* pLightMgr);

	BOOL			CreateLineBuffer(NoiseLineBuffer* pLineBuffer);

	void				SetCamera(NoiseCamera* pSceneCam);

	NoiseMesh*					GetMesh(UINT iMeshIndex);

	NoiseRenderer*				GetRenderer();

	NoiseCamera*				GetCamera();

	NoiseLightManager*		GetLightManager();

private:

	NoiseCamera*							m_pChildCamera;

	NoiseRenderer*							m_pChildRenderer;

	NoiseLightManager*					m_pChildLightMgr;

	std::vector<NoiseMesh*> *		m_pChildMeshList;

	std::vector<NoiseLineBuffer*>* m_pChildLineBufferList;

};

