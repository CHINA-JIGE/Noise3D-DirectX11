
/***********************************************************************

								h£ºModel Loader 

************************************************************************/

#pragma once
#include "_FbxLoader.h"


namespace Noise3D
{
	class Mesh;
	class Atmosphere;

	//name lists of loaded objects (e.g. from .fbx)
	struct N_SceneLoadingResult
	{
		N_SceneLoadingResult(){}

		std::vector<N_UID> meshNameList;
		std::vector<N_UID> materialNameList;
	};

	class /*_declspec(dllexport)*/ ModelLoader
	{
	public:

		//pointer won't be modified.(object pointer should be created by MeshManager)
		bool		LoadPlane(Mesh* const pTargetMesh, float fWidth, float fDepth, UINT iRowCount = 5, UINT iColumnCount = 5);

		bool		LoadBox(Mesh* const pTargetMesh, float fWidth, float fHeight, float fDepth, UINT iDepthStep = 3, UINT iWidthStep = 3, UINT iHeightStep = 3);

		bool		LoadSphere(Mesh* const pTargetMesh, float fRadius, UINT iColumnCount = 20, UINT iRingCount = 20);

		bool		LoadCylinder(Mesh* const pTargetMesh,float fRadius, float fHeight, UINT iColumnCount = 40, UINT iRingCount = 8);

		bool		LoadCustomizedModel(Mesh* const pTargetMesh, const std::vector<N_DefaultVertex>& vertexList, const std::vector<UINT>& indicesList);

		bool		LoadFile_STL(Mesh* const pTargetMesh, NFilePath filePath);

		bool		LoadFile_OBJ(Mesh* const pTargetMesh, NFilePath filePath);

		//bool		LoadFile_3DS(NFilePath pFilePath, std::vector<Mesh*>& outMeshPtrList, std::vector<N_UID>& outMeshNameList);

		//meshes are created automatically. call MeshManager.GetMesh() to retrieve pointers to mesh objects
		void		LoadFile_FBX(NFilePath pFilePath, N_SceneLoadingResult& outLoadingResult);

		bool		LoadSkyDome(Atmosphere* const pAtmo,N_UID texture, float fRadiusXZ, float fHeight);

		bool		LoadSkyBox(Atmosphere* const pAtmo, N_UID texture, float fWidth, float fHeight, float fDepth);

	private:

		friend class IFactory<ModelLoader>;

		ModelLoader();

		~ModelLoader();

		//internal mesh loading helper
		IFileIO mFileIO;
		IGeometryMeshGenerator mMeshGenerator;
		IFbxLoader mFbxLoader;

	};

};