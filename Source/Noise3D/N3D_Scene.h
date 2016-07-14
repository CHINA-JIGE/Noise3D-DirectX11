
/***********************************************************************

                           h£ºNoiseScene

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ IScene
	{
		friend class IMesh;
		friend class IRenderer;
		friend class ICamera;
		friend class IMaterialManager;
		friend class ITextureManager;
		friend class IAtmosphere;


	public:
		//¹¹Ôìº¯Êý
		IScene();

		void				ReleaseAllChildObject();

		IMesh*					CreateMesh();

		IRenderer*			CreateRenderer();

		ICamera*				CreateCamera();

		ILightManager*		CreateLightManager();

		ITextureManager*			CreateTextureManager();

		IMaterialManager	*		CreateMaterialManager();

		IAtmosphere	*				CreateAtmosphere();

		IMesh*						GetMesh(std::string meshName);

		IRenderer*				GetRenderer();

		ICamera*					GetCamera();

		ILightManager*			GetLightManager();

		ITextureManager*		GetTextureMgr();

		IMaterialManager*	GetMaterialMgr();

		IAtmosphere*			GetAtmosphere();

	private:

		void		Destroy();

		ICamera*							m_pChildCamera;

		IRenderer*						m_pChildRenderer;

		ILightManager*					m_pChildLightMgr;

		ITextureManager*				m_pChildTextureMgr;

		IMaterialManager*			m_pChildMaterialMgr;

		IAtmosphere*					m_pChildAtmosphere;

		std::vector<IMesh*> *		m_pChildMeshList;//maybe picking-ray may need this list later....

	};

}