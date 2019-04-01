
/***********************************************************************

                           h£ºNoiseScene

************************************************************************/
#pragma once


namespace Noise3D
{
	class MeshLoader;
	namespace GI 
	{
		class PathTracer;
	};


	class /*_declspec(dllexport)*/ SceneManager:
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
		public IFactory<MeshLoader>,
		public IFactory<ModelProcessor>,
		public IFactory<CollisionTestor>,
		public IFactory<LogicalShapeManager>,
		public IFactory<GI::PathTracer>
	{
	public:

		void								ReleaseAllChildObject();

		//get ref to scene graph
		SceneGraph&				GetSceneGraph();

		Renderer*					CreateRenderer(UINT BufferWidth, UINT BufferHeight, HWND renderWindowHWND);

		Renderer*					GetRenderer();

		Camera*						GetCamera();

		MeshManager*			GetMeshMgr();

		LightManager*			GetLightMgr();

		TextureManager*		GetTextureMgr();

		MaterialManager*		GetMaterialMgr();

		SweepingTrailManager*	GetSweepingTraillMgr();

		GraphicObjectManager*	GetGraphicObjMgr();

		Atmosphere*				GetAtmosphere();

		TextManager*				GetTextMgr();

		MeshLoader*				GetModelLoader();

		ModelProcessor*			GetModelProcessor();

		CollisionTestor*			GetCollisionTestor();

		LogicalShapeManager* GetLogicalShapeMgr();

		GI::PathTracer*			CreatePathTracer(uint32_t pixelWidth, uint32_t pixelHeight);

		GI::PathTracer*			GetPathTracer();

	private:

		friend class IFactory<SceneManager>;

		SceneManager();

		~SceneManager();

		//a font manager has a texMgr/GObjMgr as internal objects
		TextureManager*	mFunction_GetTexMgrInsideFontMgr();

		GraphicObjectManager*	mFunction_GetGObjMgrInsideFontMgr();

		SceneGraph		mSceneGraph;

	};


		extern SceneManager* GetScene();

}