
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
		protected IFactory<Renderer>,
		protected IFactory<Camera>,
		protected IFactory<MeshManager>,
		protected IFactory<LightManager>,
		protected IFactory<TextureManager>,
		protected IFactory<MaterialManager>,
		protected IFactory<GraphicObjectManager>,
		protected IFactory<SweepingTrailManager>,
		protected IFactory<Atmosphere>,
		protected IFactory<TextManager>,
		protected IFactory<MeshLoader>,
		protected IFactory<ModelProcessor>,
		protected IFactory<CollisionTestor>,
		protected IFactory<LogicalShapeManager>,
		protected IFactory<GI::PathTracer>
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

		MeshLoader*				GetMeshLoader();

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