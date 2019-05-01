
/***********************************************************************

                           cpp：FBX SDK encapsulation

			control point splitting according to vertex 
			attribute is implemented

************************************************************************/
#include "Noise3D.h"
#include "Noise3D_InDevHeader.h"

using namespace Noise3D;

IFbxLoader::IFbxLoader() :
	m_pFbxMgr(nullptr),
	m_pFbxScene(nullptr),
	m_pIOSettings(nullptr),
	mIsInitialized(false),
	mEnableLoadMesh(false),
	mEnableLoadSkeleton(false),
	m_pRefCommonOutResult(nullptr)
{
}

IFbxLoader::~IFbxLoader()
{
	if(m_pIOSettings)m_pIOSettings->Destroy();
	if(m_pFbxScene)m_pFbxScene->Destroy();
	if(m_pFbxMgr)m_pFbxMgr->Destroy();
	m_pFbxMgr = nullptr;
	m_pFbxScene = nullptr;
	m_pIOSettings = nullptr;
	m_pRefCommonOutResult = nullptr;
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
	if(!_Initialize(fbxPath, loadMesh, loadSkeleton))return false;

	//output result reference binding
	m_pRefCommonOutResult = &outResult;

	//traverse scene graph to get scene objects(mesh, lights, camera, skeleton)
	FbxNode* pRNode= m_pFbxScene->GetRootNode();

	//(recursive), mesh and material (etc.) result will be loaded to RefOutResult
	_TraverseSceneNodes(pRNode);

	return true;
}

bool Noise3D::IFbxLoader::LoadPbrtMeshesFromFbx(NFilePath fbxPath, N_FbxPbrtSceneLoadingResult & outResult)
{
	if (!_Initialize(fbxPath, true, false))return false;

	//output result reference binding
	m_pRefPbrtOutResult = &outResult;

	//traverse scene graph to get scene objects(mesh, lights, camera, skeleton)
	FbxNode* pRNode = m_pFbxScene->GetRootNode();

	//(recursive), mesh and material (etc.) result will be loaded to RefOutResult
	_PBRT_TraverseSceneNodes(pRNode);

	return true;
}

/*********************************************

						P R I V A T E

*********************************************/
bool Noise3D::IFbxLoader::_Initialize(NFilePath fbxPath, bool loadMesh, bool loadSkeleton)
{
	if (!mIsInitialized)
	{
		ERROR_MSG("FBX Loader : loading failed! Fbx loader not initialized!");
		return false;
	}

	mEnableLoadMesh = loadMesh;
	mEnableLoadSkeleton = loadSkeleton;

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
	int versionMajor = 0, versionMinor = 0, versionRevision = 0;
	pImporter->GetFileVersion(versionMajor, versionMinor, versionRevision);

	//!!! IMPORT SCENE by importer, delete previous scene first
	m_pFbxScene->Clear();
	bool importSucessful = pImporter->Import(m_pFbxScene);//Multi-threaded could be used?

	 //import done, it's no longer necessary to keep importer alive
	pImporter->Destroy();

}

void IFbxLoader::_TraverseSceneNodes(FbxNode * pNode)
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
				m_pRefCommonOutResult->meshDataList.push_back(N_FbxMeshInfo());
				_ProcessSceneNode_Mesh(
					pNode, 
					m_pRefCommonOutResult->meshDataList.back(),
					MESH_MATERIAL_TYPE::LAMBERT_OR_PHONG);
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
				m_pRefCommonOutResult->skeletonList.push_back(N_FbxSkeletonInfo());
				_ProcessSceneNode_Skeleton(pNode); 
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
		_TraverseSceneNodes(pNode->GetChild(i));
}

void Noise3D::IFbxLoader::_PBRT_TraverseSceneNodes(FbxNode * pNode)
{
	if (pNode == nullptr)return;

	//type of the object which is bound to pNode
	//(note that a node might not be bound to some objects)
	FbxNodeAttribute* pAttr = pNode->GetNodeAttribute();

	//name of the scene node 
	N_UID nodeName = pNode->GetName();

	//this node is bound to some actual object
	if (pAttr != nullptr)
	{
		FbxNodeAttribute::EType attrType = pAttr->GetAttributeType();
		if (attrType == FbxNodeAttribute::eMesh)
		{
			m_pRefPbrtOutResult->meshDataList.push_back(N_FbxMeshInfo());
			_ProcessSceneNode_Mesh(
				pNode, 
				m_pRefPbrtOutResult->meshDataList.back(), 
				MESH_MATERIAL_TYPE::ADVANCED);
		}
	}

	//try to go deeper down the scene tree
	for (int i = 0; i < pNode->GetChildCount(); i++)
		_PBRT_TraverseSceneNodes(pNode->GetChild(i));
}

void IFbxLoader::_ProcessSceneNode_Mesh(FbxNode * pNode, N_FbxMeshInfo& outMeshInfo, MESH_MATERIAL_TYPE matType)
{
	//Get bound mesh of current node
	FbxMesh* pMesh = pNode->GetMesh();

	if (pMesh == nullptr)
	{
		ERROR_MSG("Fbx Loader : scene-node-bound mesh invalid. ");
		return;
	}

	std::vector<N_DefaultVertex>&	refVertexBuffer = outMeshInfo.vertexBuffer;
	std::vector<uint32_t>&	refIndexBuffer = outMeshInfo.indexBuffer;
	outMeshInfo.name = pNode->GetName();

	//---------------------------MESH TRANSFORMATION--------------------------
	//in 3dsmax export, 'Y axis up' and 'Z axis up' should be carefully processed
	//to convert handness correctly
	//对不起，忍不住说句中文了，这FBX文件的手性转换简直就是玄学吧？？
	FbxVector4 pos4	= pNode->EvaluateLocalTranslation();
	outMeshInfo.pos = Vec3(float(pos4.mData[0]), float(pos4.mData[2]),float(pos4.mData[1]));

	FbxVector4 scale4 = pNode->EvaluateLocalScaling();
	outMeshInfo.scale = Vec3(float(scale4.mData[0]), float(scale4.mData[2]), float(scale4.mData[1]));

	//euler angle decomposition from 3dsmax to noise3d
	//		R=Y1X2Z3 (right-handed, z-up) z~roll, x-pitch, y-yaw
	//			[c1c3+s1s2s3		c3s1s2-c1s3		c2s1	]
	//		=	[c2s3					c2c3					-s2	]
	//			[c1s2s3-c3s1		c1c3s2+s1s3	c1c2	]
	//initial extraction(right-handed)
	//XYZ(yaw pitch roll) --> YXZ(yaw pitch roll)
	FbxVector4 rotate4 = pNode->EvaluateLocalRotation();
	float rx = float(rotate4.mData[0]) / 180.0f *Ut::PI;
	float ry = float(rotate4.mData[1]) / 180.0f *Ut::PI;
	float rz = float(rotate4.mData[2]) / 180.0f *Ut::PI;
	//float c1 = cosf(rz), c2 = cosf(ry), c3 = cosf(rz);
	//float s1 = sinf(rz), s2 = sinf(ry), s3 = sinf(rz);
	Matrix mat = XMMatrixRotationRollPitchYaw(ry, rz, rx);//pitch, yaw, roll
	
	//(2019.3.11)use AffineTransform to help decompose euler angle in ZXY
	AffineTransform t;
	t.SetRigidTransformMatrix(mat);
	Vec3 euler = t.GetEulerAngleZXY();
	outMeshInfo.rotation = euler;
	//(deprecated code)
	/*float s2 = mat.m[1][2];
	float noiseEulerY = atan2(mat.m[0][2], mat.m[2][2]);
	float noiseEulerX = asin(-mat.m[1][2]);
	float noiseEulerZ = (s2 == 1.0f ? Ut::PI / 2.0f : asinf(mat.m[1][ 0] / sqrtf(1.0f - s2*s2)));
	refCurrentMesh.rotation = Vec3(noiseEulerX,noiseEulerY,noiseEulerZ);*/


	//--------------------------------MESH GEOMETRY--------------------------
	int triangleCount = 0;
	_LoadMeshGeometry(pMesh, refVertexBuffer, refIndexBuffer,triangleCount);

	//-----------------------------MATERIAL-------------------------------
	//1.subset
	std::vector<N_FbxMeshSubset> matIdSubsetList;
	_LoadMesh_MatIndexOfTriangles(pMesh, triangleCount, matIdSubsetList);

	switch (matType)
	{
	case MESH_MATERIAL_TYPE::LAMBERT_OR_PHONG:
	{
		//2.material(basic param & textures)
		_LoadMesh_Materials(pNode, outMeshInfo.matList);

		//3.convert materialID to material NAME( subset of mesh)
		std::vector<N_MeshSubsetInfo>& refSubsetList = outMeshInfo.subsetList;
		for (auto& s : matIdSubsetList)
		{
			N_MeshSubsetInfo subset;
			subset.matName = outMeshInfo.matList.at(s.matID).matName;
			subset.primitiveCount = s.triCount;
			subset.startPrimitiveID = s.triStart;
			refSubsetList.push_back(subset);
		}
		break; 
	}
	case MESH_MATERIAL_TYPE::ADVANCED:
	{
		//2.material(basic param & textures)
		_PBRT_LoadMesh_Materials(pNode, outMeshInfo.pbrtMatList);

		//3.convert materialID to material NAME( subset of mesh)
		std::vector<N_MeshSubsetInfo>& refSubsetList = outMeshInfo.subsetList;
		for (auto& s : matIdSubsetList)
		{
			N_MeshSubsetInfo subset;
			subset.matName = outMeshInfo.pbrtMatList.at(s.matID).matName;
			subset.primitiveCount = s.triCount;
			subset.startPrimitiveID = s.triStart;
			refSubsetList.push_back(subset);
		}
		break;
	}

	default:
		ERROR_MSG("fbx loader's bug! there is no such material type");
	}
}

void Noise3D::IFbxLoader::_LoadMeshGeometry(FbxMesh * pMesh, std::vector<N_DefaultVertex>& outVB, std::vector<uint32_t>& outIB, int& outTriangleCount)
{

	//1, Vertices -------- copy control points (vertices with unique position) to temp vertex buffer
	int ctrlPointCount = pMesh->GetControlPointsCount();
	FbxVector4* pCtrlPointArray = pMesh->GetControlPoints();
	outVB.resize(ctrlPointCount);
	for (int i = 0; i < ctrlPointCount; ++i)
	{
		FbxVector4& ctrlPoint = pCtrlPointArray[i];
		outVB.at(i).Pos.x = float(ctrlPoint.mData[0]);
		outVB.at(i).Pos.y = float(ctrlPoint.mData[2]);
		outVB.at(i).Pos.z = float(ctrlPoint.mData[1]);
	}


	//2, Indices ------- indices of control points to assemble triangles
	int indexCount = pMesh->GetPolygonVertexCount();
	int* pIndexArray = pMesh->GetPolygonVertices();
	outIB.resize(indexCount);
	for (int i = 0; i < indexCount; ++i)outIB.at(i) = pIndexArray[i];

	//3, other vertex attributes ----- vertex color/normal/tangent/texcoord
	//meshes are assumed to be TRIANGULAR here
	//in triangular mesh, poly-vertex count is 3x of triangleCount;
	//and many vertex attributes can be shared, hence
	int triangleCount = pMesh->GetPolygonCount();
	//int vertexCount = pMesh->GetPolygonVertexCount();

	//(but be careful, one control point can possess several normal/tangent....
	//thus control point could be split into several vertices with different vertex attributes.
	//vertex attributes will be loaded below, but if a "dirty" vertex is encountered again,
	//then a new vertex should be CREATED, index should be MODIFIED
	std::vector<bool> ctrlPointDirtyMarkArray(ctrlPointCount, false);

	for (int i = 0; i < triangleCount; ++i)
	{
		//3 vertices for each triangle
		for (int j = 0; j < 3; ++j)
		{
			int ctrlPointIndex = pMesh->GetPolygonVertex(i, j);
			int uvIndex = pMesh->GetTextureUVIndex(i, j);
			//int polygonVertexIndex = pMesh->GetPolygonVertexIndex(i)+j;
			int polygonVertexIndex = i * 3 + j;


			//load other vertex attributes for control points(or say, vertex, because
			//control point could be split according to each vertex attribute

			//vertex color
			Vec4 color;
			_LoadMesh_VertexColor(pMesh, ctrlPointIndex, polygonVertexIndex, color);

			//vertex normal
			Vec3 normal;
			_LoadMesh_VertexNormal(pMesh, ctrlPointIndex, polygonVertexIndex, normal);

			//vertex tangent (nice!!!)
			Vec3 tangent;
			_LoadMesh_VertexTangent(pMesh, ctrlPointIndex, polygonVertexIndex, tangent);

			//texture coordinates could be multiple layers, but we only support 1 layer here
			Vec2 texcoord;
			_LoadMesh_VertexTexCoord(pMesh, ctrlPointIndex, polygonVertexIndex, uvIndex, 0, texcoord);

			//if current control point has been loaded before,then we should test and 
			//determine whether we should split the vertex
			if (ctrlPointDirtyMarkArray.at(ctrlPointIndex) == true)
			{
				N_DefaultVertex existedV = outVB.at(ctrlPointIndex);
				N_DefaultVertex currentV;
				currentV.Pos = existedV.Pos;
				currentV.Color = color;
				currentV.Normal = normal;
				currentV.TexCoord = texcoord;
				currentV.Tangent = tangent;

				//Vertex attr not equal, split control point
				if (existedV != currentV)
				{
					outVB.push_back(currentV);
					//index of new vertex of splitting
					outIB.at(polygonVertexIndex) = outVB.size() - 1;
				}
			}
			else
			{
				//current vertex not dirty, attributes need to be initialized
				N_DefaultVertex& v = outVB.at(ctrlPointIndex);
				v.Color = color;
				v.Normal = normal;
				v.Tangent = tangent;
				v.TexCoord = texcoord;
				//set dirty mark
				ctrlPointDirtyMarkArray.at(ctrlPointIndex) = true;
			}
		}
	}

	//Clockwise/ counterClockwise
	for (int i = 0; i < indexCount; i += 3)
	{
		std::swap(outIB.at(i + 1), outIB.at(i + 2));
	}

	outTriangleCount = triangleCount;
}

void IFbxLoader::_LoadMesh_VertexColor(FbxMesh * pMesh, int ctrlPointIndex, int polygonVertexIndex, Vec4 & outColor)
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

void IFbxLoader::_LoadMesh_VertexNormal(FbxMesh * pMesh, int ctrlPointIndex, int polygonVertexIndex, Vec3 & outNormal)
{
	if (pMesh->GetElementNormalCount() < 1)
	{
		outNormal = { 0,0,0 };
		return;
	}

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

void IFbxLoader::_LoadMesh_VertexTangent(FbxMesh * pMesh, int ctrlPointIndex, int polygonVertexIndex, Vec3 & outTangent)
{
	if (pMesh->GetElementTangentCount() < 1)
	{
		outTangent = { 0,0,0 };
		return;
	}

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
			break;
		}

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = pElement->GetIndexArray().GetAt(ctrlPointIndex);
			v = pElement->GetDirectArray().GetAt(id);
			break;
		}

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
			break;
		}

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = pElement->GetIndexArray().GetAt(polygonVertexIndex);
			v = pElement->GetDirectArray().GetAt(id);
			break;
		}

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

void IFbxLoader::_LoadMesh_VertexTexCoord(FbxMesh * pMesh, int ctrlPointIndex, int polygonVertexIndex, int uvIndex, int uvLayer, Vec2 & outTexcoord)
{
	if (pMesh->GetElementUVCount() < 1)
	{
		outTexcoord = { 0,0 };
		return;
	}

	FbxGeometryElementUV* pElement = pMesh->GetElementUV(uvLayer);

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
			//this 2 case can be combined using uvIndex(tested 2018.3.14)
			case FbxGeometryElement::eDirect:
			case FbxGeometryElement::eIndexToDirect:
			{
				v = pElement->GetDirectArray().GetAt(uvIndex);
			}
			break;

		default:
			break;
		}
	}
	break;
	}

	//对不起我又要说一句中文，1.0f - texcoord.y 是几个意思？
	//黑人问号.jpg ？？
	//不就是导出的文件是 Y-Axis up吗？
	outTexcoord.x = float(v.mData[0]);
	outTexcoord.y = float(1.0f - v.mData[1]);
}

void IFbxLoader::_LoadMesh_VertexBinormal(FbxMesh * pMesh, int ctrlPointIndex, int polygonVertexIndex, Vec3 & outBinormal)
{
	if (pMesh->GetElementNormalCount() < 1)
	{
		outBinormal = { 0,0,0 };
		return;
	}

	FbxGeometryElementBinormal* pElement = pMesh->GetElementBinormal();

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

	outBinormal.x = float(v.mData[0]);
	outBinormal.y = float(v.mData[2]);
	outBinormal.z = float(v.mData[1]);
}

void IFbxLoader::_LoadMesh_MatIndexOfTriangles(FbxMesh * pMesh, int triangleCount, std::vector<N_FbxMeshSubset>& outFbxSubsetList)
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
			switch (matMappingMode)
			{
			//one Mat ID for one polygon(triangle)
			case FbxGeometryElement::eByPolygon:
				{
					//count check before loading material IDs
					if (pMatIndices->GetCount() == triangleCount)
					{
						//initial subset
						N_FbxMeshSubset initSubset;
						initSubset.triStart = 0;
						initSubset.triCount = 1;
						initSubset.matID = pMatIndices->GetAt(0);
						outFbxSubsetList.push_back(initSubset);

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

void IFbxLoader::_LoadMesh_Materials(FbxNode* pNode, std::vector<N_FbxMaterialInfo>& outMatList)
{
#pragma warning (push)
#pragma warning (disable : 4244)
	//materials are independent from mesh geometry data
	int materialCount = pNode->GetMaterialCount();
	
	//load every materials
	for (int materialIndex = 0; materialIndex < materialCount; materialIndex++)
	{
		FbxSurfaceMaterial* pSurfaceMaterial = pNode->GetMaterial(materialIndex);

		//material name
		N_FbxMaterialInfo newFbxMat;
		N_BasicLambertMaterialDesc& basicMat = newFbxMat.matBasicInfo;
		newFbxMat.matName = pSurfaceMaterial->GetName();

		// Phong material  
		if (pSurfaceMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			// Ambient Color  
			FbxDouble3 amb= ((FbxSurfacePhong*)pSurfaceMaterial)->Ambient;
			basicMat.ambientColor = Vec3(amb.mData[0], amb.mData[1], amb.mData[2]);

			// Diffuse Color  
			FbxDouble3 diff= ((FbxSurfacePhong*)pSurfaceMaterial)->Diffuse;
			basicMat.diffuseColor = Vec3(diff.mData[0], diff.mData[1], diff.mData[2]);
			
			// Specular Color  
			FbxDouble3 spec= ((FbxSurfacePhong*)pSurfaceMaterial)->Specular;
			basicMat.specularColor = Vec3(spec.mData[0], spec.mData[1], spec.mData[2]);

			// Emissive Color  
			FbxDouble3 emissive= ((FbxSurfacePhong*)pSurfaceMaterial)->Emissive;
			//there is no emissive var in material struct (2017.9.25)

			// Opacity  
			FbxDouble transparency = ((FbxSurfacePhong*)pSurfaceMaterial)->TransparencyFactor;
			basicMat.transparency = transparency;

			// Shininess  
			FbxDouble shininess = ((FbxSurfacePhong*)pSurfaceMaterial)->Shininess;
			basicMat.specularSmoothLevel = int32_t(shininess);

			// Reflectivity  
			FbxDouble reflectFactor= ((FbxSurfacePhong*)pSurfaceMaterial)->ReflectionFactor;
			basicMat.environmentMapTransparency = reflectFactor;

			//normal map bump intensity
			FbxDouble bumpFactor = ((FbxSurfacePhong*)pSurfaceMaterial)->BumpFactor;
			basicMat.normalMapBumpIntensity = bumpFactor;

		}
		// Lambert material  
		else if (pSurfaceMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{

			// Ambient Color  
			FbxDouble3 amb= ((FbxSurfaceLambert*)pSurfaceMaterial)->Ambient;
			basicMat.ambientColor = Vec3(amb.mData[0], amb.mData[1], amb.mData[2]);

			// Diffuse Color  
			FbxDouble3 diff= ((FbxSurfaceLambert*)pSurfaceMaterial)->Diffuse;
			basicMat.diffuseColor = Vec3(diff.mData[0], diff.mData[1], diff.mData[2]);

			// Emissive Color  
			FbxDouble3 emissive= ((FbxSurfaceLambert*)pSurfaceMaterial)->Emissive;
			//there is no emissive var in material struct (2017.9.25)

			// Opacity  
			FbxDouble transparency= ((FbxSurfaceLambert*)pSurfaceMaterial)->TransparencyFactor;
			basicMat.transparency = transparency;
		}

		//load textures infos
		N_FbxTextureMapsInfo& texMapsInfo = newFbxMat.texMapInfo;
		_LoadMesh_Material_Textures(pSurfaceMaterial, texMapsInfo);

		outMatList.push_back(newFbxMat);
	}
#pragma warning (pop)
}

void Noise3D::IFbxLoader::_PBRT_LoadMesh_Materials(FbxNode * pNode, std::vector<N_FbxPbrtMaterialInfo>& outMatList)
{
	//(2019.5.1)for the reason that 3DMAX might not support all of the PBRT materials' attribute,
	//there will be some name re-mapping to the material.
	//phong material will be loaded and re-map to PBRT material.
	// for example:
	// diffuse color--->albedo
	// specular color ---> metal_F0
	// specular level ---> metallicity
	// glossiness ---> roughness
	// emissive ---> emissive
	// opacity ---> transparency

#pragma warning (push)
#pragma warning (disable : 4244)
	//materials are independent from mesh geometry data
	int materialCount = pNode->GetMaterialCount();

	//load every materials
	for (int materialIndex = 0; materialIndex < materialCount; materialIndex++)
	{
		FbxSurfaceMaterial* pSurfaceMaterial = pNode->GetMaterial(materialIndex);

		//material name
		N_FbxPbrtMaterialInfo newFbxMat;
		GI::N_PbrtMatDesc& basicMat = newFbxMat.matBasicInfo;
		newFbxMat.matName = pSurfaceMaterial->GetName();

		if (pSurfaceMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			// albedo <-- Diffuse Color  
			FbxDouble3 albedo = ((FbxSurfacePhong*)pSurfaceMaterial)->Diffuse;
			basicMat.albedo = Color4f(albedo.mData[0], albedo.mData[1], albedo.mData[2], 1.0f);

			// metal_F0 <-- specular color
			FbxDouble3 metal_F0 = ((FbxSurfacePhong*)pSurfaceMaterial)->Specular;
			basicMat.metal_F0 = Vec3(metal_F0.mData[0], metal_F0.mData[1], metal_F0.mData[2]);

			// metallicity <-- Specular intensity  
			FbxDouble metallicity = ((FbxSurfacePhong*)pSurfaceMaterial)->SpecularFactor;
			basicMat.metallicity = metallicity;

			// Emissive Color  
			FbxDouble3 emissive = ((FbxSurfacePhong*)pSurfaceMaterial)->Emissive;
			basicMat.emission = Vec3(emissive.mData[0], emissive.mData[1], emissive.mData[2]);

			// transparency  
			FbxDouble transparency = ((FbxSurfacePhong*)pSurfaceMaterial)->TransparencyFactor;
			basicMat.transparency = transparency;

			// Roughness <-- Shininess  
			FbxDouble shininess = ((FbxSurfacePhong*)pSurfaceMaterial)->Shininess;
			basicMat.roughness = 1.0f - shininess / 100.0f;

			//normal map bump intensity
			//FbxDouble bumpFactor = ((FbxSurfacePhong*)pSurfaceMaterial)->BumpFactor;
			//basicMat.normalMapBumpIntensity = bumpFactor;
		}

		//load textures infos
		N_FbxPbrtTextureMapsInfo& texMapsInfo = newFbxMat.texMapInfo;
		_PBRT_LoadMesh_Material_Textures(pSurfaceMaterial, texMapsInfo);

		outMatList.push_back(newFbxMat);
	}
#pragma warning (pop)
}

void IFbxLoader::_LoadMesh_Material_Textures(FbxSurfaceMaterial* pSM, N_FbxTextureMapsInfo& outTexInfo)
{
	//diffuse map
	FbxProperty prop = pSM->FindProperty(FbxSurfaceMaterial::sDiffuse);
	mFunction_LoadMesh_Material_TextureMapInfo(
		prop, outTexInfo.diffMapName, outTexInfo.diffMapFilePath);

	//normal map
	prop = pSM->FindProperty(FbxSurfaceMaterial::sBump);
	mFunction_LoadMesh_Material_TextureMapInfo(
		prop, outTexInfo.normalMapName, outTexInfo.normalMapFilePath);

	//specular map
	prop = pSM->FindProperty(FbxSurfaceMaterial::sSpecular);
	mFunction_LoadMesh_Material_TextureMapInfo(
		prop, outTexInfo.specMapName, outTexInfo.specMapFilePath);

	//emissive map
	prop = pSM->FindProperty(FbxSurfaceMaterial::sEmissive);
	mFunction_LoadMesh_Material_TextureMapInfo(
		prop, outTexInfo.emissiveMapName, outTexInfo.emissiveMapFilePath);
}

void Noise3D::IFbxLoader::_PBRT_LoadMesh_Material_Textures(FbxSurfaceMaterial * pSM, N_FbxPbrtTextureMapsInfo & outTexInfo)
{
	//albedo map
	FbxProperty prop = pSM->FindProperty(FbxSurfaceMaterial::sDiffuse);
	mFunction_LoadMesh_Material_TextureMapInfo(
		prop, outTexInfo.albedoMapName, outTexInfo.albedoMapFilePath);

	//normal map (2019.5.1, not supported by now)
	//prop = pSM->FindProperty(FbxSurfaceMaterial::sBump);
	//mFunction_LoadMesh_Material_TextureMapInfo(
	//	prop, outTexInfo.normalMapName, outTexInfo.normalMapFilePath);

	//roughness map
	prop = pSM->FindProperty(FbxSurfaceMaterial::sShininess);
	mFunction_LoadMesh_Material_TextureMapInfo(
		prop, outTexInfo.roughnessMapName, outTexInfo.roughnessMapFilePath);

	//metallicity map
	prop = pSM->FindProperty(FbxSurfaceMaterial::sSpecularFactor);
	mFunction_LoadMesh_Material_TextureMapInfo(
		prop, outTexInfo.metallicityMapName, outTexInfo.metallicityMapFilePath);

	//emissive map
	prop = pSM->FindProperty(FbxSurfaceMaterial::sEmissive);
	mFunction_LoadMesh_Material_TextureMapInfo(
		prop, outTexInfo.emissiveMapName, outTexInfo.emissiveMapFilePath);
}

void IFbxLoader::mFunction_LoadMesh_Material_TextureMapInfo(const FbxProperty & prop, std::string & outTextureName, std::string & outTextureFilePath)
{
	int layerCount = prop.GetSrcObjectCount<FbxLayeredTexture>();

	//texture could be layered/Non-layered
	if (layerCount > 0)//layered
	{
		//for (int texLayerIndex = 0; texLayerIndex < layerCount; ++texLayerIndex)
		//{
		//FbxLayeredTexture* pLayeredTexture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(texLayerIndex));
		FbxLayeredTexture* pLayeredTexture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(0));
		int texCount = pLayeredTexture->GetSrcObjectCount<FbxTexture>();

		for (int k = 0; k < texCount; k++)
		{
			FbxFileTexture* pTex = FbxCast<FbxFileTexture>(pLayeredTexture->GetSrcObject<FbxTexture>(k));
			// Then, you can get all the properties of the texture, include its name
			if (pTex != nullptr)
			{
				outTextureName = pTex->GetName();
				outTextureFilePath = pTex->GetFileName();
			}
		}
		//}
	}
	else//non-layered
	{
		int texCount = prop.GetSrcObjectCount<FbxTexture>();
		//iterate all simple texture
		for (int texId = 0; texId<texCount; texId++)
		{
			FbxFileTexture * pTex = FbxCast<FbxFileTexture>(prop.GetSrcObject<FbxTexture>(texId));
			if (pTex != nullptr)
			{
				outTextureName = pTex->GetName();
				outTextureFilePath = pTex->GetFileName();
			}
		}
	}
}

void IFbxLoader::_ProcessSceneNode_Skeleton(FbxNode * pNode)
{
	//not implemented(2017.9.20)
}
