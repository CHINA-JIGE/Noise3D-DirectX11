
/***********************************************************************

							   h£ºFBX loader
		Desc: encapsulation of  FBXSDK, loading FBX model/scene
		which will be integrated to MeshLoader

************************************************************************/

#pragma once

#include "_FbxLoaderDataStructure.h"

namespace Noise3D
{
	struct N_MeshSubsetInfo;
	struct N_BasicLambertMaterialDesc;

	class /*_declspec(dllexport)*/ IFbxLoader
	{
	public:

		IFbxLoader();

		~IFbxLoader();

		bool		Initialize();

		//there are many types of scene nodes in fbx, but mesh/animation are the only two interested now(2017.9.20)
		bool		LoadSceneFromFbx(NFilePath fbxPath, N_FbxLoadingResult& outResult, bool loadMesh=true, bool loadSkeleton =false);

		bool		LoadPbrtMeshesFromFbx(NFilePath fbxPath, N_FbxPbrtSceneLoadingResult& outResult);

	private:

		struct N_FbxMeshSubset
		{
			N_FbxMeshSubset():triStart(0),triCount(0),matID(0){}
			int triStart;
			int triCount;
			int matID;
		};

		//require different process procedure
		enum MESH_MATERIAL_TYPE
		{
			LAMBERT_OR_PHONG,
			ADVANCED
		};

		//initialization before loading
		bool		_Initialize(NFilePath fbxPath, bool loadMesh, bool loadSkeleton);

		//tree-traversal
		void		_TraverseSceneNodes(FbxNode* pNode);

		void		_PBRT_TraverseSceneNodes(FbxNode* pNode);

		void		_ProcessSceneNode_Mesh(FbxNode* pNode, MESH_MATERIAL_TYPE matType);
		
		//invoked by Process Mesh Scene Node member function, output {0,0,0,0} if no vertex color is defined
		void		_LoadMesh_VertexColor(FbxMesh* pMesh, int ctrlPointIndex, int polygonVertexIndex, Vec4& outColor);
		
		//invoked by Process Mesh Scene Node member function, output {0,0,0,0} if no vertex Normal is defined
		void		_LoadMesh_VertexNormal(FbxMesh* pMesh, int ctrlPointIndex, int polygonVertexIndex, Vec3& outNormal);
		
		//invoked by Process Mesh Scene Node member function, output {0,0,0,0} if no vertex tangent is defined
		void		_LoadMesh_VertexTangent(FbxMesh* pMesh, int ctrlPointIndex, int polygonVertexIndex, Vec3& outTangent);
		
		//invoked by Process Mesh Scene Node member function, output {0,0,0,0} if no vertex UV is defined
		void		_LoadMesh_VertexTexCoord(FbxMesh* pMesh, int ctrlPointIndex, int polygonVertexIndex, int uvIndex, int uvLayer, Vec2& outTexcoord);

		//Actually i don't want to import binormal at first. 
		//But the handness of tangent space basis could not determine without binormal(what a big pit!!)
		void		_LoadMesh_VertexBinormal(FbxMesh* pMesh, int ctrlPointIndex, int polygonVertexIndex, Vec3& outBinormal);

		//load the mapping of triangle id and material id
		void		_LoadMesh_MatIndexOfTriangles(FbxMesh* pMesh, int triangleCount, std::vector<N_FbxMeshSubset>& outFbxSubsetList);

		//details information of materials bound to current mesh
		void		_LoadMesh_Materials(FbxNode* pNode, std::vector<N_FbxMaterialInfo>& outMatList);

		void		_PBRT_LoadMesh_Materials(FbxNode* pNode, std::vector<N_FbxPbrtMaterialInfo>& outMatList);

		//textures which are bound to certain material
		void		_LoadMesh_Material_Textures(FbxSurfaceMaterial* pSM, N_FbxTextureMapsInfo& outTexInfo);

		void		_PBRT_LoadMesh_Material_Textures(FbxSurfaceMaterial* pSM, N_FbxPbrtTextureMapsInfo& outTexInfo);

		void		mFunction_LoadMesh_Material_TextureMapInfo(const FbxProperty& prop, std::string& outTextureName, std::string& outTextureFilePath);

		//void		mFunction_ProcessSceneNode_Light(FbxNode* pNode);

		void		_ProcessSceneNode_Skeleton(FbxNode* pNode);

		//void		mFunction_ProcessSceneNode_Camera();

		FbxManager*		m_pFbxMgr;

		FbxScene*			m_pFbxScene;

		FbxIOSettings*	m_pIOSettings;

		N_FbxLoadingResult*		m_pRefCommonOutResult;

		N_FbxPbrtSceneLoadingResult*	m_pRefPbrtOutResult;

		bool						mIsInitialized;

		bool						mEnableLoadMesh;

		bool						mEnableLoadSkeleton;

	};

};