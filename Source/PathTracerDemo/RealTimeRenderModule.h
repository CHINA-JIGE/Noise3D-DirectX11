#pragma once

#include  "SceneLoader.h"

using namespace Noise3D;

class RealTimeRenderModule:
	private IMainloop,
	public SceneLoader
{
public:
	
	RealTimeRenderModule();

	~RealTimeRenderModule();

	void Init(HINSTANCE hInstance);

	void RunMainloop();

private:
	
	//Real time stuffs
	void _InitRealTime3D(HWND hwnd);

	void _InitMgrsAndRenderers(HWND hwnd);

	void _InitScene();

	void _InitAtmos();

	void _InitGraphicObjects();

	void _InitText();

	void _InitLight_RealTime();

protected:

	virtual void Callback_Mainloop() override=0;

protected:

	//***manager**
	Root* m_pRoot;
	Renderer* m_pRenderer;
	Camera* m_pCamera;
	Atmosphere* m_pAtmos;
	GraphicObjectManager*	m_pGraphicObjMgr;
	LightManager* m_pLightMgr;
	TextManager* m_pTextMgr;

	//**object**
	GraphicObject*	m_pGraphicObjBuffer;
	DynamicText*		m_pMyText_fps;

	//**Ut**
	Ut::Timer mTimer;
	Ut::InputEngine mInputE;




};