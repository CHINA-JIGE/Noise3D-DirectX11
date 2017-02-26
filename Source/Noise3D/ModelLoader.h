
/***********************************************************************

								h£ºModel Loader (mainly from file)

************************************************************************/

#pragma once

namespace Noise3D
{
	//class IMesh;

	class /*_declspec(dllexport)*/ IModelLoader : private IFileManager
	{
	public:

		BOOL		LoadPlane(IMesh* pTargetMesh, float fWidth, float fDepth, UINT iRowCount = 5, UINT iColumnCount = 5);

		BOOL		LoadBox(IMesh* pTargetMesh, float fWidth, float fHeight, float fDepth, UINT iDepthStep = 3, UINT iWidthStep = 3, UINT iHeightStep = 3);

		BOOL		LoadSphere(IMesh* pTargetMesh, float fRadius, UINT iColumnCount = 20, UINT iRingCount = 20);

		BOOL		LoadCylinder(IMesh* pTargetMesh,float fRadius, float fHeight, UINT iColumnCount = 40, UINT iRingCount = 8);

		BOOL		LoadFile_STL(IMesh* pTargetMesh, NFilePath pFilePath);

		BOOL		LoadFile_OBJ(IMesh* pTargetMesh, NFilePath pFilePath);

		BOOL		LoadFile_3DS(NFilePath pFilePath, std::vector<IMesh*>& outMeshPtrList, std::vector<N_UID>& outMeshNameList);

	private:

		friend class IFactory<IModelLoader>;

		IModelLoader();

		~IModelLoader();

		//internal mesh generator
		IGeometryMeshGenerator mMeshGenerator;

	};

};