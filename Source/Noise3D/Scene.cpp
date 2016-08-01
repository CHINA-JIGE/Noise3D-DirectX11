
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
	 IFactory<ITextureManager>(1),
	 IFactory<IMaterialManager>(1),
	IFactory<IGraphicObjectManager>(1),
	 IFactory<IAtmosphere>(1)
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
}

IMeshManager * IScene::GetMeshMgr()
{
	static const N_UID uid = "myMeshMgr";
	if (IFactory<IMeshManager>::GetObjectCount() == 0)
	{
		IFactory<IMeshManager>::CreateObject(uid);
	}
	return IFactory<IMeshManager>::GetObjectPtr(uid);
};


IRenderer * IScene::GetRenderer()
{
	static const N_UID uid = "myRenderer";
	if (IFactory<IRenderer>::GetObjectCount() == 0)
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
}

ICamera * IScene::GetCamera()
{
	static const N_UID uid = "myCamera";
	if (IFactory<ICamera>::GetObjectCount() == 0)
	{
		IFactory<ICamera>::CreateObject(uid);
	}
	return IFactory<ICamera>::GetObjectPtr(uid);
}

ILightManager * IScene::GetLightMgr()
{
	static const N_UID uid = "myLightMgr";
	if (IFactory<ILightManager>::GetObjectCount() == 0)
	{
		IFactory<ILightManager>::CreateObject(uid);
	}
	return IFactory<ILightManager>::GetObjectPtr(uid);
}

ITextureManager * IScene::GetTextureMgr()
{
	static const N_UID uid = "myTexMgr";
	if (IFactory<ITextureManager>::GetObjectCount() == 0)
	{
		IFactory<ITextureManager>::CreateObject(uid);
	}
	return IFactory<ITextureManager>::GetObjectPtr(uid);
}

IMaterialManager * IScene::GetMaterialMgr()
{
	static const N_UID uid = "myMatMgr";
	if (IFactory<IMaterialManager>::GetObjectCount() == 0)
	{
		IFactory<IMaterialManager>::CreateObject(uid);
	}
	return IFactory<IMaterialManager>::GetObjectPtr(uid);
}

IAtmosphere * IScene::GetAtmosphere()
{
	static const N_UID uid = "myAtmos";
	if (IFactory<IAtmosphere>::GetObjectCount() == 0)
	{
		IFactory<IAtmosphere>::CreateObject(uid);
	}
	return IFactory<IAtmosphere>::GetObjectPtr(uid);
}

IGraphicObjectManager * IScene::GetGraphicObjMgr()
{
	static const N_UID uid = "myGObjMgr";
	if (IFactory<IGraphicObjectManager>::GetObjectCount() == 0)
	{
		IFactory<IGraphicObjectManager>::CreateObject(uid);
	}
	return IFactory<IGraphicObjectManager>::GetObjectPtr(uid);
}



/************************************************************************
                                          P R I V A T E                       
************************************************************************/
