
/***********************************************************************

                           h£ºNoiseScene

************************************************************************/

#pragma once

class _declspec(dllexport) NoiseScene:public NoiseClassLifeCycle
{
	friend class NoiseMesh;
	friend class NoiseRenderer;
	friend class NoiseCamera;
	friend class NoiseMaterialManager;
	friend class NoiseTextureManager;
	friend class NoiseAtmosphere;


public:
	//¹¹Ôìº¯Êý
	NoiseScene();

	void				ReleaseAllChildObject();

	UINT				InitMesh(NoiseMesh& refObject);
	
	BOOL			InitRenderer(NoiseRenderer& refObject);

	BOOL			InitCamera(NoiseCamera& refObject);

	BOOL			InitLightManager(NoiseLightManager& refObject);

	BOOL			InitTextureManager(NoiseTextureManager& refObject);

	BOOL			InitMaterialManager(NoiseMaterialManager& refObject);

	BOOL			InitAtmosphere(NoiseAtmosphere& refObject);

	void			BindRenderer(NoiseRenderer& refObject);

	void			BindLightManager(NoiseLightManager& refObject);

	void			BindTextureManager(NoiseTextureManager& refObject);

	void			BindMaterialManager(NoiseMaterialManager& refObject);

	void			BindAtmosphere(NoiseAtmosphere& refObject);

	void			BindCamera(NoiseCamera& refObject);

	NoiseRenderer*					GetRenderer();

	NoiseCamera*					GetCamera();

	NoiseLightManager*			GetLightManager();

	NoiseTextureManager*		GetTextureMgr();

	NoiseMaterialManager*	GetMaterialMgr();

private:

	void		Destroy();

	NoiseCamera*							m_pChildCamera;

	NoiseRenderer*							m_pChildRenderer;

	NoiseLightManager*					m_pChildLightMgr;

	NoiseTextureManager*				m_pChildTextureMgr;

	NoiseMaterialManager*			m_pChildMaterialMgr;

	NoiseAtmosphere*					m_pChildAtmosphere;

	std::vector<NoiseMesh*> *		m_pChildMeshList;//maybe picking-ray may need this list later....

};

