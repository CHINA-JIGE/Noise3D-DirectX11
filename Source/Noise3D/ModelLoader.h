
/***********************************************************************

								h£ºModel Loader (mainly from file)

************************************************************************/

#pragma once
#include "_FbxLoader.h"


namespace Noise3D
{
	class IMesh;
	class IAtmosphere;

	//name lists of loaded objects (e.g. from .fbx)
	struct N_SceneLoadingResult
	{
		N_SceneLoadingResult(){}

		std::vector<N_UID> meshNameList;
		std::vector<N_UID> materialNameList;
	};

	class /*_declspec(dllexport)*/ IModelLoader
	{
	public:

		bool		LoadPlane(IMesh* pTargetMesh, float fWidth, float fDepth, UINT iRowCount = 5, UINT iColumnCount = 5);

		bool		LoadBox(IMesh* pTargetMesh, float fWidth, float fHeight, float fDepth, UINT iDepthStep = 3, UINT iWidthStep = 3, UINT iHeightStep = 3);

		bool		LoadSphere(IMesh* pTargetMesh, float fRadius, UINT iColumnCount = 20, UINT iRingCount = 20);

		bool		LoadCylinder(IMesh* pTargetMesh,float fRadius, float fHeight, UINT iColumnCount = 40, UINT iRingCount = 8);

		bool		LoadCustomizedModel(IMesh* pTargetMesh, const std::vector<N_DefaultVertex>& vertexList, const std::vector<UINT>& indicesList);

		bool		LoadFile_STL(IMesh* pTargetMesh, NFilePath filePath);

		bool		LoadFile_OBJ(IMesh* pTargetMesh, NFilePath filePath);

		//bool		LoadFile_3DS(NFilePath pFilePath, std::vector<IMesh*>& outMeshPtrList, std::vector<N_UID>& outMeshNameList);

		//meshes are created automatically. call MeshManager.GetMesh() to retrieve pointers to mesh objects
		void		LoadFile_FBX(NFilePath pFilePath, N_SceneLoadingResult& outLoadingResult);

		bool		LoadSkyDome(IAtmosphere* pAtmo,N_UID texture, float fRadiusXZ, float fHeight);

		bool		LoadSkyBox(IAtmosphere* pAtmo, N_UID texture, float fWidth, float fHeight, float fDepth);

	private:

		friend class IFactory<IModelLoader>;

		IModelLoader();

		~IModelLoader();

		//internal mesh loading helper
		IFileIO mFileIO;
		IGeometryMeshGenerator mMeshGenerator;
		IFbxLoader mFbxLoader;

	};

};