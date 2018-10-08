
/***********************************************************************

                           h£ºNoiseScene

************************************************************************/

#pragma once

namespace Noise3D
{
	class IModelLoader;

		class /*_declspec(dllexport)*/ IScene:
		public IFactory<IRenderer>,
		public IFactory<ICamera>,
		public IFactory<IMeshManager>,
		public IFactory<ILightManager>,
		public IFactory<TextureManager>,
		public IFactory<IMaterialManager>,
		public IFactory<IGraphicObjectManager>,
		public IFactory<ISweepingTrailManager>,
		public IFactory<Atmosphere>,
		public IFactory<IFontManager>,
		public IFactory<IModelLoader>,
		public IFactory<IModelProcessor>,
		public IFactory<ICollisionTestor>
	{
	public:

		void								ReleaseAllChildObject();

		IRenderer*					CreateRenderer(UINT BufferWidth, UINT BufferHeight, HWND renderWindowHWND);

		IRenderer*					GetRenderer();

		ICamera*						GetCamera();

		IMeshManager*			GetMeshMgr();

		ILightManager*			GetLightMgr();

		TextureManager*		GetTextureMgr();

		IMaterialManager*		GetMaterialMgr();

		ISweepingTrailManager*		GetSweepingTraillMgr();

		IGraphicObjectManager*	GetGraphicObjMgr();

		Atmosphere*					GetAtmosphere();

		IFontManager*				GetFontMgr();

		IModelLoader*				GetModelLoader();

		IModelProcessor*			GetModelProcessor();

		ICollisionTestor*				GetCollisionTestor();

	private:

		friend class IFactory<IScene>;

		IScene();

		~IScene();

		//a font manager has a texMgr/GObjMgr as internal objects
		TextureManager*				mFunction_GetTexMgrInsideFontMgr();

		IGraphicObjectManager*	mFunction_GetGObjMgrInsideFontMgr();

	};


		extern IScene* GetScene();

}