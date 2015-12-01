
/***********************************************************************

                           h£ºNoiseScene

************************************************************************/

#pragma once

class _declspec(dllexport) NoiseScene:public NoiseClassLifeCycle
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

	UINT				CreateMesh(NoiseMesh& refObject);
	
	BOOL			CreateRenderer(NoiseRenderer& refObject);

	BOOL			CreateCamera(NoiseCamera& refObject);

	BOOL			CreateLightManager(NoiseLightManager& refObject);

	UINT				CreateGraphicObject(NoiseGraphicObject& refObject);

	BOOL			CreateTextureManager(NoiseTextureManager& refObject);

	BOOL			CreateMaterialManager(NoiseMaterialManager& refObject);

	BOOL			CreateAtmosphere(NoiseAtmosphere& refObject);

	BOOL			CreateGUI(NoiseGUIManager& refGUI, NoiseUtInputEngine& refInputE,HWND hwnd);

	BOOL			CreateFontManager(NoiseFontManager& refObject);

	void			BindRenderer(NoiseRenderer& refObject);

	void			BindLightManager(NoiseLightManager& refObject);

	void			BindTextureManager(NoiseTextureManager& refObject);

	void			BindMaterialManager(NoiseMaterialManager& refObject);

	void			BindAtmosphere(NoiseAtmosphere& refObject);

	void			BindGUI(NoiseGUIManager& refObject);

	void			BindFontManager(NoiseFontManager& refObject);

	void			BindCamera(NoiseCamera& refObject);

	NoiseRenderer*					GetRenderer();

	NoiseCamera*					GetCamera();

	NoiseLightManager*			GetLightManager();

	NoiseTextureManager*		GetTextureMgr();

	NoiseMaterialManager*	GetMaterialMgr();

	NoiseFontManager*			GetFontMgr();

	NoiseGUIManager*			GetGUIManager();

private:

	void		Destroy();

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

