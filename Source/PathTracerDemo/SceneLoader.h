#pragma once

using namespace Noise3D;

class SceneLoader
{
public:

	void Init(SceneManager* pMgr);

	void LoadScene_Mesh(Camera* pCam);

	void LoadScene_DiffuseDemo(Camera * pCam);

	void LoadScene_RefractionDemo(Camera * pCam);

	void LoadScene_AreaLightingDemo(Camera * pCam);

	void LoadScene_StandardShader(Camera * pCam);

	void LoadScene_Ironman(Camera * pCam);

	void LoadScene_Porsche(Camera * pCam);

	void LoadScene_Buddha(Camera * pCam);

private:

	void _LoadTextures();

	void _LoadLambertMaterials();

	void _LoadAdvancedMaterials();

	void _LoadSphere(SceneGraph& sg,Vec3 pos, float radius, N_UID matUid);

	void _LoadBox(SceneGraph& sg,Vec3 pos, Vec3 size, N_UID matUid);

	void _LoadRect(SceneGraph& sg,NOISE_RECT_ORIENTATION ori, Vec3 pos, Vec2 size, N_UID matUid);

	Mesh* _LoadMeshSTL(SceneGraph& sg, NFilePath filePath, Vec3 pos, N_UID matUid);

	Mesh* _LoadMeshOBJ(SceneGraph& sg, NFilePath filePath, Vec3 pos, N_UID matUid);


protected:

	SceneManager* m_pScene;
	MeshManager* m_pMeshMgr;
	MeshLoader* m_pModelLoader;
	MaterialManager*	m_pMatMgr;
	TextureManager*	m_pTexMgr;
	LogicalShapeManager* m_pShapeMgr;
	std::vector<Mesh*> mRealTimeRenderMeshList;
};