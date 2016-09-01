
/***********************************************************************

							class：Scene Loader

				Desc: Load/parse resources from file

***********************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

IModelLoader::IModelLoader()
{

};

IModelLoader::~IModelLoader()
{

};

BOOL IModelLoader::LoadPlane(IMesh * pTargetMesh, float fWidth, float fDepth, UINT iRowCount, UINT iColumnCount)
{
	if (pTargetMesh == nullptr)return;

	//check if the input "Step Count" is illegal
	if (iColumnCount <= 2) { iColumnCount = 2; }
	if (iRowCount <= 2) { iRowCount = 2; }

	//delegate vert/idx creation duty to MeshGenerator 
	std::vector<N_DefaultVertex> tmpVB;
	std::vector<UINT> tmpIB;
	mMeshGenerator.CreatePlane(fWidth, fDepth, iRowCount, iColumnCount, tmpVB, tmpIB);

	//copy won't be overhead because std::move is used inside the function
	BOOL isUpdateOk = pTargetMesh->mFunction_UpdateDataToVideoMem(tmpVB, tmpIB);

	//user-set material
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

BOOL IModelLoader::LoadBox(IMesh * pTargetMesh, float fWidth, float fHeight, float fDepth, UINT iDepthStep, UINT iWidthStep, UINT iHeightStep)
{
	if (iDepthStep <= 2) { iDepthStep = 2; }
	if (iWidthStep <= 2) { iWidthStep = 2; }
	if (fWidth < 0)fWidth = 0;
	if (fHeight < 0)fHeight = 0;
	if (fDepth < 0)fDepth = 0;

	//mesh creation delegate to MeshGenerator
	std::vector<N_DefaultVertex> tmpVB;
	std::vector<UINT> tmpIB;
	mMeshGenerator.CreateBox(fWidth, fHeight, fDepth, iDepthStep, iWidthStep, iHeightStep, tmpVB, tmpIB);

	//copy won't be overhead because std::move is used inside the function
	BOOL isUpdateOk = pTargetMesh->mFunction_UpdateDataToVideoMem(tmpVB, tmpIB);

	//user-set material
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

BOOL IModelLoader::LoadSphere(IMesh * pTargetMesh, float fRadius, UINT iColumnCount, UINT iRingCount)
{
	//check if the input "Step Count" is illegal
	if (iColumnCount <= 3) { iColumnCount = 3; }
	if (iRingCount <= 1) { iRingCount = 1; }

	//mesh creation delegate to MeshGenerator
	std::vector<N_DefaultVertex> tmpVB;
	std::vector<UINT> tmpIB;
	mMeshGenerator.CreateSphere(fRadius, iColumnCount, iRingCount, tmpVB, tmpIB);

	//copy won't be overhead because std::move is used inside the function
	BOOL isUpdateOk = pTargetMesh->mFunction_UpdateDataToVideoMem(tmpVB, tmpIB);

	//user-set material
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

BOOL IModelLoader::LoadCylinder(IMesh * pTargetMesh, float fRadius, float fHeight, UINT iColumnCount, UINT iRingCount)
{
	//check if the input "Step Count" is illegal
	if (iColumnCount <= 3) { iColumnCount = 3; }
	if (iRingCount <= 2) { iRingCount = 2; }

	//mesh creation delegate to MeshGenerator
	std::vector<N_DefaultVertex> tmpVB;
	std::vector<UINT> tmpIB;
	mMeshGenerator.CreateCylinder(fRadius, fHeight, iColumnCount, iRingCount, tmpVB, tmpIB);

	//copy won't be overhead because std::move is used inside the function
	BOOL isUpdateOk = pTargetMesh->mFunction_UpdateDataToVideoMem(tmpVB, tmpIB);

	//user-set material
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

BOOL IModelLoader::LoadFile_STL(IMesh * pTargetMesh, NFilePath pFilePath)
{
	std::vector<UINT>			tmpIndexList;
	std::vector<NVECTOR3> tmpVertexList;
	std::vector<NVECTOR3> tmpNormalList;
	std::string							tmpInfo;

	//Load STL using file manager
	BOOL fileLoadSucceeded = FALSE;
	fileLoadSucceeded = IFileManager::ImportFile_STL(pFilePath, tmpVertexList, tmpIndexList, tmpNormalList, tmpInfo);
	if (!fileLoadSucceeded)
	{
		ERROR_MSG("Noise Mesh : Load STL failed!");
		return FALSE;
	}

	//compute the center pos of bounding box
	N_Box bbox= pTargetMesh->ComputeBoundingBox();
	NVECTOR3			tmpBoundingBoxCenter(0, 0, 0);
	tmpBoundingBoxCenter = NVECTOR3(
		(bbox.max.x + bbox.min.x) / 2.0f,
		(bbox.max.y + bbox.min.y) / 2.0f,
		(bbox.max.z + bbox.min.z) / 2.0f);

	//lambda function : compute texcoord for spherical mapping
	auto ComputeTexCoord_SphericalWrap= [](NVECTOR3 vBoxCenter, NVECTOR3 vPoint)->NVECTOR2
	{
		//额...这个函数做简单的纹理球形包裹

		NVECTOR2 outTexCoord(0, 0);
		NVECTOR3 tmpP = vPoint - vBoxCenter;

		//投影到单位球上
		D3DXVec3Normalize(&tmpP, &tmpP);

		//反三角函数算球坐标系坐标，然后角度值映射到[0,1]
		float angleYaw = 0.0f;
		float anglePitch = 0.0f;
		float tmpLength = sqrtf(tmpP.x*tmpP.x + tmpP.z*tmpP.z);

		// [ -PI/2 , PI/2 ]
		anglePitch = atan2(tmpP.y, tmpLength);

		// [ -PI	, PI ]
		angleYaw = atan2(tmpP.z, tmpP.x);

		//map to [0,1]
		outTexCoord.x = (angleYaw + MATH_PI) / (2.0f * MATH_PI);
		outTexCoord.y = (anglePitch + (MATH_PI / 2.0f)) / MATH_PI;

		return outTexCoord;
	};

	UINT k = 0;
	std::vector<N_DefaultVertex>  completeVertexList;
	for (UINT i = 0;i < tmpVertexList.size();i++)
	{
		N_DefaultVertex	tmpCompleteV;
		tmpCompleteV.Color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		tmpCompleteV.Pos = tmpVertexList.at(i);
		tmpCompleteV.Normal = tmpNormalList.at(k);
		tmpCompleteV.Tangent = NVECTOR3(-tmpCompleteV.Normal.z, 0, tmpCompleteV.Normal.x);//mighty tangent algorithm= =
		tmpCompleteV.TexCoord = ComputeTexCoord_SphericalWrap(tmpBoundingBoxCenter, tmpCompleteV.Pos);
		completeVertexList.push_back(tmpCompleteV);

		//每新增了一个三角形3个顶点 就要轮到下个三角形的法线了
		if (i % 3 == 2) { k++; }
	}


	//copy won't be overhead because std::move is used inside the function
	BOOL isUpdateOk = pTargetMesh->mFunction_UpdateDataToVideoMem(completeVertexList, tmpIndexList);

	//user-set material
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}
