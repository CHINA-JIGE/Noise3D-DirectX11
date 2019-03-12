
/***********************************************************************

                           class£ºSceneManger

			desc£ºfactory of many manager object£¨MESH,LIGHT,MATERIAL,TEXTURE...£©
					
************************************************************************/

//!!!!!!!!!IMPORTANT : when a new class need to be bound to SceneManager,remember to modify
// 1. Inheritance
// 2. Destructor
// 3. Corresponding Creation method

#include "Noise3D.h"

using namespace Noise3D;

SceneManager::SceneManager():
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

SceneManager::~SceneManager()
{
	ReleaseAllChildObject();
}


void	SceneManager::ReleaseAllChildObject()
{
	//(2019.3.12)but there is a problem, that ISceneObject is derived from SceneNode
	//and when SceneGraph call its destructor, all the scene Node/Objects 's base ptr will be deleted
	//

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

SceneGraph & Noise3D::SceneManager::GetSceneGraph()
{
	return mSceneGraph;
}

//first time to init RENDERER
Renderer * SceneManager::CreateRenderer(UINT BufferWidth, UINT BufferHeight,HWND renderWindowHWND)
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
			ERROR_MSG("SceneManager: Renderer Initialization failed.");
			return nullptr;
		}
	}
	return IFactory<Renderer>::GetObjectPtr(uid);
}

Renderer * SceneManager::GetRenderer()
{
	static const N_UID uid = "sceneRenderer";
	if (IFactory<Renderer>::FindUid(uid) == false)
	{
		ERROR_MSG("SceneManager: GetRenderer() : Renderer must be initialized by CreateRenderer() method.");
		return nullptr;
	}
	else
	{
		//return initialized renderer ptr
		return IFactory<Renderer>::GetObjectPtr(uid);
	}
};


MeshManager * SceneManager::GetMeshMgr()
{
	static const N_UID uid = "sceneMeshMgr";
	if (IFactory<MeshManager>::FindUid(uid) == false)
	{
		IFactory<MeshManager>::CreateObject(uid);
	}
	return IFactory<MeshManager>::GetObjectPtr(uid);
};


Camera * SceneManager::GetCamera()
{
	const N_UID uid = "sceneCamera";
	if (IFactory<Camera>::FindUid(uid) == false)
	{
		Camera* pCam = IFactory<Camera>::CreateObject(uid);

		//init scene object info(necessary for class derived from ISceneObject)
		SceneNode* pNode =  mSceneGraph.GetRoot()->CreateChildNode();
		pCam->ISceneObject::mFunc_InitSceneObject(uid, pNode);
	}
	return IFactory<Camera>::GetObjectPtr(uid);
}

LightManager * SceneManager::GetLightMgr()
{
	const N_UID uid = "sceneLightMgr";
	if (IFactory<LightManager>::FindUid(uid) == false)
	{
		IFactory<LightManager>::CreateObject(uid);
	}
	return IFactory<LightManager>::GetObjectPtr(uid);
}

TextureManager * SceneManager::GetTextureMgr()
{
	const N_UID uid = "sceneTexMgr";
	if (IFactory<TextureManager>::FindUid(uid) == false)
	{
		IFactory<TextureManager>::CreateObject(uid);
	}
	return IFactory<TextureManager>::GetObjectPtr(uid);
}

MaterialManager * SceneManager::GetMaterialMgr()
{
	const N_UID uid = "sceneMatMgr";
	if (IFactory<MaterialManager>::FindUid(uid) == false)
	{
		IFactory<MaterialManager>::CreateObject(uid);
	}
	return IFactory<MaterialManager>::GetObjectPtr(uid);
}

SweepingTrailManager * Noise3D::SceneManager::GetSweepingTraillMgr()
{
	const N_UID uid = "sceneSweepingTrailMgr";
	if (IFactory<SweepingTrailManager>::FindUid(uid) == false)
	{
		IFactory<SweepingTrailManager>::CreateObject(uid);
	}
	return IFactory<SweepingTrailManager>::GetObjectPtr(uid);
}

Atmosphere * SceneManager::GetAtmosphere()
{
	const N_UID uid = "sceneAtmos";
	if (IFactory<Atmosphere>::FindUid(uid) == false)
	{
		IFactory<Atmosphere>::CreateObject(uid);
	}
	return IFactory<Atmosphere>::GetObjectPtr(uid);
}

GraphicObjectManager * SceneManager::GetGraphicObjMgr()
{
	const N_UID uid = "sceneGObjMgr";
	if (IFactory<GraphicObjectManager>::FindUid(uid) == false)
	{
		IFactory<GraphicObjectManager>::CreateObject(uid);
	}
	return IFactory<GraphicObjectManager>::GetObjectPtr(uid);
}

TextManager * SceneManager::GetTextMgr()
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
			ERROR_MSG("SceneManager: Font Manager Initialization failed.");
			return nullptr;
		}
	}
	return IFactory<TextManager>::GetObjectPtr(uid);
}

ModelLoader * SceneManager::GetModelLoader()
{
	const N_UID uid = "sceneModelLoader";
	if (IFactory<ModelLoader>::FindUid(uid) == false)
	{
		IFactory<ModelLoader>::CreateObject(uid);
	}
	return IFactory<ModelLoader>::GetObjectPtr(uid);
}

ModelProcessor * Noise3D::SceneManager::GetModelProcessor()
{
	const N_UID uid = "sceneModelProcessor";
	if (IFactory<ModelProcessor>::FindUid(uid) == false)
	{
		IFactory<ModelProcessor>::CreateObject(uid);
	}
	return IFactory<ModelProcessor>::GetObjectPtr(uid);
}

CollisionTestor * SceneManager::GetCollisionTestor()
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
			ERROR_MSG("SceneManager: Collision Testor Initialization failed.");
			return nullptr;
		}
	}
	return IFactory<CollisionTestor>::GetObjectPtr(uid);
}



/************************************************************************
                                          P R I V A T E                       
************************************************************************/

TextureManager * SceneManager::mFunction_GetTexMgrInsideFontMgr()
{
	//get internal texMgr singleton instance
	const N_UID uid = "TexMgrOfFont";
	if (IFactory<TextureManager>::FindUid(uid) == false)
	{
		IFactory<TextureManager>::CreateObject(uid);
	}
	return IFactory<TextureManager>::GetObjectPtr(uid);
}

GraphicObjectManager * SceneManager::mFunction_GetGObjMgrInsideFontMgr()
{
	//get internal GObjMgr singleton instance
	const N_UID uid = "GObjMgrOfFont";
	if (IFactory<GraphicObjectManager>::FindUid(uid) == false)
	{
		IFactory<GraphicObjectManager>::CreateObject(uid);
	}
	return IFactory<GraphicObjectManager>::GetObjectPtr(uid);
}

 SceneManager * Noise3D::GetScene()
{
	return GetRoot()->GetSceneMgrPtr();
}
