
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
	 IFactory<Renderer>(1),
	 IFactory<Camera>(1),
	IFactory<MeshManager>(1),
	 IFactory<LightManager>(1),
	 IFactory<TextureManager>(2),//scene/font-internal
	 IFactory<MaterialManager>(1),
	IFactory<SweepingTrailManager>(1),
	IFactory<GraphicObjectManager>(2),//scene/font-internal
	 IFactory<Atmosphere>(1),
	IFactory<TextManager>(1),
	IFactory<ModelLoader>(1),
	IFactory<ModelProcessor>(1),
	IFactory<CollisionTestor>(1)
{

}

IScene::~IScene()
{
	ReleaseAllChildObject();
}


void	IScene::ReleaseAllChildObject()
{
	IFactory<MeshManager>::DestroyAllObject();
	IFactory<Renderer>::DestroyAllObject();
	IFactory<Camera>::DestroyAllObject();
	IFactory<LightManager>::DestroyAllObject();
	IFactory<TextureManager>::DestroyAllObject();
	IFactory<MaterialManager>::DestroyAllObject();
	IFactory<Atmosphere>::DestroyAllObject();
	IFactory<GraphicObjectManager>::DestroyAllObject();
	IFactory<CollisionTestor>::DestroyAllObject();
}

//first time to init RENDERER
Renderer * IScene::CreateRenderer(UINT BufferWidth, UINT BufferHeight,HWND renderWindowHWND)
{
	static const N_UID uid = "sceneRenderer";
	if (IFactory<Renderer>::FindUid(uid) == false)
	{
		Renderer* pRd = IFactory<Renderer>::CreateObject(uid);

		//init of shaders/RV/states/....
		bool isSucceeded = pRd->mFunction_Init(BufferWidth,BufferHeight, renderWindowHWND);
		if (isSucceeded)
		{
			return pRd;
		}
		else
		{
			IFactory<Renderer>::DestroyObject(uid);
			ERROR_MSG("IScene: Renderer Initialization failed.");
			return nullptr;
		}
	}
	return IFactory<Renderer>::GetObjectPtr(uid);
}

Renderer * IScene::GetRenderer()
{
	static const N_UID uid = "sceneRenderer";
	if (IFactory<Renderer>::FindUid(uid) == false)
	{
		ERROR_MSG("IScene: GetRenderer() : Renderer must be initialized by CreateRenderer() method.");
		return nullptr;
	}
	else
	{
		//return initialized renderer ptr
		return IFactory<Renderer>::GetObjectPtr(uid);
	}
};


MeshManager * IScene::GetMeshMgr()
{
	static const N_UID uid = "sceneMeshMgr";
	if (IFactory<MeshManager>::FindUid(uid) == false)
	{
		IFactory<MeshManager>::CreateObject(uid);
	}
	return IFactory<MeshManager>::GetObjectPtr(uid);
};


Camera * IScene::GetCamera()
{
	const N_UID uid = "sceneCamera";
	if (IFactory<Camera>::FindUid(uid) == false)
	{
		IFactory<Camera>::CreateObject(uid);
	}
	return IFactory<Camera>::GetObjectPtr(uid);
}

LightManager * IScene::GetLightMgr()
{
	const N_UID uid = "sceneLightMgr";
	if (IFactory<LightManager>::FindUid(uid) == false)
	{
		IFactory<LightManager>::CreateObject(uid);
	}
	return IFactory<LightManager>::GetObjectPtr(uid);
}

TextureManager * IScene::GetTextureMgr()
{
	const N_UID uid = "sceneTexMgr";
	if (IFactory<TextureManager>::FindUid(uid) == false)
	{
		IFactory<TextureManager>::CreateObject(uid);
	}
	return IFactory<TextureManager>::GetObjectPtr(uid);
}

MaterialManager * IScene::GetMaterialMgr()
{
	const N_UID uid = "sceneMatMgr";
	if (IFactory<MaterialManager>::FindUid(uid) == false)
	{
		IFactory<MaterialManager>::CreateObject(uid);
	}
	return IFactory<MaterialManager>::GetObjectPtr(uid);
}

SweepingTrailManager * Noise3D::IScene::GetSweepingTraillMgr()
{
	const N_UID uid = "sceneSweepingTrailMgr";
	if (IFactory<SweepingTrailManager>::FindUid(uid) == false)
	{
		IFactory<SweepingTrailManager>::CreateObject(uid);
	}
	return IFactory<SweepingTrailManager>::GetObjectPtr(uid);
}

Atmosphere * IScene::GetAtmosphere()
{
	const N_UID uid = "sceneAtmos";
	if (IFactory<Atmosphere>::FindUid(uid) == false)
	{
		IFactory<Atmosphere>::CreateObject(uid);
	}
	return IFactory<Atmosphere>::GetObjectPtr(uid);
}

GraphicObjectManager * IScene::GetGraphicObjMgr()
{
	const N_UID uid = "sceneGObjMgr";
	if (IFactory<GraphicObjectManager>::FindUid(uid) == false)
	{
		IFactory<GraphicObjectManager>::CreateObject(uid);
	}
	return IFactory<GraphicObjectManager>::GetObjectPtr(uid);
}

TextManager * IScene::GetFontMgr()
{
	const N_UID uid = "sceneFontMgr";
	if (IFactory<TextManager>::FindUid(uid) == false)
	{
		TextManager* pFontMgr = IFactory<TextManager>::CreateObject(uid);

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
			IFactory<TextManager>::DestroyObject(uid);
			ERROR_MSG("IScene: Font Manager Initialization failed.");
			return nullptr;
		}
	}
	return IFactory<TextManager>::GetObjectPtr(uid);
}

ModelLoader * IScene::GetModelLoader()
{
	const N_UID uid = "sceneModelLoader";
	if (IFactory<ModelLoader>::FindUid(uid) == false)
	{
		IFactory<ModelLoader>::CreateObject(uid);
	}
	return IFactory<ModelLoader>::GetObjectPtr(uid);
}

ModelProcessor * Noise3D::IScene::GetModelProcessor()
{
	const N_UID uid = "sceneModelProcessor";
	if (IFactory<ModelProcessor>::FindUid(uid) == false)
	{
		IFactory<ModelProcessor>::CreateObject(uid);
	}
	return IFactory<ModelProcessor>::GetObjectPtr(uid);
}

CollisionTestor * IScene::GetCollisionTestor()
{
	const N_UID uid = "sceneCollisionTestor";
	if (IFactory<CollisionTestor>::FindUid(uid) == false)
	{
		CollisionTestor* pCT = IFactory<CollisionTestor>::CreateObject(uid);

		//init of FreeType, internal TexMgr,GraphicObjMgr

		bool isSucceeded = pCT->mFunction_Init();
		if (isSucceeded)
		{
			return pCT;
		}
		else
		{
			IFactory<CollisionTestor>::DestroyObject(uid);
			ERROR_MSG("IScene: Collision Testor Initialization failed.");
			return nullptr;
		}
	}
	return IFactory<CollisionTestor>::GetObjectPtr(uid);
}



/************************************************************************
                                          P R I V A T E                       
************************************************************************/

TextureManager * IScene::mFunction_GetTexMgrInsideFontMgr()
{
	//get internal texMgr singleton instance
	const N_UID uid = "TexMgrOfFont";
	if (IFactory<TextureManager>::FindUid(uid) == false)
	{
		IFactory<TextureManager>::CreateObject(uid);
	}
	return IFactory<TextureManager>::GetObjectPtr(uid);
}

GraphicObjectManager * IScene::mFunction_GetGObjMgrInsideFontMgr()
{
	//get internal GObjMgr singleton instance
	const N_UID uid = "GObjMgrOfFont";
	if (IFactory<GraphicObjectManager>::FindUid(uid) == false)
	{
		IFactory<GraphicObjectManager>::CreateObject(uid);
	}
	return IFactory<GraphicObjectManager>::GetObjectPtr(uid);
}

 IScene * Noise3D::GetScene()
{
	return GetRoot()->GetScenePtr();
}
