#pragma once

using namespace Noise3D;

class RealTimeRenderModule:
	private IMainloop
{
public:
	
	RealTimeRenderModule();

	~RealTimeRenderModule();

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

protected:

	virtual void Callback_Mainloop() override=0;

protected:

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

	//**object**
	GraphicObject*	m_pGraphicObjBuffer;
	DynamicText*		m_pMyText_fps;
	std::vector<Mesh*> mMeshList;

	//**Ut**
	Ut::Timer mTimer;
	Ut::InputEngine mInputE;




};