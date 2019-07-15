#pragma once

using namespace Noise3D;

class SceneLoader
{
public:

	void Init(SceneManager* pMgr);

	void LoadScene_Mesh(Camera* pCam);

	void LoadScene_DiffuseDemo(Camera * pCam);

	void LoadScene_DiffuseDemoOrenNayar(Camera * pCam);

	void LoadScene_RefractionDemo(Camera * pCam);

	void LoadScene_AreaLightingDemo(Camera * pCam);

	void LoadScene_StandardShader(Camera * pCam);

	void LoadScene_MicrofacetTransmissionTest(Camera * pCam);

	void LoadScene_Ironman(Camera * pCam);

	void LoadScene_IronmanAndAvenger(Camera* pCam);

	void LoadScene_IronmanCloseUp(Camera* pCam);

	void LoadScene_ShieldAndAvenger(Camera* pCam);

	void LoadScene_GoldenPorsche(Camera * pCam);

	void LoadScene_GlassRabbit(Camera * pCam);

	void LoadScene_Lamborghini(Camera * pCam);

	void LoadScene_Buddha(Camera * pCam);

	void LoadScene_M4A1(Camera * pCam);

	void LoadScene_Dragon(Camera * pCam);

	void LoadScene_Boxes(Camera * pCam);

	void LoadScene_SHDiffuseExperiment(Camera * pCam);

	//(2019.5.19) my graduation paper's experiment
	void LoadScene_PaperExpr6_2_1_1(Camera * pCam);

	void LoadScene_PaperExpr6_2_1_2(Camera * pCam);

	void LoadScene_PaperExpr6_2_2(Camera * pCam);

	void LoadScene_PaperExpr6_2_3(Camera * pCam);

	void LoadScene_PaperExpr6_2_4(Camera * pCam);

	void LoadScene_PaperExpr6_3_1(Camera * pCam);

	void LoadScene_PaperExpr6_3_2(Camera * pCam);

	void SetSkyLightType(GI::NOISE_PATH_TRACER_SKYLIGHT_TYPE type);

	void SetSkyLightMultiplier(float m);

	void SetAmbientRadiance(GI::Radiance r);


private:

	void _LoadTextures();

	void _LoadLambertMaterials();

	void _LoadAdvancedMaterials();

	void _LoadSphere(SceneGraph& sg,Vec3 pos, float radius, N_UID matUid);

	LogicalBox* _LoadBox(SceneGraph& sg,Vec3 pos, Vec3 size, N_UID matUid);

	void _LoadRect(SceneGraph& sg,NOISE_RECT_ORIENTATION ori, Vec3 pos, Vec2 size, N_UID matUid);

	Mesh* _LoadMeshSTL(SceneGraph& sg, NFilePath filePath, Vec3 pos, N_UID matUid);

	Mesh* _LoadMeshOBJ(SceneGraph& sg, NFilePath filePath, Vec3 pos, N_UID matUid);

	void _LoadMeshFBX(SceneGraph& sg, NFilePath filePath,N_SceneLoadingResult& outRes);

protected:

	SceneManager* m_pScene;
	MeshManager* m_pMeshMgr;
	MeshLoader* m_pModelLoader;
	MaterialManager*	m_pMatMgr;
	TextureManager*	m_pTexMgr;
	LogicalShapeManager* m_pShapeMgr;
	std::vector<Mesh*> mRealTimeRenderMeshList;

	GI::NOISE_PATH_TRACER_SKYLIGHT_TYPE mSkyLightType;
	float mSkyLightMultiplier;
	GI::Radiance mAmbientRadiance;
};