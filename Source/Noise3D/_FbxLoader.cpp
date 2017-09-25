
/***********************************************************************

                           cpp£ºFBX SDK encapsulation

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

IFbxLoader::IFbxLoader() :
	m_pFbxMgr(nullptr),
	m_pFbxScene(nullptr),
	m_pIOSettings(nullptr),
	mIsInitialized(false),
	mEnableLoadMesh(false),
	mEnableLoadSkeleton(false),
	m_pResult(nullptr)
{
}

IFbxLoader::~IFbxLoader()
{
	m_pIOSettings->Destroy();
	m_pFbxScene->Destroy();
	m_pFbxMgr->Destroy();
	m_pFbxMgr = nullptr;
	m_pFbxScene = nullptr;
	m_pIOSettings = nullptr;
	m_pResult = nullptr;
	mIsInitialized = false;
}

bool IFbxLoader::Initialize()
{
	if (mIsInitialized)return true;

	//FBX Manager object handles fbxsdk memory
	m_pFbxMgr = FbxManager::Create();

	if (m_pFbxMgr == nullptr)
	{
		ERROR_MSG("FBX Loader : fbx manager fails to init. ");
		return false;
	}

	//Fbx scene handles scene graph and scene objects
	m_pFbxScene = FbxScene::Create(m_pFbxMgr, "myFbxScene");

	if (m_pFbxScene == nullptr)
	{
		m_pFbxMgr->Destroy();
		ERROR_MSG("FBX Loader : fbx scene fails to init. ");
		return false;
	}

	// Create the IO settings object
	m_pIOSettings = FbxIOSettings::Create(m_pFbxMgr, IOSROOT);

	if (m_pIOSettings == nullptr)
	{
		m_pFbxMgr->Destroy();
		m_pFbxScene->Destroy();
		ERROR_MSG("FBX Loader : fbx IO settings fails to init. ");
		return false;
	}

	mIsInitialized = true;
	return true;
}

bool IFbxLoader::LoadSceneFromFbx(NFilePath fbxPath, N_FbxLoadingResult& outResult, bool loadMesh,  bool loadSkeleton)
{
	if (!mIsInitialized)
	{
		ERROR_MSG("FBX Loader : loading failed! Fbx loader not initialized!");
		return false;
	}

	mEnableLoadMesh		= loadMesh;
	mEnableLoadSkeleton	= loadSkeleton;

	//configure IO settings
	m_pFbxMgr->SetIOSettings(m_pIOSettings);

	//Create an importer. 
	//note that importer is initialized by .fbx file path (which means
	//importer is init-ed every time this Load Fbx function is invoked
	FbxImporter*	pImporter = FbxImporter::Create(m_pFbxMgr, "");

	// Use the first argument as the filename for the importer.
	if (!pImporter->Initialize(fbxPath.c_str(), -1, m_pIOSettings))
	{
		ERROR_MSG(std::string("FBX Loader: fbx importer init failed! error details : ") 
			+ pImporter->GetStatus().GetErrorString());
		return false;
	}

	//get fbx file version 
	int versionMajor=0, versionMinor=0,versionRevision=0;
	pImporter->GetFileVersion(versionMajor, versionMinor, versionRevision);

	//!!! IMPORT SCENE by importer, delete previous scene first
	m_pFbxScene->Clear();
	bool importSucessful = pImporter->Import(m_pFbxScene);//Multi-threaded could be used?

	//import done, it's no longer necessary to keep importer alive
	pImporter->Destroy();

	//traverse scene graph to get scene objects(mesh, lights, camera, skeleton)
	FbxNode* pRNode= m_pFbxScene->GetRootNode();

	//output result reference binding
	m_pResult = &outResult;

	//(recursive)
	mFunction_TraverseSceneNodes(pRNode);

	return true;
}


/*********************************************

						P R I V A T E

*********************************************/
void IFbxLoader::mFunction_TraverseSceneNodes(FbxNode * pNode)
{
	if (pNode == nullptr)return;

	//type of the object which is bound to pNode
	//(note that a node might not be bound to some objects)
	FbxNodeAttribute* pAttr = pNode->GetNodeAttribute();

	//name of the scene node 
	N_UID nodeName =  pNode->GetName();

	//this node is bound to some actual object
	if (pAttr != nullptr)
	{
		FbxNodeAttribute::EType attrType = pAttr->GetAttributeType();

		switch (attrType)
		{
		case FbxNodeAttribute::eMesh:
		{
			if (mEnableLoadMesh)
			{ 
				m_pResult->meshDataList.push_back(N_FbxMeshInfo());
				mFunction_ProcessSceneNode_Mesh(pNode); 
			}
			break;
		}

		case FbxNodeAttribute::eLight:
		{
			//if (mEnableLoadLight) {mFunction_ProcessSceneNode_Light(pNode); }
			break;
		}

		case FbxNodeAttribute::eSkeleton:
		{
			if (mEnableLoadSkeleton)
			{
				m_pResult->skeletonList.push_back(N_FbxSkeletonInfo());
				mFunction_ProcessSceneNode_Skeleton(pNode); 
			}
			break;
		}

		case FbxNodeAttribute::eCamera:
		{
			//if (mEnableLoadCamera){mFunction_ProcessSceneNode_Camera();}
			break;
		}

			default:break;
		}
	}

	//try to go deeper down the scene tree
	for (int i = 0; i < pNode->GetChildCount(); i++)
		mFunction_TraverseSceneNodes(pNode->GetChild(i));
}

void IFbxLoader::mFunction_ProcessSceneNode_Mesh(FbxNode * pNode)
{
	//Get bound mesh of current node
	FbxMesh* pMesh = pNode->GetMesh();

	if (pMesh == nullptr)
	{
		ERROR_MSG("Fbx Loader : scene-node-bound mesh invalid. ");
		return;
	}

	N_FbxMeshInfo& currentMesh = m_pResult->meshDataList.back();
	std::vector<N_DefaultVertex>&	vertexBuffer = currentMesh.vertexBuffer;
	std::vector<UINT>&	indexBuffer = currentMesh.indexBuffer;
	std::vector<N_MeshSubsetInfo>& subsetList = currentMesh.subsetList;
	currentMesh.name = pNode->GetName();

	//--------------------MESH GEOMETRY--------------------------

	//1, Vertices -------- copy control points (vertices with unique position) to temp vertex buffer
	int ctrlPointCount = pMesh->GetControlPointsCount();
	FbxVector4* pCtrlPointArray = pMesh->GetControlPoints();
	vertexBuffer.resize(ctrlPointCount);
	for (int i = 0; i < ctrlPointCount; ++i)
	{
		FbxVector4& ctrlPoint = pCtrlPointArray[i];
		vertexBuffer.at(i).Pos.x = float(ctrlPoint.mData[0]);
		vertexBuffer.at(i).Pos.y = float(ctrlPoint.mData[2]);
		vertexBuffer.at(i).Pos.z = float(ctrlPoint.mData[1]);
	}

	//2, Indices ------- indices of control points to assemble triangles
	int indexCount = pMesh->GetPolygonVertexCount();
	int* pIndexArray = pMesh->GetPolygonVertices();
	indexBuffer.resize(indexCount);
	for (int i = 0; i < indexCount; ++i)
	{
		indexBuffer.at(i) = pIndexArray[i];
	}

	//3, other vertex attributes ----- vertex color/normal/tangent/texcoord
	//meshes are assumed to be TRIANGULAR here
	//in triangular mesh, poly-vertex count is 3x of triangleCount;
	//and many vertex attributes can be shared, hence
	int triangleCount = pMesh->GetPolygonCount();
	//int vertexCount = pMesh->GetPolygonVertexCount();
	for (int i = 0; i < triangleCount; ++i)
	{
		//3 vertices for each triangle
		for (int j = 0; j < 3; ++j)
		{
			int ctrlPointIndex = pMesh->GetPolygonVertex(i, j);
			int polygonVertexIndex = i * 3 + j;

			//load other vertex attributes for control points
			NVECTOR4& color = vertexBuffer.at(ctrlPointIndex).Color;
			NVECTOR3& tangent = vertexBuffer.at(ctrlPointIndex).Tangent;
			NVECTOR3& normal = vertexBuffer.at(ctrlPointIndex).Normal;
			NVECTOR2& texcoord = vertexBuffer.at(ctrlPointIndex).TexCoord;

			//vertex color
			mFunction_LoadMesh_VertexColor(pMesh, ctrlPointIndex, polygonVertexIndex, color);

			//vertex normal
			mFunction_LoadMesh_VertexNormal(pMesh, ctrlPointIndex, polygonVertexIndex, normal);

			//vertex tangent (nice!!!)
			mFunction_LoadMesh_VertexTangent(pMesh, ctrlPointIndex, polygonVertexIndex, tangent);

			//texture coordinates could be multiple layers, but we only support 1 layer here
			mFunction_LoadMesh_VertexTexCoord(pMesh, ctrlPointIndex, polygonVertexIndex, 0, texcoord);
		}
	}

	//---------------------------MATERIAL----------------------
	std::vector<N_FbxMeshSubset> matIdSubsetList;
	mFunction_LoadMesh_MatIndexOfTriangles(pMesh, triangleCount, matIdSubsetList);

	std::vector<std::string> matNameList;
	std::vector<N_MaterialDesc> matList;
	mFunction_LoadMesh_Materials(pNode,matNameList ,matList);


}

void IFbxLoader::mFunction_LoadMesh_VertexColor(FbxMesh * pMesh, int ctrlPointIndex, int polygonVertexIndex, NVECTOR4 & outColor)
{
	//if no vertex color is defined, output an zero-ed color
	if (pMesh->GetElementVertexColorCount() < 1)
	{
		outColor = { 0,0,0,0 };
		return;
	}

	//get an array of vertex color(indexed by control point index OR polygon vertex index)
	FbxGeometryElementVertexColor* pElement = pMesh->GetElementVertexColor();

	//target vector
	FbxColor& v = pElement->GetDirectArray().GetAt(0);

	//re-bind variable 'v' according to index mapping mode and ref mode
	switch (pElement->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			v = pElement->GetDirectArray().GetAt(ctrlPointIndex);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = pElement->GetIndexArray().GetAt(ctrlPointIndex);
			v = pElement->GetDirectArray().GetAt(id);
		}
		break;

		default:
			break;
		}
	}
	break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			v = pElement->GetDirectArray().GetAt(polygonVertexIndex);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = pElement->GetIndexArray().GetAt(polygonVertexIndex);
			v = pElement->GetDirectArray().GetAt(id);
		}
		break;
		default:
			break;
		}
	}
	break;
	}


	outColor.x = float(v.mRed);
	outColor.y = float(v.mGreen);
	outColor.z = float(v.mBlue);
	outColor.w = float(v.mAlpha);
}

void IFbxLoader::mFunction_LoadMesh_VertexNormal(FbxMesh * pMesh, int ctrlPointIndex, int polygonVertexIndex, NVECTOR3 & outNormal)
{
	if (pMesh->GetElementNormalCount() < 1)
	{
		outNormal = { 0,0,0 };
		return;
	}

	//get an array of vertex color(indexed by control point index OR polygon vertex index)
	FbxGeometryElementNormal* pElement = pMesh->GetElementNormal();

	//target vector
	FbxVector4& v=pElement->GetDirectArray().GetAt(0);

	//re-bind variable 'v' according to index mapping mode and ref mode
	switch (pElement->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			v = pElement->GetDirectArray().GetAt(ctrlPointIndex);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = pElement->GetIndexArray().GetAt(ctrlPointIndex);
			v = pElement->GetDirectArray().GetAt(id);
		}
		break;

		default:
			break;
		}
	}
	break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			v = pElement->GetDirectArray().GetAt(polygonVertexIndex);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = pElement->GetIndexArray().GetAt(polygonVertexIndex);
			v = pElement->GetDirectArray().GetAt(id);
		}
		break;
		default:
			break;
		}
	}
	break;
	}

	outNormal.x = float(v.mData[0]);
	outNormal.y = float(v.mData[2]);
	outNormal.z = float(v.mData[1]);
}

void IFbxLoader::mFunction_LoadMesh_VertexTangent(FbxMesh * pMesh, int ctrlPointIndex, int polygonVertexIndex, NVECTOR3 & outTangent)
{
	if (pMesh->GetElementTangentCount() < 1)
	{
		outTangent = { 0,0,0 };
		return;
	}

	//get an array of vertex color(indexed by control point index OR polygon vertex index)
	FbxGeometryElementTangent* pElement = pMesh->GetElementTangent();

	//target vector
	FbxVector4& v = pElement->GetDirectArray().GetAt(0);

	//re-bind variable 'v' according to index mapping mode and ref mode
	switch (pElement->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			v = pElement->GetDirectArray().GetAt(ctrlPointIndex);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = pElement->GetIndexArray().GetAt(ctrlPointIndex);
			v = pElement->GetDirectArray().GetAt(id);
		}
		break;

		default:
			break;
		}
	}
	break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			v = pElement->GetDirectArray().GetAt(polygonVertexIndex);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = pElement->GetIndexArray().GetAt(polygonVertexIndex);
			v = pElement->GetDirectArray().GetAt(id);
		}
		break;
		default:
			break;
		}
	}
	break;
	}

	outTangent.x = float(v.mData[0]);
	outTangent.y = float(v.mData[2]);
	outTangent.z = float(v.mData[1]);
}

void IFbxLoader::mFunction_LoadMesh_VertexTexCoord(FbxMesh * pMesh, int ctrlPointIndex, int uvIndex, int uvLayer, NVECTOR2 & outTexcoord)
{
	if (pMesh->GetElementUVCount() < 1)
	{
		outTexcoord = { 0,0 };
		return;
	}

	//get an array of vertex color(indexed by control point index OR polygon vertex index)
	FbxGeometryElementUV* pElement = pMesh->GetElementUV();

	//target vector
	FbxVector2& v = pElement->GetDirectArray().GetAt(0);

	//re-bind variable 'v' according to index mapping mode and ref mode
	switch (pElement->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			v = pElement->GetDirectArray().GetAt(ctrlPointIndex);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = pElement->GetIndexArray().GetAt(ctrlPointIndex);
			v = pElement->GetDirectArray().GetAt(id);
		}
		break;

		default:
			break;
		}
	}
	break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (pElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
		//in the following tutorial, eDirect case is combined with eIndexToDirect.... for unknown reason
		//file:///F:/1VS%20PROJECT/3D/_%E8%B5%84%E6%96%99/%E5%9F%BA%E4%BA%8EFBX%20SDK%E7%9A%84FBX%E6%A8%A1%E5%9E%8B%E8%A7%A3%E6%9E%90%E4%B8%8E%E5%8A%A0%E8%BD%BD%20-%EF%BC%88%E4%B8%80%EF%BC%89%20-%20BugRunner%E7%9A%84%E4%B8%93%E6%A0%8F%20-%20%E5%8D%9A%E5%AE%A2%E9%A2%91%E9%81%93%20-%20CSDN.NET.htm
			v = pElement->GetDirectArray().GetAt(uvIndex);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = pElement->GetIndexArray().GetAt(uvIndex);
			v = pElement->GetDirectArray().GetAt(id);
		}
		break;
		default:
			break;
		}
	}
	break;
	}

	outTexcoord.x = float(v.mData[0]);
	outTexcoord.y = float(v.mData[1]);
}

void IFbxLoader::mFunction_LoadMesh_MatIndexOfTriangles(FbxMesh * pMesh, int triangleCount, std::vector<N_FbxMeshSubset>& outFbxSubsetList)
{
	FbxLayerElementArrayTemplate<int>*	pMatIndices	=nullptr;
	FbxGeometryElement::EMappingMode	matMappingMode = FbxGeometryElement::eNone;

	//a mesh might haven't bind any material
	if (pMesh->GetElementMaterial())
	{
		pMatIndices = &pMesh->GetElementMaterial()->GetIndexArray();
		matMappingMode = pMesh->GetElementMaterial()->GetMappingMode();
		
		if (pMatIndices!=nullptr)
		{
			//initial subset
			N_FbxMeshSubset initSubset;
			initSubset.triStart = 0;
			initSubset.triCount = 1;
			initSubset.matID = pMatIndices->GetAt(0);
			outFbxSubsetList.push_back(initSubset);

			switch (matMappingMode)
			{
			//one Mat ID for one polygon(triangle)
			case FbxGeometryElement::eByPolygon:
				{
					//count check before loading material IDs
					if (pMatIndices->GetCount() == triangleCount)
					{
						for (int triangleIndex = 1; triangleIndex < triangleCount; ++triangleIndex)
						{
							int materialIndex = pMatIndices->GetAt(triangleIndex);

							//subset grows if next index is identical with current subset
							if (materialIndex == outFbxSubsetList.back().matID)
							{
								++outFbxSubsetList.back().triCount;
							}
							else
							{
								N_FbxMeshSubset newSubset;
								initSubset.triStart = triangleIndex;
								initSubset.triCount = 1;
								initSubset.matID = materialIndex;
								outFbxSubsetList.push_back(newSubset);
							}

						}
					}
				}
			break;

			//one material for the whole mesh(all triangles)
			case FbxGeometryElement::eAllSame:
				{
				N_FbxMeshSubset initSubset;
				initSubset.triStart = 0;
				initSubset.triCount = triangleCount;
				initSubset.matID = pMatIndices->GetAt(0);
				outFbxSubsetList.push_back(initSubset);
				}
			}//switch
		}
	}
}

void IFbxLoader::mFunction_LoadMesh_Materials(FbxNode* pNode, std::vector<std::string>& outMatNameList, std::vector<N_MaterialDesc>& outMatList)
{
	//materials are independent from mesh geometry data
	int materialCount = pNode->GetMaterialCount();
	
	//load every materials
	for (int materialIndex = 0; materialIndex < materialCount; materialIndex++)
	{
		FbxSurfaceMaterial* pSurfaceMaterial = pNode->GetMaterial(materialIndex);

		//material name
		std::string matName;
		matName = pSurfaceMaterial->GetName();

		N_MaterialDesc mat;
		// Phong material  
		if (pSurfaceMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			// Ambient Color  
			FbxDouble3 amb= ((FbxSurfacePhong*)pSurfaceMaterial)->Ambient;
			mat.ambientColor = NVECTOR3(amb.mData[0], amb.mData[1], amb.mData[2]);

			// Diffuse Color  
			FbxDouble3 diff= ((FbxSurfacePhong*)pSurfaceMaterial)->Diffuse;
			mat.diffuseColor = NVECTOR3(diff.mData[0], diff.mData[1], diff.mData[2]);
			
			// Specular Color  
			FbxDouble3 spec= ((FbxSurfacePhong*)pSurfaceMaterial)->Specular;
			mat.specularColor = NVECTOR3(spec.mData[0], spec.mData[1], spec.mData[2]);

			// Emissive Color  
			FbxDouble3 emissive= ((FbxSurfacePhong*)pSurfaceMaterial)->Emissive;
			//there is no emissive var in material struct (2017.9.25)

			// Opacity  
			FbxDouble transparency = ((FbxSurfacePhong*)pSurfaceMaterial)->TransparencyFactor;
			mat.transparency = transparency;

			// Shininess  
			FbxDouble shininess = ((FbxSurfacePhong*)pSurfaceMaterial)->Shininess;
			mat.specularSmoothLevel = int32_t(shininess);

			// Reflectivity  
			FbxDouble reflectFactor= ((FbxSurfacePhong*)pSurfaceMaterial)->ReflectionFactor;
			mat.environmentMapTransparency = reflectFactor;
		}
		// Lambert material  
		else if (pSurfaceMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{

			// Ambient Color  
			FbxDouble3 amb= ((FbxSurfaceLambert*)pSurfaceMaterial)->Ambient;
			mat.ambientColor = NVECTOR3(amb.mData[0], amb.mData[1], amb.mData[2]);

			// Diffuse Color  
			FbxDouble3 diff= ((FbxSurfaceLambert*)pSurfaceMaterial)->Diffuse;
			mat.diffuseColor = NVECTOR3(diff.mData[0], diff.mData[1], diff.mData[2]);

			// Emissive Color  
			FbxDouble3 emissive= ((FbxSurfaceLambert*)pSurfaceMaterial)->Emissive;
			//there is no emissive var in material struct (2017.9.25)

			// Opacity  
			FbxDouble transparency= ((FbxSurfaceLambert*)pSurfaceMaterial)->TransparencyFactor;
			mat.transparency = transparency;

			return;
		}
	}

}

void IFbxLoader::mFunction_ProcessSceneNode_Skeleton(FbxNode * pNode)
{
	//not implemented(2017.9.20)
}
