
/***********************************************************************

							   h£ºFBX loader
		Desc: encapsulation of  FBXSDK, loading FBX model/scene

************************************************************************/

#include "fbxsdk.h"

#pragma once

namespace Noise3D
{
	/*struct N_DirLightDesc;
	struct N_PointLightDesc;
	struct N_SpotLightDesc;*/
	struct N_MeshSubsetInfo;

	struct N_FbxMeshInfo
	{
		N_FbxMeshInfo(){}

		N_UID	name;
		std::vector<N_DefaultVertex> vertexBuffer;
		std::vector<UINT> indexBuffer;
		std::vector<N_MeshSubsetInfo> subsetList;
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
		void		mFunction_LoadMesh_VertexTexCoord(FbxMesh* pMesh, int ctrlPointIndex, int uvIndex, int uvLayer, NVECTOR2& outTexcoord);

		//load the mapping of triangle id and material id
		void		mFunction_LoadMesh_MatIndexOfTriangles(FbxMesh* pMesh, int triangleCount, std::vector<N_FbxMeshSubset>& outFbxSubsetList);

		//details information of materials bound to current mesh
		void		mFunction_LoadMesh_Materials(FbxNode* pNode,std::vector<std::string>& outMatNameList,std::vector<N_MaterialDesc>& outMatList);

		//textures which are bound to certain material
		void		mFunction_LoadMesh_Textures();

		//void		mFunction_ProcessSceneNode_Light(FbxNode* pNode);

		void		mFunction_ProcessSceneNode_Skeleton(FbxNode* pNode);

		//void		mFunction_ProcessSceneNode_Camera();

		FbxManager*		m_pFbxMgr;

		FbxScene*			m_pFbxScene;

		FbxIOSettings*	m_pIOSettings;

		N_FbxLoadingResult*		m_pResult;

		bool						mIsInitialized;

		bool						mEnableLoadMesh;

		bool						mEnableLoadSkeleton;

	};


}