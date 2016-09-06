
/***********************************************************************

                           ¿‡£∫NOISE SceneManger

			ºÚ ˆ£∫Center of many manager object£®MESH,LIGHT,MATERIAL,TEXTURE...£©
					

************************************************************************/

#include "Noise3D.h"
#include "Scene.h"

using namespace Noise3D;


IScene::IScene():
	 IFactory<IRenderer>(1),
	 IFactory<ICamera>(1),
	IFactory<IMeshManager>(1),
	 IFactory<ILightManager>(1),
	 IFactory<ITextureManager>(2),//scene/font-internal
	 IFactory<IMaterialManager>(1),
	IFactory<IGraphicObjectManager>(2),//scene/font-internal
	 IFactory<IAtmosphere>(1),
	IFactory<IFontManager>(1),
	IFactory<IModelLoader>(1)
{

}

IScene::~IScene()
{
	ReleaseAllChildObject();
}


void	IScene::ReleaseAllChildObject()
{
	IFactory<IMeshManager>::DestroyAllObject();
	IFactory<IRenderer>::DestroyAllObject();
	IFactory<ICamera>::DestroyAllObject();
	IFactory<ILightManager>::DestroyAllObject();
	IFactory<ITextureManager>::DestroyAllObject();
	IFactory<IMaterialManager>::DestroyAllObject();
	IFactory<IAtmosphere>::DestroyAllObject();
	IFactory<IGraphicObjectManager>::DestroyAllObject();
}

IMeshManager * IScene::GetMeshMgr()
{
	static const N_UID uid = "sceneMeshMgr";
	if (IFactory<IMeshManager>::FindUid(uid) == FALSE)
	{
		IFactory<IMeshManager>::CreateObject(uid);
	}
	return IFactory<IMeshManager>::GetObjectPtr(uid);
};

//first time to init RENDERER
IRenderer * IScene::GetRenderer()
{
	static const N_UID uid = "sceneRenderer";
	if (IFactory<IRenderer>::FindUid(uid) == FALSE)
	{
		IRenderer* pRd = IFactory<IRenderer>::CreateObject(uid);

		//init of shaders/RV/states/....
		BOOL isSucceeded =pRd->mFunction_Init();
		if (isSucceeded)
		{
			return pRd;
		}
		else
		{
			IFactory<IRenderer>::DestroyObject(uid);
			ERROR_MSG("IScene: Renderer Initialization failed.");
			return nullptr;
		}
	}
	return IFactory<IRenderer>::GetObjectPtr(uid);
};

ICamera * IScene::GetCamera()
{
	const N_UID uid = "sceneCamera";
	if (IFactory<ICamera>::FindUid(uid) == FALSE)
	{
		IFactory<ICamera>::CreateObject(uid);
	}
	return IFactory<ICamera>::GetObjectPtr(uid);
}

ILightManager * IScene::GetLightMgr()
{
	const N_UID uid = "sceneLightMgr";
	if (IFactory<ILightManager>::FindUid(uid) == FALSE)
	{
		IFactory<ILightManager>::CreateObject(uid);
	}
	return IFactory<ILightManager>::GetObjectPtr(uid);
}

ITextureManager * IScene::GetTextureMgr()
{
	const N_UID uid = "sceneTexMgr";
	if (IFactory<ITextureManager>::FindUid(uid) == FALSE)
	{
		IFactory<ITextureManager>::CreateObject(uid);
	}
	return IFactory<ITextureManager>::GetObjectPtr(uid);
}

IMaterialManager * IScene::GetMaterialMgr()
{
	const N_UID uid = "sceneMatMgr";
	if (IFactory<IMaterialManager>::FindUid(uid) == FALSE)
	{
		IFactory<IMaterialManager>::CreateObject(uid);
	}
	return IFactory<IMaterialManager>::GetObjectPtr(uid);
}

IAtmosphere * IScene::GetAtmosphere()
{
	const N_UID uid = "sceneAtmos";
	if (IFactory<IAtmosphere>::FindUid(uid) == FALSE)
	{
		IFactory<IAtmosphere>::CreateObject(uid);
	}
	return IFactory<IAtmosphere>::GetObjectPtr(uid);
}

IGraphicObjectManager * IScene::GetGraphicObjMgr()
{
	const N_UID uid = "sceneGObjMgr";
	if (IFactory<IGraphicObjectManager>::FindUid(uid) == FALSE)
	{
		IFactory<IGraphicObjectManager>::CreateObject(uid);
	}
	return IFactory<IGraphicObjectManager>::GetObjectPtr(uid);
}

IFontManager * IScene::GetFontMgr()
{
	const N_UID uid = "sceneFontMgr";
	if (IFactory<IFontManager>::FindUid(uid) == FALSE)
	{
		IFontManager* pFontMgr = IFactory<IFontManager>::CreateObject(uid);

		//init of FreeType, internal TexMgr,GraphicObjMgr
		auto pTexMgr = mFunction_GetTexMgrInsideFontMgr();
		auto pGObjMgr = mFunction_GetGObjMgrInsideFontMgr();

		BOOL isSucceeded = pFontMgr->mFunction_Init(pTexMgr,pGObjMgr);
		if (isSucceeded)
		{
			return pFontMgr;
		}
		else
		{
			IFactory<IFontManager>::DestroyObject(uid);
			ERROR_MSG("IScene: Renderer Initialization failed.");
			return nullptr;
		}
	}
	return IFactory<IFontManager>::GetObjectPtr(uid);
}

IModelLoader * IScene::GetModelLoader()
{
	const N_UID uid = "sceneModelLoader";
	if (IFactory<IModelLoader>::FindUid(uid) == FALSE)
	{
		IFactory<IModelLoader>::CreateObject(uid);
	}
	return IFactory<IModelLoader>::GetObjectPtr(uid);
}



/************************************************************************
                                          P R I V A T E                       
************************************************************************/

ITextureManager * IScene::mFunction_GetTexMgrInsideFontMgr()
{
	//get internal texMgr singleton instance
	const N_UID uid = "TexMgrOfFont";
	if (IFactory<ITextureManager>::FindUid(uid) == FALSE)
	{
		IFactory<ITextureManager>::CreateObject(uid);
	}
	return IFactory<ITextureManager>::GetObjectPtr(uid);
}

IGraphicObjectManager * IScene::mFunction_GetGObjMgrInsideFontMgr()
{
	//get internal GObjMgr singleton instance
	const N_UID uid = "GObjMgrOfFont";
	if (IFactory<IGraphicObjectManager>::FindUid(uid) == FALSE)
	{
		IFactory<IGraphicObjectManager>::CreateObject(uid);
	}
	return IFactory<IGraphicObjectManager>::GetObjectPtr(uid);
}

 IScene * Noise3D::GetScene()
{
	return GetRoot()->GetScenePtr();
}
