
/***********************************************************************

                           h£ºNoiseScene

************************************************************************/

#pragma once

namespace Noise3D
{
	class ModelLoader;

		class /*_declspec(dllexport)*/ IScene:
		public IFactory<Renderer>,
		public IFactory<Camera>,
		public IFactory<MeshManager>,
		public IFactory<LightManager>,
		public IFactory<TextureManager>,
		public IFactory<MaterialManager>,
		public IFactory<GraphicObjectManager>,
		public IFactory<SweepingTrailManager>,
		public IFactory<Atmosphere>,
		public IFactory<TextManager>,
		public IFactory<ModelLoader>,
		public IFactory<ModelProcessor>,
		public IFactory<CollisionTestor>
	{
	public:

		void								ReleaseAllChildObject();

		Renderer*					CreateRenderer(UINT BufferWidth, UINT BufferHeight, HWND renderWindowHWND);

		Renderer*					GetRenderer();

		Camera*						GetCamera();

		MeshManager*			GetMeshMgr();

		LightManager*			GetLightMgr();

		TextureManager*		GetTextureMgr();

		MaterialManager*		GetMaterialMgr();

		SweepingTrailManager*		GetSweepingTraillMgr();

		GraphicObjectManager*	GetGraphicObjMgr();

		Atmosphere*					GetAtmosphere();

		TextManager*				GetFontMgr();

		ModelLoader*				GetModelLoader();

		ModelProcessor*			GetModelProcessor();

		CollisionTestor*				GetCollisionTestor();

	private:

		friend class IFactory<IScene>;

		IScene();

		~IScene();

		//a font manager has a texMgr/GObjMgr as internal objects
		TextureManager*				mFunction_GetTexMgrInsideFontMgr();

		GraphicObjectManager*	mFunction_GetGObjMgrInsideFontMgr();

	};


		extern IScene* GetScene();

}