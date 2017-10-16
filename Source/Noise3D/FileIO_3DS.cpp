/***********************************************************************

							CLASS: IFileLoader_3ds

				Description: 3ds file loading implementation

************************************************************************/

#ifdef NOISE_ENABLE_3DS_LOADING

#include "Noise3D.h"
using namespace Noise3D;

//-----------------file & file operation----------------
#define BINARY_READ(var) \
fileIn.read((char*)&var,sizeof(var));

static std::ifstream fileIn;
static uint64_t static_fileSize = 0;
static uint64_t static_currentChunkFileEndPos = 0;
static std::string static_objectBlockParsedName;

//----------------static var of 3ds loader---------------
const std::string	IFileIO_3DS::c_defaultTexNamePrefix = std::string("T_");
const std::string	IFileIO_3DS::c_defaultMatNamePrefix = std::string("M_");
const UINT			IFileIO_3DS::c_chunkHeadByteLength = 6;
UINT					IFileIO_3DS::mMeshIndex = 0;
UINT					IFileIO_3DS::mTextureMapIndex = 0;


IFileIO_3DS::IFileIO_3DS()
{
	m_pMeshObjList		= new	std::vector<N_Load3ds_MeshObject>;	
	m_pMaterialList		= new	std::vector<N_MaterialDesc>	;
	m_pMatNameList	= new	std::vector<std::string>;
	m_pTexName2FilePathPairList=new std::unordered_map<std::string, NFilePath>;	//string as UID, FilePath is used to load file

}

bool IFileIO_3DS::ImportFile_3DS(
	NFilePath pFilePath,
	std::vector<N_Load3ds_MeshObject>& outMeshInfoList,
	std::vector<N_MaterialDesc>& outMaterialList,
	std::vector<std::string>& outMatNameList,
	std::unordered_map<std::string, NFilePath>& out_TexName2FilePathPairList)
{
	m_pMeshObjList->clear();
	m_pMaterialList->clear();
	m_pTexName2FilePathPairList->clear();
	m_pMatNameList->clear();

	fileIn.open(pFilePath, std::ios::binary);

	if (!fileIn.good())
	{
		ERROR_MSG("Noise File Manager : Import .3ds file failed!!");
		return false;
	}

	fileIn.seekg(0, std::ios::end);
	static_currentChunkFileEndPos = 0;
	static_fileSize = fileIn.tellg();
	fileIn.seekg(0);

	//maybe linearly deal with chunks is also OK....
	//stack will be used when necessary because recursive solution
	//can sometimes be rewritten in non-recursive form using stack.
	while (!fileIn.eof() && fileIn.good())
	{
		ReadChunk();
	}
	fileIn.close();

	//Finish Parsing,check if Vertices/Indices had been loaded
	auto& vertexList = m_pMeshObjList->back().verticesList;
	if (vertexList.size() == 0 || vertexList.size() == 0)
	{
		ERROR_MSG("Noise File Manager :Data Damaged!!!No Vertices or Indices loaded!");
		return false;
	}


	//std::move transform lval into rval to avoid copying happens
	outMeshInfoList = std::move(*m_pMeshObjList);
	outMaterialList = std::move(*m_pMaterialList);
	outMatNameList = std::move(*m_pMatNameList);
	out_TexName2FilePathPairList = std::move(*m_pTexName2FilePathPairList);


	return TRUE;
}



/************************************************************************

										P R I V A T E

*************************************************************************/



//when encountering not interested chunk, use absolute end file pos of the chunk to skip
void IFileIO_3DS::SkipCurrentChunk()
{
	//sometimes we will encounter error / corrupted data , we must shutdown the reading 
	//of this chunk
	if (static_currentChunkFileEndPos < static_fileSize)
	{
		fileIn.seekg(static_currentChunkFileEndPos);
	}
	else
	{
		fileIn.seekg(0, std::ios::end);
	}
}

//Linearly read chunks
void IFileIO_3DS::ReadChunk()
{
	//because 3DS file stores data with a tree topology, so i do have considered 
	//recursive/tree-like reading...but there is some mysterious chunks which might vary
	//from time to time... ehh...linear reading will be clearer???

	const UINT c_chunkHeadByteLength = 6;

	//2 numbers at the head of every chunk
	uint16_t chunkID = 0;
	uint32_t chunkLength = 0;

	BINARY_READ(chunkID);
	BINARY_READ(chunkLength);

	//if chunk length corrupted , length data might be invalid
	if (chunkLength > static_fileSize)
	{
		static_currentChunkFileEndPos = static_fileSize - 1;
		fileIn.seekg(0, std::ios::end);
		ERROR_MSG("Load 3ds File : chunk is too long ! 3ds file might be damaged!!!");
		return;
	}

	uint64_t tmpFilePos = fileIn.tellg();
	static_currentChunkFileEndPos = tmpFilePos + chunkLength - c_chunkHeadByteLength;

	//in CURRENT chunk, there can be several sub-chunks to read
	switch (chunkID)
	{
	case NOISE_3DS_CHUNKID_MAIN3D:
		ParseMainChunk();//level1
		break;

	case NOISE_3DS_CHUNKID_3D_EDITOR_CHUNK:
		Parse3DEditorChunk();//level2
		break;

	case NOISE_3DS_CHUNKID_OBJECT_BLOCK:
		ParseObjectBlock();//level3
		break;

	case NOISE_3DS_CHUNKID_TRIANGULAR_MESH:
		ParseTriangularMeshChunk();//level4
		break;

	case NOISE_3DS_CHUNKID_VERTICES_LIST:
		ParseVerticesChunk();//level5
		break;

	case NOISE_3DS_CHUNKID_FACES_DESCRIPTION:
		ParseFaceDescAndIndices();//level5
		break;

	case NOISE_3DS_CHUNKID_FACES_MATERIAL:
		ParseFacesAndMatName();//level 6
		break;

	case NOISE_3DS_CHUNKID_MAPPING_COORDINATES_LIST:
		ParseTextureCoordinate();//level5
		break;

	case NOISE_3DS_CHUNKID_MATERIAL_BLOCK:
		ParseMaterialBlock();//level 3
		break;

	case NOISE_3DS_CHUNKID_MATERIAL_NAME:
		ParseMaterialName();//level 4
		break;

	case NOISE_3DS_CHUNKID_AMBIENT_COLOR:
		ParseAmbientColor();//level 4
		break;

	case NOISE_3DS_CHUNKID_DIFFUSE_COLOR:
		ParseDiffuseColor();//level 4
		break;

	case NOISE_3DS_CHUNKID_SPECULAR_COLOR:
		ParseSpecularColor();//level 4
		break;

	case NOISE_3DS_CHUNKID_TEXTURE_MAP:
		ParseDiffuseMap();//level 4
		break;

	case NOISE_3DS_CHUNKID_BUMP_MAP:
		ParseBumpMap();//level 4
		break;

	case NOISE_3DS_CHUNKID_SPECULAR_MAP:
		ParseSpecularMap();//level 4
		break;

		/*case NOISE_3DS_CHUNKID_REFLECTION_MAP:
		ParseReflectionMap();//4
		break;*/

	default:
		//not interested chunks , skip
		SkipCurrentChunk();
		break;
	}

}

//orderly read a string from file at current position
void IFileIO_3DS::ReadStringFromFileA(std::string & outString)
{
	do
	{
		char tmpChar = 0;
		BINARY_READ(tmpChar);

		if (tmpChar)
		{
			outString.push_back(tmpChar);
		}
		else
		{
			break;
		}
	} while (true);
}

//Read String(Wide Char Version)
void	IFileIO_3DS::ReadStringFromFileW(std::wstring& outString)
{
	do
	{
		char tmpChar = 0;
		BINARY_READ(tmpChar);

		if (tmpChar)
		{
			outString.push_back(tmpChar);
		}
		else
		{
			break;
		}
	} while (true);
}

//integrate several meshes into one big mesh
/*void IntegratePartialMeshIntoOnePiece(N_Load3ds_MeshObject & outCombinedMesh)
{
	N_Load3ds_MeshObject tmpMesh;

	//sum up to calculate element count, then reserve memory
	UINT totalIndicesCount = 0;
	UINT totalVerticesCount = 0;
	UINT totalSubsetCount = 0;
	for (auto& partialMesh : mMeshObjList)
	{
		totalIndicesCount += partialMesh.indicesList.size();
		totalVerticesCount += partialMesh.verticesList.size();
		totalSubsetCount += partialMesh.subsetList.size();
	}
	tmpMesh.indicesList.reserve(totalIndicesCount);
	tmpMesh.subsetList.reserve(totalSubsetCount);
	tmpMesh.texcoordList.reserve(totalVerticesCount);
	tmpMesh.verticesList.reserve(totalVerticesCount);


	//start to compute OFFSET and combine into one mesh
	for (auto& partialMesh : mMeshObjList)
	{
		UINT indexOffset = tmpMesh.verticesList.size();
		UINT primitiveIndexOffset = 0;

		if (tmpMesh.subsetList.size() != 0)
			primitiveIndexOffset = tmpMesh.subsetList.back().startPrimitiveID + tmpMesh.subsetList.back().primitiveCount;
	

		//INDICES LIST
		for (auto idx : partialMesh.indicesList)
		{
			tmpMesh.indicesList.push_back(idx + indexOffset);
		}

		//SUBSET LIST
		for (auto subset : partialMesh.subsetList)
		{
			subset.startPrimitiveID += primitiveIndexOffset;
			tmpMesh.subsetList.push_back(subset);
		}

		//VERTICES LIST
		tmpMesh.verticesList.insert(tmpMesh.verticesList.end(), partialMesh.verticesList.begin(), partialMesh.verticesList.end());

		//TEXCOORD LIST
		tmpMesh.texcoordList.insert(tmpMesh.texcoordList.end(), partialMesh.texcoordList.begin(), partialMesh.texcoordList.end());
	}

	//when material is not valid, a default subset should also be generated.
	//otherwise, the vertices wouldn't even be drawn ('Cos one subset for one draw call)
	if (tmpMesh.subsetList.size() == 0)
	{
		N_MeshSubsetInfo tmpSubsetInfo;
		tmpSubsetInfo.matName = NOISE_MACRO_DEFAULT_MATERIAL_NAME;
		tmpSubsetInfo.primitiveCount = tmpMesh.subsetList.size();
		tmpSubsetInfo.startPrimitiveID = 0;
		tmpMesh.subsetList.push_back(tmpSubsetInfo);
	}

	//swap vertices rotation order

	//clockwise or counterClockwise
	for (UINT i = 0;i < tmpMesh.indicesList.size();i += 3)
	{
		std::swap(tmpMesh.indicesList[i + 1], tmpMesh.indicesList[i + 2]);
	}

	outCombinedMesh = std::move(tmpMesh);
}
*/

//father: xxx color chunk
//child:none
void IFileIO_3DS::ReadAndParseColorChunk(NVECTOR3 & outColor)
{
	//please refer to project "Assimp" for more chunk information

	uint16_t chunkID = 0;
	uint32_t chunkLength = 0;
	BINARY_READ(chunkID);
	BINARY_READ(chunkLength);

	switch (chunkID)
	{
	case NOISE_3DS_CHUNKID_RGBF://float
	{
		BINARY_READ(outColor.x);
		BINARY_READ(outColor.y);
		BINARY_READ(outColor.z);
	}
	break;

	case NOISE_3DS_CHUNKID_RGBB://byte
	{
		uint8_t colorByte = 0;
		BINARY_READ(colorByte);
		outColor.x = float(colorByte) / 255.0f;
		BINARY_READ(colorByte);
		outColor.y = float(colorByte) / 255.0f;
		BINARY_READ(colorByte);
		outColor.z = float(colorByte) / 255.0f;
	}
	break;

	case NOISE_3DS_CHUNKID_PERCENTF://grey scale??
	{
		float colorPercentF = 0.0f;
		BINARY_READ(colorPercentF);
		outColor.x = outColor.y = outColor.z = colorPercentF;
	}
	break;

	case NOISE_3DS_CHUNKID_PERCENTW: //grey scale??
	{
		uint8_t colorByte = 0;
		BINARY_READ(colorByte);
		outColor.x = outColor.y = outColor.z = float(colorByte) / 255.0f;
	}
	break;

	default:
		//skip this color chunk
		fileIn.seekg(chunkLength - c_chunkHeadByteLength, std::ios::cur);
		return;
	}
}

//father: xxx map chunk
//child:none
void IFileIO_3DS::ReadAndParseMapChunk(std::string& outGeneratedTextureName)
{
	//please refer to project "Assimp" for more chunk information

	uint16_t chunkID = 0;
	uint32_t chunkLength = 0;
	BINARY_READ(chunkID);
	BINARY_READ(chunkLength);

	/*case Discreet3DS::CHUNK_PERCENTF:
	case Discreet3DS::CHUNK_PERCENTW:
	case Discreet3DS::CHUNK_MAT_MAP_USCALE:
	case Discreet3DS::CHUNK_MAT_MAP_VSCALE:
	case Discreet3DS::CHUNK_MAT_MAP_UOFFSET:
	case Discreet3DS::CHUNK_MAT_MAP_VOFFSET:
	case Discreet3DS::CHUNK_MAT_MAP_ANG:
	case Discreet3DS::CHUNK_MAT_MAP_TILING:*/

	switch (chunkID)
	{
	case NOISE_3DS_CHUNKID_MAPPING_FILENAME:
	{
		std::string texFilePath;
		//back() can retrieve the current processing material
		std::string texName=c_defaultTexNamePrefix + std::to_string(mTextureMapIndex);
		++mTextureMapIndex;

		//...Read File Path From File
		ReadStringFromFileA(texFilePath);

		//texName-filePath pair,so that texture creation could be done
		m_pTexName2FilePathPairList->insert(std::make_pair(texName,texFilePath.c_str()));

		outGeneratedTextureName = std::move(texName);
	}
		break;

	default:
		//skip this color chunk
		fileIn.seekg(chunkLength - c_chunkHeadByteLength, std::ios::cur);
		break;
	}

}

//to generate unique mat Name
std::string IFileIO_3DS::DecorateMatName(std::string name) 
{ 
	return c_defaultMatNamePrefix + name + std::to_string(mMeshIndex); 
};


//*****************************************************************

//*************************************************
//father : none
//child : 3D Editor Chunk
void IFileIO_3DS::ParseMainChunk()
{
	//no data in this chunk ,only need to go into sub-chunks
};

//*************************************************
//father : Main3D
//child :	3D EDITOR CHUNK 0x3D3D
//			MATERIAL BLOCK 0xAFFF
void IFileIO_3DS::Parse3DEditorChunk()
{
	//no data in this chunk ,only need to go into sub-chunks
};

//*************************************************
//father : 3D Editor Chunk
//child : TriangularMesh,  (Light , Camera)
void IFileIO_3DS::ParseObjectBlock()
{
	//data: object name (could be mesh,light,camera)
	ReadStringFromFileA(static_objectBlockParsedName);
	//SkipCurrentChunk();
};

//*************************************************
//father : Object Block
//child : VerticesChunk, FaceDescription(Indices) , TextureCoordinate
void IFileIO_3DS::ParseTriangularMeshChunk()
{
	//create a new mesh
	N_Load3ds_MeshObject tmpMesh;
	tmpMesh.meshName = "3ds_Mesh" +std::to_string(mMeshIndex) +"_" + static_objectBlockParsedName;
	m_pMeshObjList->push_back(tmpMesh);

	//used to generate unique name
	++mMeshIndex;

}

//*************************************************
//father::Triangular Mesh Chunk
//child: none
void IFileIO_3DS::ParseVerticesChunk()
{
	//data: Vertices List:
	uint16_t verticesCount = 0;//unsigned short
	BINARY_READ(verticesCount);

	auto& vertexList = m_pMeshObjList->back().verticesList;
	vertexList.reserve(vertexList.size()+verticesCount);
	for (UINT i = 0;i < verticesCount;i++)
	{
		NVECTOR3 tmpVertex(0, 0, 0);
		BINARY_READ(tmpVertex.x);
		BINARY_READ(tmpVertex.z);
		BINARY_READ(tmpVertex.y);
		vertexList.push_back(tmpVertex);
	}

}

//*************************************************
//father::Triangular Mesh Chunk
//child: FACES MATERIAL 0x4130
void IFileIO_3DS::ParseFaceDescAndIndices()
{
	//data:faces count + n * (3*indices+faceDesc)
	uint16_t faceCount = 0;
	BINARY_READ(faceCount);


	auto& indicesList = m_pMeshObjList->back().indicesList;
	indicesList.reserve(indicesList.size()+3*faceCount);

	//find the biggest index in this indices block to compute index offset for next block

	//I want to combine various object into one 
	for (UINT i = 0;i < faceCount;i++)
	{
		//1 face for 3 indices
		uint16_t tmpIndex = 0;

		for (UINT i = 0;i < 3;i++)
		{
			BINARY_READ(tmpIndex);
			indicesList.push_back(tmpIndex);
		}

		uint16_t faceFlag = 0;
		//face option,side visibility , etc.
		BINARY_READ(faceFlag);
	}

}

//*************************************************
//father::FACES DESCRIPTION 0x4120
//child: none
void IFileIO_3DS::ParseFacesAndMatName()
{
	//i guess one indices(faces) block can
	//possess several this kind of FaceMaterial Block ...

	//data : 
	//1. material name (end with '/0')
	//2. faces count that linked to current material (ushort) 
	//3. FACE indices

	std::string matName("");

	std::vector<N_MeshSubsetInfo> currMatSubsetList;//faces can be 
	std::vector<uint16_t> indicesList;

	//material name
	ReadStringFromFileA(matName);

	//faces that link to this material
	uint16_t faceCount = 0;
	BINARY_READ(faceCount);

	//read FACE indices block
	indicesList.reserve(faceCount);
	for (UINT i = 0;i < faceCount;i++)
	{
		uint16_t tmpFaceIndex = 0;
		BINARY_READ(tmpFaceIndex);

		//(vertex_index+1)/3 yields the primitive_index offset
		//LastIndexOffset stores the idx offset before current chunk was read.
		//while CurrIndexOffset means after.
		indicesList.push_back(tmpFaceIndex);
	}

	if (indicesList.size() == 0)
	{
		ERROR_MSG( "face Material Info : no faces attached to this material!!");
		return;
	}

	//generate subsets list (for current Material)
	N_MeshSubsetInfo newSubset;
	newSubset.matName = DecorateMatName(matName);
	newSubset.primitiveCount = 0;
	newSubset.startPrimitiveID = indicesList.at(0);
	currMatSubsetList.push_back(newSubset);
	for (UINT i = 0;i < indicesList.size();i++)
	{
		UINT startTriangleID = currMatSubsetList.back().startPrimitiveID;
		UINT endTriangleID = startTriangleID + currMatSubsetList.back().primitiveCount - 1;
		//region growing (extend subset region)
		//if next index is adjacent to current region ,then grow
		if (endTriangleID + 1 == indicesList[i])
		{
			currMatSubsetList.back().primitiveCount += 1;
		}
		else
		{
			//this index isn't adjacent to the growing region
			//so create a new region
			newSubset.matName = matName;
			newSubset.primitiveCount = 1;
			newSubset.startPrimitiveID = indicesList.at(i);
			currMatSubsetList.push_back(newSubset);
		}
	}

	//add to global subset list
	auto& subsetList = m_pMeshObjList->back().subsetList;
	for (auto subset : currMatSubsetList)
	{
		subsetList.push_back(subset);
	}

}

//*************************************************
//father::Triangular Mesh
//child: smoothing group
void IFileIO_3DS::ParseTextureCoordinate()
{
	//data: 
	//1.texcoord count (2byte)
	//2.  n * (u + v) ,2n float
	uint16_t texcoordCount = 0;
	BINARY_READ(texcoordCount);

	auto& texcoordList = m_pMeshObjList->back().texcoordList;
	for (UINT i = 0;i < texcoordCount;i++)
	{
		NVECTOR2 tmpTexCoord(0, 0);
		BINARY_READ(tmpTexCoord.x);
		BINARY_READ(tmpTexCoord.y);
		texcoordList.push_back(tmpTexCoord);
	}

}

//*************************************************
//father:3D Editor Chunk 
//child: MATERIAL NAME 0xA000
//         AMBIENT COLOR 0xA010
//			DIFFUSE COLOR 0xA020
//			SPECULAR COLOR 0xA030
//			TEXTURE MAP 1 0xA200
//			BUMP MAP 0xA230
//			REFLECTION MAP 0xA220
void IFileIO_3DS::ParseMaterialBlock()
{
	//Data:None

	//but this identifier signify creation a new material
	N_MaterialDesc tmpMat;
	m_pMaterialList->push_back(tmpMat);
}

//*************************************************
//father:Material block
//child:  none
void IFileIO_3DS::ParseMaterialName()
{
	//data: material name
	std::string orginalMatName;
	ReadStringFromFileA(orginalMatName);
	m_pMatNameList->push_back(DecorateMatName(orginalMatName));
}

//*************************************************
//father:Material Block
//child	: sub color chunk
void IFileIO_3DS::ParseAmbientColor()
{
	//data:sub chunks

	uint64_t filePos = 0;
	do
	{
		ReadAndParseColorChunk(m_pMaterialList->back().ambientColor);
		filePos = fileIn.tellg();
	} while (filePos<static_currentChunkFileEndPos);
}

//father:Material Block
//child	: sub color chunk
void IFileIO_3DS::ParseDiffuseColor()
{
	uint64_t filePos = 0;
	do
	{
		ReadAndParseColorChunk(m_pMaterialList->back().diffuseColor);
		filePos = fileIn.tellg();
	} while (filePos<static_currentChunkFileEndPos);
}

//father:Material Block
//child	: sub color chunk
void IFileIO_3DS::ParseSpecularColor()
{
	uint64_t filePos = 0;
	do
	{
		ReadAndParseColorChunk(m_pMaterialList->back().specularColor);
		filePos = fileIn.tellg();
	} while (filePos<static_currentChunkFileEndPos);
}

//father:Material Block
//child	: Mapping File Path  (Map option) 
void IFileIO_3DS::ParseDiffuseMap()
{
	//data:sub chunks
	uint64_t filePos = 0;
	do
	{
		std::string texName;
		//mapName-mapFilePath pair will be added in this function
		ReadAndParseMapChunk(m_pMaterialList->back().diffuseMapName);
		filePos = fileIn.tellg();
	} while (filePos<static_currentChunkFileEndPos);
}

//father:Material Block
//child	: Mapping File Path  (Map option) 
void IFileIO_3DS::ParseBumpMap()
{
	//data:sub chunks
	uint64_t filePos = 0;
	do
	{
		//mapName-mapFilePath pair will be added in this function
		ReadAndParseMapChunk(m_pMaterialList->back().normalMapName);
		filePos = fileIn.tellg();
	} while (filePos<static_currentChunkFileEndPos);
}

//father:Material Block
//child	: Mapping File Path  (Map option) 
void IFileIO_3DS::ParseSpecularMap()
{
	//data:sub chunks
	uint64_t filePos = 0;
	do
	{
		//mapName-mapFilePath pair will be added in this function
		ReadAndParseMapChunk(m_pMaterialList->back().specularMapName);
		filePos = fileIn.tellg();
	} while (filePos<static_currentChunkFileEndPos);
}

#endif