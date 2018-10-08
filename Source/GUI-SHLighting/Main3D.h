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

	void	SetCamProjType(bool isPerspective);

private:

	const char* c_originTexName = "Tex";
	const char* c_ShTexName = "ShTex";
	const int c_defaultTexWidth = 512;
	const Noise3D::NVECTOR3 c_ballPos1 = Noise3D::NVECTOR3(-1.5f, 0, 0);
	const Noise3D::NVECTOR3 c_ballPos2 = Noise3D::NVECTOR3(1.5f, 0, 0);

	void mFunction_SHPreprocess_SphericalMap(int shOrder, int monteCarloSampleCount, std::vector<Noise3D::NVECTOR3>& outShVector);

	void mFunction_SHPreprocess_CubeMap(int shOrder, int monteCarloSampleCount, std::vector<Noise3D::NVECTOR3>& outShVector);

	Noise3D::Root* m_pRoot;
	Noise3D::Renderer* m_pRenderer;
	Noise3D::SceneManager* m_pScene;
	Noise3D::Camera* m_pCamera;
	Noise3D::LightManager* m_pLightMgr;
	Noise3D::DirLightD* m_pLight;
	Noise3D::ModelLoader* m_pModelLoader;
	Noise3D::MeshManager* m_pMeshMgr;
	Noise3D::Mesh* m_pMeshOriginal;
	Noise3D::Mesh* m_pMeshSh;
	Noise3D::MaterialManager*	m_pMatMgr;
	Noise3D::TextureManager*	m_pTexMgr;
	Noise3D::Texture2D* m_pOriginTex;
	Noise3D::Texture2D* m_pShTex;
	Noise3D::GraphicObjectManager*	m_pGraphicObjMgr;
	Noise3D::GraphicObject* m_pGO_GUI;
	Noise3D::GraphicObject* m_pGO_Axis;
	Noise3D::TextManager* m_pTextMgr;
	Noise3D::DynamicText* m_pMyText_fps;
	Noise3D::DynamicText* m_pMyText_camProjType;

	Noise3D::Ut::Timer mTimer;// (Noise3D::Ut::NOISE_TIMER_TIMEUNIT_MILLISECOND);

	//object fixed, camera's orbit rotation is easy; but camera fixed, object's orbit rotation can't be dealt with trivially
	float mOrbitYaw;
	float mOrbitPitch;
};
