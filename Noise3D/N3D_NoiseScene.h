
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
	friend class NoiseFontManager;


public:
	//¹¹Ôìº¯Êý
	NoiseScene();

	void				ReleaseAllChildObject();

	UINT				CreateMesh(NoiseMesh& refMesh);
	
	BOOL			CreateRenderer(NoiseRenderer& refRenderer);

	BOOL			CreateCamera(NoiseCamera& refSceneCam);

	BOOL			CreateLightManager(NoiseLightManager& refLightMgr);

	UINT				CreateGraphicObject(NoiseGraphicObject& refGraphicObj);

	BOOL			CreateTextureManager(NoiseTextureManager& refTexMgr);

	BOOL			CreateMaterialManager(NoiseMaterialManager& refMatMgr);

	BOOL			CreateAtmosphere(NoiseAtmosphere& refAtmosphere);

	BOOL			CreateGUI(NoiseGUIManager& refGUI, NoiseUtInputEngine& refInputE,HWND hwnd);

	BOOL			CreateFontManager(NoiseFontManager& refFMgr);

	void			BindRenderer(NoiseRenderer& refRenderer);

	void			BindLightManager(NoiseLightManager& refLightMgr);

	void			BindTextureManager(NoiseTextureManager& refTexMgr);

	void			BindMaterialManager(NoiseMaterialManager& refMatMgr);

	void			BindAtmosphere(NoiseAtmosphere& refAtmosphere);

	void			BindGUI(NoiseGUIManager& refGUI);

	void			BindFontManager(NoiseFontManager& refFMgr);

	void			BindCamera(NoiseCamera& refSceneCam);

	NoiseRenderer*					GetRenderer();

	NoiseCamera*					GetCamera();

	NoiseLightManager*			GetLightManager();

	NoiseTextureManager*		GetTextureMgr();

	NoiseMaterialManager*	GetMaterialMgr();

	NoiseFontManager*			GetFontMgr();

	NoiseGUIManager*			GetGUIManager();

private:
	
	NoiseCamera*							m_pChildCamera;

	NoiseRenderer*							m_pChildRenderer;

	NoiseLightManager*					m_pChildLightMgr;

	NoiseTextureManager*				m_pChildTextureMgr;

	NoiseMaterialManager*			m_pChildMaterialMgr;

	NoiseAtmosphere*					m_pChildAtmosphere;

	NoiseGUIManager*					m_pChildGUIMgr;

	NoiseFontManager*					m_pChildFontMgr;



	std::vector<NoiseMesh*> *				m_pChildMeshList;

	std::vector<NoiseGraphicObject*>*	m_pChildGraphicObjectList;

};

