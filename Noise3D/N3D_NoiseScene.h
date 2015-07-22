
/***********************************************************************

                           h：NoiseScene

************************************************************************/

#pragma once

public class _declspec(dllexport) NoiseScene
{
	//scene要跟它管的类都互粉=v=
	friend class NoiseCamera;
	friend class NoiseMesh;
	friend class NoiseRenderer;

public:
	//构造函数
	NoiseScene();
	~NoiseScene();

	BOOL			CreateMesh(NoiseMesh* pMesh);
	
	BOOL			CreateRenderer(NoiseRenderer* pRenderer);

	BOOL			CreateCamera(NoiseCamera* pSceneCam);

	BOOL			CreateLightManager(NoiseLightManager* pLightMgr);

	void				SetCamera(NoiseCamera* pSceneCam);



private:

	NoiseCamera*						m_pChildCamera;
	NoiseRenderer*						m_pChildRenderer;
	NoiseLightManager*				m_pChildLightMgr;

	std::vector<NoiseMesh*> *	m_pChildMeshList;

	std::vector <NoiseMesh*>*	m_pRenderList_Mesh;


};

