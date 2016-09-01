
/***********************************************************************

                           h£ºNoiseScene

************************************************************************/

#pragma once

namespace Noise3D
{

		class /*_declspec(dllexport)*/ IScene:
		public IFactory<IRenderer>,
		public IFactory<ICamera>,
		public IFactory<IMeshManager>,
		public IFactory<ILightManager>,
		public IFactory<ITextureManager>,
		public IFactory<IMaterialManager>,
		public IFactory<IGraphicObjectManager>,
		public IFactory<IAtmosphere>,
		public IFactory<IFontManager>,
		public IFactory<IModelLoader>
	{

	public:

		void				ReleaseAllChildObject();

		IRenderer*				GetRenderer();

		ICamera*					GetCamera();

		IMeshManager*		GetMeshMgr();

		ILightManager*			GetLightMgr();

		ITextureManager*		GetTextureMgr();

		IMaterialManager*	GetMaterialMgr();

		IAtmosphere*			GetAtmosphere();

		IGraphicObjectManager*		GetGraphicObjMgr();

		IFontManager*				GetFontMgr();

		IModelLoader*				GetModelLoader();

	private:

		friend class IFactory<IScene>;

		IScene();

		~IScene();

		//a font manager has a texMgr/GObjMgr as internal objects
		ITextureManager*				mFunction_GetTexMgrInsideFontMgr();

		IGraphicObjectManager*	mFunction_GetGObjMgrInsideFontMgr();

	};


		extern IScene* GetScene();

}