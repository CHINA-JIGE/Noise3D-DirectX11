
/***********************************************************************

							   h£ºFBX loader
		Desc: encapsulation of  FBXSDK, loading FBX model/scene
		which will be integrated to IModelLoader

************************************************************************/

#pragma once

namespace Noise3D
{
	struct N_MeshSubsetInfo;
	struct N_BasicMaterialDesc;

	struct N_FbxTextureMapsInfo
	{
		std::string diffMapName;
		std::string diffMapFilePath;
		std::string normalMapName;
		std::string normalMapFilePath;
		std::string specMapName;
		std::string specMapFilePath;
		std::string emissiveMapName;
		std::string emissiveMapFilePath;
	};

	struct N_FbxMaterialInfo
	{
		std::string matName;
		N_BasicMaterialDesc matBasicInfo;
		N_FbxTextureMapsInfo texMapInfo;//including loading path and names of texture maps
	};

	struct N_FbxMeshInfo
	{
		N_FbxMeshInfo(){}

		N_UID	name;
		std::vector<N_DefaultVertex> vertexBuffer;
		std::vector<uint32_t> indexBuffer;
		std::vector<N_MeshSubsetInfo> subsetList;
		std::vector<N_FbxMaterialInfo> matList;
		NVECTOR3 pos;//world translation
		NVECTOR3 scale;
		NVECTOR3 rotation;
	};

	struct N_FbxSkeletonInfo
	{

	};

	struct N_FbxLoadingResult
	{
		N_FbxLoadingResult(){}

		std::vector<N_FbxMeshInfo>			meshDataList;
		std::vector<N_FbxSkeletonInfo>	skeletonList;
	};


	class /*_declspec(dllexport)*/ IFbxLoader
	{
	public:

		IFbxLoader();

		~IFbxLoader();

		bool		Initialize();

		//there are many types of scene nodes in fbx, but mesh/animation are the only two interested now(2017.9.20)
		bool		LoadSceneFromFbx(NFilePath fbxPath, N_FbxLoadingResult& outResult, bool loadMesh=true, bool loadSkeleton =false);

	private:

		struct N_FbxMeshSubset
		{
			N_FbxMeshSubset():triStart(0),triCount(0),matID(0){}
			int triStart;
			int triCount;
			int matID;
		};

		//tree-traversal
		void		mFunction_TraverseSceneNodes(FbxNode* pNode);

		void		mFunction_ProcessSceneNode_Mesh(FbxNode* pNode);
		
		//invoked by Process Mesh Scene Node member function, output {0,0,0,0} if no vertex color is defined
		void		mFunction_LoadMesh_VertexColor(FbxMesh* pMesh, int ctrlPointIndex, int polygonVertexIndex, NVECTOR4& outColor);
		
		//invoked by Process Mesh Scene Node member function, output {0,0,0,0} if no vertex Normal is defined
		void		mFunction_LoadMesh_VertexNormal(FbxMesh* pMesh, int ctrlPointIndex, int polygonVertexIndex, NVECTOR3& outNormal);
		
		//invoked by Process Mesh Scene Node member function, output {0,0,0,0} if no vertex tangent is defined
		void		mFunction_LoadMesh_VertexTangent(FbxMesh* pMesh, int ctrlPointIndex, int polygonVertexIndex, NVECTOR3& outTangent);
		
		//invoked by Process Mesh Scene Node member function, output {0,0,0,0} if no vertex UV is defined
		void		mFunction_LoadMesh_VertexTexCoord(FbxMesh* pMesh, int ctrlPointIndex, int polygonVertexIndex, int uvIndex, int uvLayer, NVECTOR2& outTexcoord);

		//Actually i don't want to import binormal at first. 
		//But the handness of tangent space basis could not determine without binormal(what a big pit!!)
		void		mFunction_LoadMesh_VertexBinormal(FbxMesh* pMesh, int ctrlPointIndex, int polygonVertexIndex, NVECTOR3& outBinormal);

		//load the mapping of triangle id and material id
		void		mFunction_LoadMesh_MatIndexOfTriangles(FbxMesh* pMesh, int triangleCount, std::vector<N_FbxMeshSubset>& outFbxSubsetList);

		//details information of materials bound to current mesh
		void		mFunction_LoadMesh_Materials(FbxNode* pNode, std::vector<N_FbxMaterialInfo>& outMatList);

		//textures which are bound to certain material
		void		mFunction_LoadMesh_Material_Textures(FbxSurfaceMaterial* pSM, N_FbxTextureMapsInfo& outTexInfo);

		void		mFunction_LoadMesh_Material_TextureMapInfo(const FbxProperty& prop, std::string& outTextureName, std::string& outTextureFilePath);

		//void		mFunction_ProcessSceneNode_Light(FbxNode* pNode);

		void		mFunction_ProcessSceneNode_Skeleton(FbxNode* pNode);

		//void		mFunction_ProcessSceneNode_Camera();

		FbxManager*		m_pFbxMgr;

		FbxScene*			m_pFbxScene;

		FbxIOSettings*	m_pIOSettings;

		N_FbxLoadingResult*		m_pRefOutResult;

		bool						mIsInitialized;

		bool						mEnableLoadMesh;

		bool						mEnableLoadSkeleton;

	};

};