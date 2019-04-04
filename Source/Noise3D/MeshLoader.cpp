
/***********************************************************************

							class£ºScene Loader

				Desc: Load/parse resources from file

***********************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

MeshLoader::MeshLoader()
{

};

MeshLoader::~MeshLoader()
{

};

bool MeshLoader::LoadPlane(Mesh * const pTargetMesh, float fWidth, float fDepth, UINT iRowCount, UINT iColumnCount)
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
	bool isUpdateOk = pTargetMesh->mFunction_CreateGpuBufferAndUpdateData(tmpVB, tmpIB);
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

bool MeshLoader::LoadBox(Mesh * const pTargetMesh, float fWidth, float fHeight, float fDepth, UINT iDepthStep, UINT iWidthStep, UINT iHeightStep)
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
	bool isUpdateOk = pTargetMesh->mFunction_CreateGpuBufferAndUpdateData(tmpVB, tmpIB);
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

bool MeshLoader::LoadSphere(Mesh * const pTargetMesh, float fRadius, UINT iColumnCount, UINT iRingCount)
{
	//check if the input "Step Count" is illegal
	if (iColumnCount <= 3) { iColumnCount = 3; }
	if (iRingCount <= 1) { iRingCount = 1; }

	//mesh creation delegate to MeshGenerator
	std::vector<N_DefaultVertex> tmpVB;
	std::vector<UINT> tmpIB;
	mMeshGenerator.CreateSphere(fRadius, iColumnCount, iRingCount, tmpVB, tmpIB);

	//copy won't be overhead because std::move is used inside the function
	bool isUpdateOk = pTargetMesh->mFunction_CreateGpuBufferAndUpdateData(tmpVB, tmpIB);
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

bool MeshLoader::LoadCylinder(Mesh * const pTargetMesh, float fRadius, float fHeight, UINT iColumnCount, UINT iRingCount)
{
	//check if the input "Step Count" is illegal
	if (iColumnCount <= 3) { iColumnCount = 3; }
	if (iRingCount <= 2) { iRingCount = 2; }

	//mesh creation delegate to MeshGenerator
	std::vector<N_DefaultVertex> tmpVB;
	std::vector<UINT> tmpIB;
	mMeshGenerator.CreateCylinder(fRadius, fHeight, iColumnCount, iRingCount, tmpVB, tmpIB);

	//copy won't be overhead because std::move is used inside the function
	bool isUpdateOk = pTargetMesh->mFunction_CreateGpuBufferAndUpdateData(tmpVB, tmpIB);
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

bool MeshLoader::LoadCustomizedModel(Mesh * const pTargetMesh, const std::vector<N_DefaultVertex>& vertexList, const std::vector<UINT>& indicesList)
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
	bool isUpdateOk = pTargetMesh->mFunction_CreateGpuBufferAndUpdateData( vertexList, indicesList);
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

bool MeshLoader::LoadFile_STL(Mesh * const pTargetMesh, NFilePath pFilePath)
{
	std::vector<UINT>			tmpIndexList;
	std::vector<Vec3> tmpVertexList;
	std::vector<Vec3> tmpNormalList;
	std::string							tmpInfo;

	//Load STL using file manager
	bool fileLoadSucceeded = false;
	fileLoadSucceeded = mFileIO.ImportFile_STL(pFilePath, tmpVertexList, tmpIndexList, tmpNormalList, tmpInfo);
	if (!fileLoadSucceeded)
	{
		ERROR_MSG("IMesh : Load STL failed ! Cannot open file!");
		return false;
	}

	//compute the center pos of bounding box
	N_AABB bbox= pTargetMesh->GetLocalAABB();
	Vec3			tmpBoundingBoxCenter(0, 0, 0);
	tmpBoundingBoxCenter = Vec3(
		(bbox.max.x + bbox.min.x) / 2.0f,
		(bbox.max.y + bbox.min.y) / 2.0f,
		(bbox.max.z + bbox.min.z) / 2.0f);

	//lambda function : compute texcoord for spherical mapping
	auto ComputeTexCoord_SphericalWrap= [](Vec3 vBoxCenter, Vec3 vPoint)->Vec2
	{
		//a simple texture coord wrapping (spherical)

		Vec2 outTexCoord(0, 0);
		Vec3 tmpP = vPoint - vBoxCenter;

		//project to unit sphere
		tmpP.Normalize();

		//inverse trigonometric function
		float angleYaw = 0.0f;
		float anglePitch = 0.0f;
		float tmpLength = sqrtf(tmpP.x*tmpP.x + tmpP.z*tmpP.z);

		// [ -PI/2 , PI/2 ]
		anglePitch = atan2(tmpP.y, tmpLength);

		// [ -PI	, PI ]
		angleYaw = atan2(tmpP.z, tmpP.x);

		//map to [0,1]
		outTexCoord.x = (angleYaw + Ut::PI) / (2.0f * Ut::PI);
		outTexCoord.y = (anglePitch + (Ut::PI / 2.0f)) / Ut::PI;

		return outTexCoord;
	};

	UINT normal_index = 0;
	std::vector<N_DefaultVertex>  completeVertexList;
	//fill vertex attribute
	for (UINT i = 0;i < tmpVertexList.size();i++)
	{
		N_DefaultVertex	tmpCompleteV;
		tmpCompleteV.Color = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		tmpCompleteV.Pos = tmpVertexList.at(i);
		tmpCompleteV.Normal = tmpNormalList.at(normal_index);
		//tangent
		if (tmpCompleteV.Normal.x==0.0f && tmpCompleteV.Normal.z==0.0f)
		{
			tmpCompleteV.Tangent = Vec3(1.0f, 0, 0);
		}
		else
		{
			Vec3 tmpVec(-tmpCompleteV.Normal.z, 0, tmpCompleteV.Normal.x);
			tmpCompleteV.Tangent = tmpCompleteV.Normal.Cross(tmpVec);
			//D3DXVec3Cross(&tmpCompleteV.Tangent, &tmpCompleteV.Normal, &tmpVec);
			tmpCompleteV.Tangent.Normalize();
			//D3DXVec3Normalize(&tmpCompleteV.Tangent, &tmpCompleteV.Tangent);
		}
		tmpCompleteV.TexCoord = ComputeTexCoord_SphericalWrap(tmpBoundingBoxCenter, tmpCompleteV.Pos);
		completeVertexList.push_back(tmpCompleteV);

		//one normal vector for 3 vertices (1 triangle)
		if (i % 3 == 2) { normal_index++; }
	}


	bool isUpdateOk = pTargetMesh->mFunction_CreateGpuBufferAndUpdateData(completeVertexList, tmpIndexList);
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

bool MeshLoader::LoadFile_OBJ(Mesh * const pTargetMesh, NFilePath filePath)
{
	std::vector<N_DefaultVertex> tmpCompleteVertexList;
	std::vector<UINT>	tmpIndexList;

	//¼ÓÔØSTL
	bool fileLoadSucceeded = false;
	fileLoadSucceeded = mFileIO.ImportFile_OBJ(filePath, tmpCompleteVertexList, tmpIndexList);
	if (!fileLoadSucceeded)
	{
		ERROR_MSG("IMesh : Load OBJ failed! Cannot open file. ");
		return false;
	}

	//copy won't be overhead because std::move is used inside the function
	bool isUpdateOk = pTargetMesh->mFunction_CreateGpuBufferAndUpdateData(tmpCompleteVertexList, tmpIndexList);
	pTargetMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);

	return isUpdateOk;
}

void MeshLoader::LoadFile_FBX(NFilePath filePath, N_SceneLoadingResult & outLoadingResult)
{
	//if fbx loader has already been initialized,
	//then actual init procedure will be skipped
	N_FbxLoadingResult fbxResult;
	mFbxLoader.Initialize();

	//some scene nodes are not supported(2017.9.20)
	mFbxLoader.LoadSceneFromFbx(filePath, fbxResult, true, false);

	//create mesh object in Noise3D with the data loaded from fbx
	SceneManager* pScene = Noise3D::GetScene();
	MeshManager*		pMeshMgr	= pScene->GetMeshMgr();
	MaterialManager*	pMatMgr		= pScene->GetMaterialMgr();
	TextureManager*	pTexMgr		= pScene->GetTextureMgr();

	//root node to fbx scene
	SceneNode* pFbxRootNode = pScene->GetSceneGraph().GetRoot()->CreateChildNode();
	outLoadingResult.pFbxSceneRootNode = pFbxRootNode;

	for (auto& m : fbxResult.meshDataList)
	{
		//***1.Create Mesh and its scene node 
		SceneNode* pNode = pFbxRootNode->CreateChildNode();
		Mesh* pMesh = pMeshMgr->CreateMesh(pNode, m.name);
		if (pMesh == nullptr)
		{
			WARNING_MSG("Model Loader: Load FBX scene: failed to create Noise3D::IMesh Object"
				+ m.name);
			continue;
		}

		//update data to graphic memory
		bool isUpdateSuccessful = pMesh->mFunction_CreateGpuBufferAndUpdateData(m.vertexBuffer, m.indexBuffer);
		if (!isUpdateSuccessful) 
		{
			WARNING_MSG("Model Loader: Load FBX scene: Mesh failed to load: mesh name:" 
				+ m.name);
			pMeshMgr->DestroyObject(m.name); 
			continue;
		}
	
		//set coordinate transformation
		pNode->GetLocalTransform().SetScale(m.scale.x, m.scale.y, m.scale.z);
		pNode->GetLocalTransform().SetPosition(m.pos.x, m.pos.y, m.pos.z);
		pNode->GetLocalTransform().SetRotation(m.rotation.x, m.rotation.y, m.rotation.z);

		//output new mesh name
		outLoadingResult.meshNameList.push_back(m.name);

		//***2,material & texture creation
		for (uint32_t matID = 0; matID < m.matList.size(); ++matID)
		{
			const N_FbxMaterialInfo& fbxMat = m.matList.at(matID);
			
			//Create texture first (failed one will be deprecated)
			//emmmm... seems that Cube Map is not supported by .fbx ??
			std::string diffMapName = fbxMat.texMapInfo.diffMapName;
			std::string normalMapName =fbxMat.texMapInfo.normalMapName;
			std::string specMapName = fbxMat.texMapInfo.specMapName;
			std::string emissiveMapName = fbxMat.texMapInfo.emissiveMapName;

			std::string modelFileFolder = Ut::GetFileFolderFromPath(filePath);
			std::string diffMapPath = modelFileFolder + Ut::GetFileNameFromPath(fbxMat.texMapInfo.diffMapFilePath);
			std::string normalMapPath = modelFileFolder + Ut::GetFileNameFromPath(fbxMat.texMapInfo.normalMapFilePath);
			std::string specMapPath = modelFileFolder + Ut::GetFileNameFromPath(fbxMat.texMapInfo.specMapFilePath);
			std::string emissiveMapPath = modelFileFolder + Ut::GetFileNameFromPath(fbxMat.texMapInfo.emissiveMapFilePath);

			ITexture* pTexDiff = nullptr;
			ITexture* pTexNormal = nullptr;
			ITexture* pTexEmissive = nullptr;
			ITexture* pTexSpec = nullptr;


			if (!diffMapName.empty())
			{
				//skip repeated map
				if (pTexMgr->ValidateTex2D(diffMapName) == true)continue;

				pTexDiff = pTexMgr->CreateTextureFromFile(diffMapPath, diffMapName, true, 0, 0, false);

				if (pTexDiff == nullptr)
				{
					WARNING_MSG("Model Loader: Load FBX scene: Texture (diffuse) failed to load: Texture name:"
						+ diffMapName);
					diffMapName = "";
				}
			}

			if (!normalMapName.empty())
			{
				//skip repeated map
				if (pTexMgr->ValidateTex2D(normalMapName) == true)continue;

				pTexNormal = pTexMgr->CreateTextureFromFile(normalMapPath, normalMapName, true, 0, 0, false);

				if (pTexNormal == nullptr)
				{
					WARNING_MSG("Model Loader: Load FBX scene: Texture (normal map) failed to load: Texture name:"
						+ normalMapName);
					normalMapName = "";
				}
			}

			if (!emissiveMapName.empty())
			{
				//skip repeated map
				if (pTexMgr->ValidateTex2D(emissiveMapName) == true)continue;

				pTexEmissive = pTexMgr->CreateTextureFromFile(emissiveMapPath, emissiveMapName, true, 0, 0, false);

				if (pTexEmissive == nullptr)
				{
					WARNING_MSG("Model Loader: Load FBX scene: Texture (emissive map) failed to load: Texture name:"
						+ emissiveMapName);
					emissiveMapName = "";
				}
			}

			if (!specMapName.empty())
			{
				//skip repeated map
				if (pTexMgr->ValidateTex2D(specMapName) == true)continue;

				pTexSpec = pTexMgr->CreateTextureFromFile(specMapPath, specMapName, true, 0, 0, false);

				if (pTexSpec == nullptr)
				{
					WARNING_MSG("Model Loader: Load FBX scene: Texture (specular map) failed to load: Texture name:"
						+ specMapName);
					specMapName = "";
				}
			}
			
			//create material
			N_LambertMaterialDesc newMatDesc( fbxMat.matBasicInfo);
			newMatDesc.diffuseMapName = diffMapName;
			newMatDesc.normalMapName = normalMapName;
			newMatDesc.specularMapName = specMapName;
			newMatDesc.emissiveMapName = emissiveMapName;

			const std::string& matName = fbxMat.matName;
			if (!matName.empty())
			{
				if (pMatMgr->FindUid(matName) == true)continue;//material that is already created
				LambertMaterial* pMat = pMatMgr->CreateMaterial(matName, newMatDesc);
				if (pMat == nullptr)
				{
					WARNING_MSG("Model Loader: Load FBX scene: Material failed to load: material name:"
						+ matName);
					continue;
				}
				outLoadingResult.materialNameList.push_back(matName);
			}
		}


		//3. mesh subset (if no customized matieral, default subset list will be assigned
		if (m.subsetList.size() != 0)
		{
			pMesh->SetSubsetList(m.subsetList);
		}
		else
		{
			pMesh->SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
		}
	}
}

bool MeshLoader::LoadSkyDome(Atmosphere * const pAtmo,N_UID textureName, float fRadiusXZ, float fHeight)
{
	//check if the input "Step Count" is illegal
	UINT iColumnCount = 30;
	UINT iRingCount = 25;
	UINT tmpVertexCount = 0;
	UINT tmpIndexCount = 0;


	//delegate vert/idx creation duty to MeshGenerator 
	std::vector<N_SimpleVertex> tmpVB;
	std::vector<UINT> tmpIB;
	mMeshGenerator.CreateSkyDome(fRadiusXZ, fHeight, iColumnCount, iRingCount, tmpVB, tmpIB);
	
	//copy won't be overhead because std::move is used inside the function
	bool isUpdateOk = pAtmo->mFunction_CreateGpuBufferAndUpdateData(tmpVB, tmpIB);

	//set current sky type
	pAtmo->mSkyDomeRadiusXZ = fRadiusXZ;
	pAtmo->mSkyDomeHeight = fHeight;
	pAtmo->mSkyType = NOISE_ATMOSPHERE_SKYTYPE_DOME;
	pAtmo->mSkyDomeTexName = textureName;

	return isUpdateOk;
}

bool MeshLoader::LoadSkyBox(Atmosphere * const pAtmo, N_UID texture, float fWidth, float fHeight, float fDepth)
{
	//check if the input "Step Count" is illegal
	UINT iColumnCount = 30;
	UINT iRingCount = 25;
	UINT tmpVertexCount = 0;
	UINT tmpIndexCount = 0;


	//delegate vert/idx creation duty to MeshGenerator 
	std::vector<N_SimpleVertex> tmpVB;
	std::vector<UINT> tmpIB;
	mMeshGenerator.CreateSkyBox(fWidth, fHeight, fDepth, tmpVB, tmpIB);

	//copy won't be overhead because std::move is used inside the function
	bool isUpdateOk = pAtmo->mFunction_CreateGpuBufferAndUpdateData(tmpVB, tmpIB);

	//set current sky type
	pAtmo->mSkyBoxWidth = fWidth;
	pAtmo->mSkyBoxHeight = fHeight;
	pAtmo->mSkyBoxDepth = fDepth;
	pAtmo->mSkyType = NOISE_ATMOSPHERE_SKYTYPE_BOX;
	pAtmo->mSkyBoxCubeTexName = texture;

	return isUpdateOk;

	return false;
}



/*bool MeshLoader::LoadFile_3DS(NFilePath pFilePath, std::vector<Mesh*>& outMeshPtrList,std::vector<N_UID>& outMeshNameList)
{
	std::vector<N_Load3ds_MeshObject>	meshList;
	std::vector<N_LambertMaterialDesc>					materialList;
	std::vector<std::string>							matNameList;
	std::unordered_map<std::string, std::string> texName2FilePathMap;

	//import data
	bool importSucceeded = false;
	importSucceeded = mFileIO.ImportFile_3DS(
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

	MeshManager* pMeshMgr = GetScene()->GetMeshMgr();

	for (auto currentMesh:meshList)
	{
		//mesh names in .3ds file are customized in modeling software,
		//but I afraid that the name in 3ds will conflict with current Mesh Names
		N_UID currentMeshFinalName= currentMesh.meshName;
		UINT suffixIndex = 0;
		while (pMeshMgr->IsMeshExisted(currentMeshFinalName))
		{
			//add suffix _1 _2 _3 _4 etc. (if existed name found£©
			currentMeshFinalName= currentMesh.meshName+"_"+std::to_string(suffixIndex);
			suffixIndex++;
		}
		suffixIndex = 0;

		//then use unique name to create Mesh Object
		Mesh* pCreatedMesh = pMeshMgr->CreateMesh(currentMeshFinalName);

		//to compute vertex normal ,we should generate adjacent information of vertices first.
		//thus "vertexNormalList" holds the sum of face normal (the triangle is adjacent to corresponding vertex)
		std::vector<Vec3>	vertexNormalList(currentMesh.verticesList.size(), Vec3(0, 0, 0));
		std::vector<Vec3>	vertexTangentList;

		//1. compute vertex normal of faces
		for (UINT i = 0;i < currentMesh.indicesList.size();i += 3)
		{
			//compute face normal
			uint16_t idx1 = currentMesh.indicesList.at(i);
			uint16_t idx2 = currentMesh.indicesList.at(i + 1);
			uint16_t idx3 = currentMesh.indicesList.at(i + 2);
			Vec3 v1 = currentMesh.verticesList.at(idx1);
			Vec3 v2 = currentMesh.verticesList.at(idx2);
			Vec3 v3 = currentMesh.verticesList.at(idx3);
			Vec3 vec1 = v2 - v1;
			Vec3 vec2 = v3 - v1;
			Vec3 faceNormal(0, 0.0f, 0);
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
			Vec3 tmpTangent(1.0f, 0, 0);
			if (!(vn.x == 0 && vn.z == 0))
			{
				Vec3 tmpVec(-vn.z, 0, vn.x);
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
				tmpCompleteVertex.Color = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
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
		pCreatedMesh->mFunction_CreateGpuBufferAndUpdateData(completeVertexList, currentMesh.indicesList);

		//copy SUBSET lists
		pCreatedMesh->mSubsetInfoList = std::move(currentMesh.subsetList);

		//push_back Mesh ptr info for user
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
	MaterialManager* pMatMgr = GetScene()->GetMaterialMgr(); //m_pChildMaterialMgr;
	TextureManager* pTexMgr = GetScene()->GetTextureMgr();//m_pFatherScene->m_pChildTextureMgr;

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
}*/
