#pragma once

using namespace Noise3D;

class MainApp
{
public:
	
	MainApp();

	~MainApp();

	void Init(HINSTANCE hInstance);

	void RunMainloop();

private:
	
	//Real time stuffs
	void InitRealTime3D(HWND hwnd);

	void InitMgrsAndRenderers(HWND hwnd);

	void InitTextures();

	void InitMaterials();

	void InitCamera();

	void InitAtmos();

	void InitSceneNodes();

	void InitSceneObjects();

	void InitGraphicObjects();

	void InitText();

	void InitLight_RealTime();

private:

	void Init_GI();

	void InitPathTracer();

	void InitPathTracingSceneObject();

	void InitAreaLight();

private:

	enum MAINLOOP_STATE
	{
		PREVIEW,
		PATH_TRACING_RESULT
	};

	void Mainloop();

	void Mainloop_RealTimePreview();

	void Mainloop_RenderPathTracerResult();

	void InputProcess();

	MAINLOOP_STATE mMainloopState;

private:

	//***manager**
	Root* m_pRoot;
	Renderer* m_pRenderer;
	SceneManager* m_pScene;
	Camera* m_pCamera;
	Atmosphere* m_pAtmos;
	MeshLoader* m_pModelLoader;
	MeshManager* m_pMeshMgr;
	MaterialManager*	m_pMatMgr;
	TextureManager*	m_pTexMgr;
	GraphicObjectManager*	m_pGraphicObjMgr;
	LogicalShapeManager* m_pShapeMgr;
	LightManager* m_pLightMgr;
	TextManager* m_pTextMgr;

	//**SceneNode**
	SceneNode* m_pSnode_Sphere;
	SceneNode* m_pSnode_Box;

	//**GI**
	GI::PathTracer* m_pPathTracer;

	//**object**
	GraphicObject*	m_pGraphicObjBuffer;
	DynamicText*		m_pMyText_fps;
	std::vector<Mesh*> mMeshList;
	std::vector<ILogicalShape*> mShapeList;

	//**Ut**
	Ut::Timer mTimer;
	Ut::InputEngine mInputE;




};