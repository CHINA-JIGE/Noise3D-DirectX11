/***************************************************

				Render Area (using Noise3d)

***************************************************/
#pragma once

#include <Noise3D.h>

enum SH_TEXTURE_TYPE
{
	COMMON,
	CUBE_MAP
};

class Main3DApp
{
public:

	Main3DApp();

	~Main3DApp();

	bool InitNoise3D(HWND renderCanvasHwnd, HWND inputHwnd, UINT canvasWidth, UINT canvasHeight);

	void UpdateFrame();//invoked per frame

	bool LoadOriginalTexture(std::string filePath);

	bool ComputeShTexture(SH_TEXTURE_TYPE texType, int shOrder, int monteCarloSampleCount, std::vector<Noise3D::NVECTOR3>& outShVector);

	void RotateBall(int index, float deltaYaw, float deltaPitch);

	void Cleanup();

private:

	const char* c_originTexName = "Tex";
	const char* c_ShTexName = "ShTex";
	const int c_defaultTexWidth = 512;

	void mFunction_SHPreprocess_SphericalMap(int shOrder, int monteCarloSampleCount, std::vector<Noise3D::NVECTOR3>& outShVector);

	void mFunction_SHPreprocess_CubeMap(int shOrder, int monteCarloSampleCount, std::vector<Noise3D::NVECTOR3>& outShVector);

	Noise3D::IRoot* m_pRoot;
	Noise3D::IRenderer* m_pRenderer;
	Noise3D::IScene* m_pScene;
	Noise3D::ICamera* m_pCamera;
	Noise3D::ILightManager* m_pLightMgr;
	Noise3D::IDirLightD* m_pLight;
	Noise3D::IModelLoader* m_pModelLoader;
	Noise3D::IMeshManager* m_pMeshMgr;
	Noise3D::IMesh* m_pMeshOriginal;
	Noise3D::IMesh* m_pMeshSh;
	Noise3D::IMaterialManager*	m_pMatMgr;
	Noise3D::ITextureManager*	m_pTexMgr;
	Noise3D::ITexture* m_pOriginTex;
	Noise3D::ITexture* m_pShTex;
	Noise3D::IGraphicObjectManager*	m_pGraphicObjMgr;
	Noise3D::IGraphicObject*	m_pGO_GUI;
	Noise3D::IFontManager* m_pFontMgr;
	Noise3D::IDynamicText* m_pMyText_fps;

	Noise3D::Ut::Timer mTimer;// (Noise3D::Ut::NOISE_TIMER_TIMEUNIT_MILLISECOND);
	//Noise3D::Ut::InputEngine mInputE;
};
