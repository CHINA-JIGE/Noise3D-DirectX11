
/***********************************************************************

                           ¿‡£∫NOISE SceneManger

			ºÚ ˆ£∫Center of many manager object£®MESH,LIGHT,MATERIAL,TEXTURE...£©
					
************************************************************************/

//!!!!!!!!!IMPORTANT : when a new class need to be bound to IScene,remember to modify
// 1. Inheritance
// 2. Destructor
// 3. Corresponding Creation method

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
	IFactory<IModelLoader>(1),
	IFactory<IModelProcessor>(1),
	IFactory<ICollisionTestor>(1)
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
	IFactory<ICollisionTestor>::DestroyAllObject();
}

//first time to init RENDERER
IRenderer * IScene::CreateRenderer(UINT BufferWidth, UINT BufferHeight,HWND renderWindowHWND)
{
	static const N_UID uid = "sceneRenderer";
	if (IFactory<IRenderer>::FindUid(uid) == false)
	{
		IRenderer* pRd = IFactory<IRenderer>::CreateObject(uid);

		//init of shaders/RV/states/....
		bool isSucceeded = pRd->mFunction_Init(BufferWidth,BufferHeight, renderWindowHWND);
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
}

IRenderer * IScene::GetRenderer()
{
	static const N_UID uid = "sceneRenderer";
	if (IFactory<IRenderer>::FindUid(uid) == false)
	{
		ERROR_MSG("IScene: GetRenderer() : Renderer must be initialized by CreateRenderer() method.");
		return nullptr;
	}
	else
	{
		//return initialized renderer ptr
		return IFactory<IRenderer>::GetObjectPtr(uid);
	}
};


IMeshManager * IScene::GetMeshMgr()
{
	static const N_UID uid = "sceneMeshMgr";
	if (IFactory<IMeshManager>::FindUid(uid) == false)
	{
		IFactory<IMeshManager>::CreateObject(uid);
	}
	return IFactory<IMeshManager>::GetObjectPtr(uid);
};


ICamera * IScene::GetCamera()
{
	const N_UID uid = "sceneCamera";
	if (IFactory<ICamera>::FindUid(uid) == false)
	{
		IFactory<ICamera>::CreateObject(uid);
	}
	return IFactory<ICamera>::GetObjectPtr(uid);
}

ILightManager * IScene::GetLightMgr()
{
	const N_UID uid = "sceneLightMgr";
	if (IFactory<ILightManager>::FindUid(uid) == false)
	{
		IFactory<ILightManager>::CreateObject(uid);
	}
	return IFactory<ILightManager>::GetObjectPtr(uid);
}

ITextureManager * IScene::GetTextureMgr()
{
	const N_UID uid = "sceneTexMgr";
	if (IFactory<ITextureManager>::FindUid(uid) == false)
	{
		IFactory<ITextureManager>::CreateObject(uid);
	}
	return IFactory<ITextureManager>::GetObjectPtr(uid);
}

IMaterialManager * IScene::GetMaterialMgr()
{
	const N_UID uid = "sceneMatMgr";
	if (IFactory<IMaterialManager>::FindUid(uid) == false)
	{
		IFactory<IMaterialManager>::CreateObject(uid);
	}
	return IFactory<IMaterialManager>::GetObjectPtr(uid);
}

IAtmosphere * IScene::GetAtmosphere()
{
	const N_UID uid = "sceneAtmos";
	if (IFactory<IAtmosphere>::FindUid(uid) == false)
	{
		IFactory<IAtmosphere>::CreateObject(uid);
	}
	return IFactory<IAtmosphere>::GetObjectPtr(uid);
}

IGraphicObjectManager * IScene::GetGraphicObjMgr()
{
	const N_UID uid = "sceneGObjMgr";
	if (IFactory<IGraphicObjectManager>::FindUid(uid) == false)
	{
		IFactory<IGraphicObjectManager>::CreateObject(uid);
	}
	return IFactory<IGraphicObjectManager>::GetObjectPtr(uid);
}

IFontManager * IScene::GetFontMgr()
{
	const N_UID uid = "sceneFontMgr";
	if (IFactory<IFontManager>::FindUid(uid) == false)
	{
		IFontManager* pFontMgr = IFactory<IFontManager>::CreateObject(uid);

		//init of FreeType, internal TexMgr,GraphicObjMgr
		auto pTexMgr = mFunction_GetTexMgrInsideFontMgr();
		auto pGObjMgr = mFunction_GetGObjMgrInsideFontMgr();

		bool isSucceeded = pFontMgr->mFunction_Init(pTexMgr,pGObjMgr);
		if (isSucceeded)
		{
			return pFontMgr;
		}
		else
		{
			IFactory<IFontManager>::DestroyObject(uid);
			ERROR_MSG("IScene: Font Manager Initialization failed.");
			return nullptr;
		}
	}
	return IFactory<IFontManager>::GetObjectPtr(uid);
}

IModelLoader * IScene::GetModelLoader()
{
	const N_UID uid = "sceneModelLoader";
	if (IFactory<IModelLoader>::FindUid(uid) == false)
	{
		IFactory<IModelLoader>::CreateObject(uid);
	}
	return IFactory<IModelLoader>::GetObjectPtr(uid);
}

IModelProcessor * Noise3D::IScene::GetModelProcessor()
{
	const N_UID uid = "sceneModelProcessor";
	if (IFactory<IModelProcessor>::FindUid(uid) == false)
	{
		IFactory<IModelProcessor>::CreateObject(uid);
	}
	return IFactory<IModelProcessor>::GetObjectPtr(uid);
}

ICollisionTestor * IScene::GetCollisionTestor()
{
	const N_UID uid = "sceneCollisionTestor";
	if (IFactory<ICollisionTestor>::FindUid(uid) == false)
	{
		ICollisionTestor* pCT = IFactory<ICollisionTestor>::CreateObject(uid);

		//init of FreeType, internal TexMgr,GraphicObjMgr

		bool isSucceeded = pCT->mFunction_Init();
		if (isSucceeded)
		{
			return pCT;
		}
		else
		{
			IFactory<ICollisionTestor>::DestroyObject(uid);
			ERROR_MSG("IScene: Collision Testor Initialization failed.");
			return nullptr;
		}
	}
	return IFactory<ICollisionTestor>::GetObjectPtr(uid);
}



/************************************************************************
                                          P R I V A T E                       
************************************************************************/

ITextureManager * IScene::mFunction_GetTexMgrInsideFontMgr()
{
	//get internal texMgr singleton instance
	const N_UID uid = "TexMgrOfFont";
	if (IFactory<ITextureManager>::FindUid(uid) == false)
	{
		IFactory<ITextureManager>::CreateObject(uid);
	}
	return IFactory<ITextureManager>::GetObjectPtr(uid);
}

IGraphicObjectManager * IScene::mFunction_GetGObjMgrInsideFontMgr()
{
	//get internal GObjMgr singleton instance
	const N_UID uid = "GObjMgrOfFont";
	if (IFactory<IGraphicObjectManager>::FindUid(uid) == false)
	{
		IFactory<IGraphicObjectManager>::CreateObject(uid);
	}
	return IFactory<IGraphicObjectManager>::GetObjectPtr(uid);
}

 IScene * Noise3D::GetScene()
{
	return GetRoot()->GetScenePtr();
}
