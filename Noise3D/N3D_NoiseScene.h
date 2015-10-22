
/***********************************************************************

                           h£ºNoiseScene

************************************************************************/

#pragma once

public class _declspec(dllexport) NoiseScene
{
	friend class NoiseMesh;
	friend class NoiseRenderer;
	friend class NoiseCamera;
	friend class NoiseGraphicObject;
	friend class NoiseMaterialManager;
	friend class NoiseTextureManager;
	friend class NoiseAtmosphere;
	friend class NoiseGUIManager;

public:
	//¹¹Ôìº¯Êý
	NoiseScene();

	void				ReleaseAllChildObject();

	UINT				CreateMesh(NoiseMesh* pMesh);
	
	BOOL			CreateRenderer(NoiseRenderer* pRenderer);

	BOOL			CreateCamera(NoiseCamera* pSceneCam);

	BOOL			CreateLightManager(NoiseLightManager* pLightMgr);

	UINT				CreateGraphicObject(NoiseGraphicObject* pGraphicObj);

	BOOL			CreateTextureManager(NoiseTextureManager* pTexMgr);

	BOOL			CreateMaterialManager(NoiseMaterialManager* pMatMgr);

	BOOL			CreateAtmosphere(NoiseAtmosphere* pAtmosphere);

	BOOL			CreateGUI(NoiseGUIManager* pGUI, NoiseUtInputEngine* pInputE,HWND hwnd);

	void				SetCamera(NoiseCamera* pSceneCam);

	NoiseRenderer*					GetRenderer();

	NoiseCamera*					GetCamera();

	NoiseLightManager*			GetLightManager();

	NoiseTextureManager*		GetTextureMgr();

	NoiseMaterialManager*	GetMaterialMgr();

private:

	NoiseCamera*							m_pChildCamera;

	NoiseRenderer*							m_pChildRenderer;

	NoiseLightManager*					m_pChildLightMgr;

	NoiseTextureManager*				m_pChildTextureMgr;

	NoiseMaterialManager*					m_pChildMaterialMgr;

	NoiseAtmosphere*							m_pChildAtmosphere;

	NoiseGUIManager*											m_pChildGUI;

	std::vector<NoiseMesh*> *				m_pChildMeshList;

	std::vector<NoiseGraphicObject*>*	m_pChildGraphicObjectList;

};

