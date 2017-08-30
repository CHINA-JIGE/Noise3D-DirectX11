
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

bool IModelLoader::LoadPlane(IMesh * pTargetMesh, float fWidth, float fDepth, UINT iRowCount, UINT iColumnCount)
{
	if (pTargetMesh == nullptr)return false;

	//check if the input "Step Count" is illegal
	if (iColumnCount <= 2) { iColumnCount = 2; }
	if (iRowCount <= 2) { iRowCount = 2; }

	//delegate vert/idx creation duty to MeshGenerator 
	std::vector<N_DefaultVertex> tmpVB;
	std::vector<UINT> tmpIB;
	mMeshGenerator.CreatePlane(fWidth, fDepth, iRowCount, iColumnCount, tmpVB, tmpIB);

	//copy won't be overhead because std::move is used inside the function
	bool isUpdateOk = pTargetMesh->mFunction_UpdateDataToVideoMem(tmpVB, tmpIB);

	//user-set material
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

bool IModelLoader::LoadBox(IMesh * pTargetMesh, float fWidth, float fHeight, float fDepth, UINT iDepthStep, UINT iWidthStep, UINT iHeightStep)
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
	bool isUpdateOk = pTargetMesh->mFunction_UpdateDataToVideoMem(tmpVB, tmpIB);

	//user-set material
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

bool IModelLoader::LoadSphere(IMesh * pTargetMesh, float fRadius, UINT iColumnCount, UINT iRingCount)
{
	//check if the input "Step Count" is illegal
	if (iColumnCount <= 3) { iColumnCount = 3; }
	if (iRingCount <= 1) { iRingCount = 1; }

	//mesh creation delegate to MeshGenerator
	std::vector<N_DefaultVertex> tmpVB;
	std::vector<UINT> tmpIB;
	mMeshGenerator.CreateSphere(fRadius, iColumnCount, iRingCount, tmpVB, tmpIB);

	//copy won't be overhead because std::move is used inside the function
	bool isUpdateOk = pTargetMesh->mFunction_UpdateDataToVideoMem(tmpVB, tmpIB);

	//user-set material
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

bool IModelLoader::LoadCylinder(IMesh * pTargetMesh, float fRadius, float fHeight, UINT iColumnCount, UINT iRingCount)
{
	//check if the input "Step Count" is illegal
	if (iColumnCount <= 3) { iColumnCount = 3; }
	if (iRingCount <= 2) { iRingCount = 2; }

	//mesh creation delegate to MeshGenerator
	std::vector<N_DefaultVertex> tmpVB;
	std::vector<UINT> tmpIB;
	mMeshGenerator.CreateCylinder(fRadius, fHeight, iColumnCount, iRingCount, tmpVB, tmpIB);

	//copy won't be overhead because std::move is used inside the function
	bool isUpdateOk = pTargetMesh->mFunction_UpdateDataToVideoMem(tmpVB, tmpIB);

	//user-set material
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

bool IModelLoader::LoadCustomizedModel(IMesh * pTargetMesh, const std::vector<N_DefaultVertex>& vertexList, const std::vector<UINT>& indicesList)
{
	//"Out of Boundary" check for indices
	for (auto& index : indicesList)
	{
		if (index >= vertexList.size())
		{
			ERROR_MSG("IMesh: Load Customized Model failed!  illegal index found! ");
			return false;
		}
	}

	//copy won't be overhead because std::move is used inside the function
	bool isUpdateOk = pTargetMesh->mFunction_UpdateDataToVideoMem( vertexList, indicesList);

	//user-set material
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

bool IModelLoader::LoadFile_STL(IMesh * pTargetMesh, NFilePath pFilePath)
{
	std::vector<UINT>			tmpIndexList;
	std::vector<NVECTOR3> tmpVertexList;
	std::vector<NVECTOR3> tmpNormalList;
	std::string							tmpInfo;

	//Load STL using file manager
	bool fileLoadSucceeded = false;
	fileLoadSucceeded = IFileIO::ImportFile_STL(pFilePath, tmpVertexList, tmpIndexList, tmpNormalList, tmpInfo);
	if (!fileLoadSucceeded)
	{
		ERROR_MSG("IMesh : Load STL failed ! Cannot open file!");
		return false;
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
	//fill vertex attribute
	for (UINT i = 0;i < tmpVertexList.size();i++)
	{
		N_DefaultVertex	tmpCompleteV;
		tmpCompleteV.Color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		tmpCompleteV.Pos = tmpVertexList.at(i);
		tmpCompleteV.Normal = tmpNormalList.at(k);
		//tangent
		if (tmpCompleteV.Normal.x==0.0f && tmpCompleteV.Normal.z==0.0f)
		{
			tmpCompleteV.Tangent = NVECTOR3(1.0f, 0, 0);
		}
		else
		{
			NVECTOR3 tmpVec(-tmpCompleteV.Normal.z, 0, tmpCompleteV.Normal.x);
			D3DXVec3Cross(&tmpCompleteV.Tangent, &tmpCompleteV.Normal, &tmpVec);
			D3DXVec3Normalize(&tmpCompleteV.Tangent, &tmpCompleteV.Tangent);
		}
		tmpCompleteV.TexCoord = ComputeTexCoord_SphericalWrap(tmpBoundingBoxCenter, tmpCompleteV.Pos);
		completeVertexList.push_back(tmpCompleteV);

		//每新增了一个三角形3个顶点 就要轮到下个三角形的法线了
		if (i % 3 == 2) { k++; }
	}


	bool isUpdateOk = pTargetMesh->mFunction_UpdateDataToVideoMem(completeVertexList, tmpIndexList);

	//user-set material
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

bool IModelLoader::LoadFile_OBJ(IMesh * pTargetMesh, NFilePath pFilePath)
{
	std::vector<N_DefaultVertex> tmpCompleteVertexList;
	std::vector<UINT>	tmpIndexList;

	//加载STL
	bool fileLoadSucceeded = false;
	fileLoadSucceeded = IFileIO::ImportFile_OBJ(pFilePath, tmpCompleteVertexList, tmpIndexList);
	if (!fileLoadSucceeded)
	{
		ERROR_MSG("Noise Mesh : Load OBJ failed! Cannot open file. ");
		return false;
	}

	//copy won't be overhead because std::move is used inside the function
	bool isUpdateOk = pTargetMesh->mFunction_UpdateDataToVideoMem(tmpCompleteVertexList, tmpIndexList);

	//user-set material
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

bool IModelLoader::LoadFile_3DS(NFilePath pFilePath, std::vector<IMesh*>& outMeshPtrList,std::vector<N_UID>& outMeshNameList)
{
	std::vector<N_Load3ds_MeshObject>	meshList;
	std::vector<N_MaterialDesc>					materialList;
	std::vector<std::string>							matNameList;
	std::unordered_map<std::string, std::string> texName2FilePathMap;

	//import data
	bool importSucceeded = false;
	importSucceeded = IFileIO::ImportFile_3DS(
		pFilePath,
		meshList,
		materialList,
		matNameList,
		texName2FilePathMap);

	if (!importSucceeded)
	{
		ERROR_MSG("Load 3ds : Import File Failed!!");
		return false;
	}

#pragma region MeshCreation

	IMeshManager* pMeshMgr = GetScene()->GetMeshMgr();

	for (auto currentMesh:meshList)
	{
		//mesh names in .3ds file are customized in modeling software,
		//but I afraid that the name in 3ds will conflict with current Mesh Names
		N_UID currentMeshFinalName= currentMesh.meshName;
		UINT suffixIndex = 0;
		while (pMeshMgr->IsMeshExisted(currentMeshFinalName))
		{
			//add suffix _1 _2 _3 _4 etc. (if existed name found）
			currentMeshFinalName= currentMesh.meshName+"_"+std::to_string(suffixIndex);
			suffixIndex++;
		}
		suffixIndex = 0;

		//then use unique name to create IMesh Object
		IMesh* pCreatedMesh = pMeshMgr->CreateMesh(currentMeshFinalName);

		//to compute vertex normal ,we should generate adjacent information of vertices first.
		//thus "vertexNormalList" holds the sum of face normal (the triangle is adjacent to corresponding vertex)
		std::vector<NVECTOR3>	vertexNormalList(currentMesh.verticesList.size(), NVECTOR3(0, 0, 0));
		std::vector<NVECTOR3>	vertexTangentList;

		//1. compute vertex normal of faces
		for (UINT i = 0;i < currentMesh.indicesList.size();i += 3)
		{
			//compute face normal
			uint16_t idx1 = currentMesh.indicesList.at(i);
			uint16_t idx2 = currentMesh.indicesList.at(i + 1);
			uint16_t idx3 = currentMesh.indicesList.at(i + 2);
			NVECTOR3 v1 = currentMesh.verticesList.at(idx1);
			NVECTOR3 v2 = currentMesh.verticesList.at(idx2);
			NVECTOR3 v3 = currentMesh.verticesList.at(idx3);
			NVECTOR3 vec1 = v2 - v1;
			NVECTOR3 vec2 = v3 - v1;
			NVECTOR3 faceNormal(0, 0.0f, 0);
			//D3DXVec3Normalize(&vec1, &vec1);
			//D3DXVec3Normalize(&vec2, &vec2);
			D3DXVec3Cross(&faceNormal, &vec1, &vec2);

			//add face normal to vertex normal
			vertexNormalList.at(idx1) += faceNormal;
			vertexNormalList.at(idx2) += faceNormal;
			vertexNormalList.at(idx3) += faceNormal;
		}

		//2.normalize all vertex normal
		//3.by the way, compute vertex tangent
		vertexTangentList.reserve(currentMesh.verticesList.size());
		for (auto& vn : vertexNormalList)
		{
			D3DXVec3Normalize(&vn, &vn);

			//see if tangent is parallel with Y axis
			NVECTOR3 tmpTangent(1.0f, 0, 0);
			if (!(vn.x == 0 && vn.z == 0))
			{
				NVECTOR3 tmpVec(-vn.z, 0, vn.x);
				D3DXVec3Cross(&tmpTangent, &vn, &tmpVec);
				D3DXVec3Normalize(&tmpTangent, &tmpTangent);
			}

			//one vertex for one tangent
			vertexTangentList.push_back(tmpTangent);
		}

		//generate complete vertices
		std::vector<N_DefaultVertex> completeVertexList;
		completeVertexList.reserve(currentMesh.verticesList.size());
		for (UINT i = 0;i < currentMesh.verticesList.size();i++)
		{
			N_DefaultVertex tmpCompleteVertex;

			try
			{
				tmpCompleteVertex.Color = NVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
				tmpCompleteVertex.Normal = vertexNormalList.at(i);
				tmpCompleteVertex.Pos = currentMesh.verticesList.at(i);
				tmpCompleteVertex.Tangent = vertexTangentList.at(i);
				tmpCompleteVertex.TexCoord = currentMesh.texcoordList.at(i);

				completeVertexList.push_back(tmpCompleteVertex);
			}
			catch (std::out_of_range)
			{
				//theoretically , all buffer should have the same total elements count
				//but if shit happens,we must stop this.
				ERROR_MSG("Load File 3DS: WARNING : data could be damaged!!");
				break;
			}
		}

		//.....................
		//copy won't be overhead because std::move is used inside the function
		pCreatedMesh->mFunction_UpdateDataToVideoMem(completeVertexList, currentMesh.indicesList);

		//copy SUBSET lists
		*pCreatedMesh->m_pSubsetInfoList = std::move(currentMesh.subsetList);

		//push_back IMesh ptr info for user
		if (pCreatedMesh != nullptr)
		{
			outMeshPtrList.push_back(pCreatedMesh);
			outMeshNameList.push_back(currentMeshFinalName);
		}
		else
		{
			WARNING_MSG("Load 3ds : One of the mesh(es) fail to create, name:" + currentMesh.meshName);
		}

	}
#pragma endregion MeshCreation

#pragma region material&TextureData


	//!!!!!materials have not been created. The Creation of materials will be done
	//by current Scene Tex&Mat Manager	
	IMaterialManager* pMatMgr = GetScene()->GetMaterialMgr(); //m_pChildMaterialMgr;
	ITextureManager* pTexMgr = GetScene()->GetTextureMgr();//m_pFatherScene->m_pChildTextureMgr;

														   //Get the directory where the file locates
	std::string modelFileDirectory = GetFileDirectory(pFilePath);

	if (pTexMgr != nullptr && pMatMgr != nullptr)
	{
		for (UINT i = 0;i < materialList.size();i++)
		{
			//----------Create Material-----------
			IMaterial* pMat = pMatMgr->CreateMaterial(matNameList.at(i), materialList.at(i));
			if (pMat == nullptr)
				WARNING_MSG("WARNING : Load 3ds : Material Creation Failed!!");

			//----------Create Texture Maps----------
			std::string& diffMapName = materialList.at(i).diffuseMapName;
			std::string& normalMapName = materialList.at(i).normalMapName;
			std::string& specMapName = materialList.at(i).specularMapName;

			//Define a temp lambda function for texture creations
			auto lambdaFunc_CreateTexture = [&](std::string& texName)
			{
				if (pTexMgr->ValidateUID(texName) == false)
				{
					if (texName != "")
					{
						UINT textureReturnID = NOISE_MACRO_INVALID_TEXTURE_ID;

						//locate the directory to generate file path
						std::string mapPath = modelFileDirectory + texName2FilePathMap.at(texName);

						//Try to create texture
						ITexture* pTex= pTexMgr->CreateTextureFromFile(
							mapPath.c_str(),
							texName,
							true, 0, 0, false);

						if (pTex==nullptr)
							WARNING_MSG("WARNING : Load 3ds : texture Creation Failed!!");
					}
				}
			};

			//DIFFUSE MAP
			lambdaFunc_CreateTexture(diffMapName);

			//NORMAL MAP
			lambdaFunc_CreateTexture(normalMapName);

			//SPECULAR MAP
			lambdaFunc_CreateTexture(specMapName);
		}
	}

#pragma endregion


	return true;
}
