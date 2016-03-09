/***********************************************************************

				Description: OBJ File Operation

************************************************************************/

#include "Noise3D.h"


//in OBJ file ,vertex info is composed of indices
struct N_LoadOBJ_vertexInfoIndex
{
	N_LoadOBJ_vertexInfoIndex()
	{
		vertexID = texcoordID = vertexNormalID = 0;
	};

	N_LoadOBJ_vertexInfoIndex(int vID, int texcID, int vnID)
	{
		vertexID = vID;
		texcoordID = texcID;
		vertexNormalID = vnID;
	}

	inline BOOL operator==(N_LoadOBJ_vertexInfoIndex const& v)const
	{
		if (vertexID == v.vertexID && texcoordID == v.texcoordID && vertexNormalID == v.vertexNormalID)
		{
			return TRUE;
		}
		return FALSE;
	}

	UINT vertexID;
	UINT texcoordID;
	UINT vertexNormalID;
};

/*******************************************************************

										INTERFACE

*********************************************************************/
BOOL NoiseFileManager::ImportFile_OBJ(NFilePath pFilePath, std::vector<N_DefaultVertex>& refVertexBuffer, std::vector<UINT>& refIndexBuffer)
{
	std::fstream fileIn(pFilePath);
	if (!fileIn.good())
	{
		DEBUG_MSG1("Import OBJ : Open File failed!!");
		return FALSE;
	}

	std::vector<NVECTOR3> pointList;//xyz buffer
	std::vector<NVECTOR2> texcoordList;//texcoord buffer
	std::vector<NVECTOR3> VNormalList;//vertex normal buffer
	std::vector<N_LoadOBJ_vertexInfoIndex> vertexInfoList;//indices combination

														  //newly input string from file
	std::string currString;

	//,...............
	while (!fileIn.eof())
	{
		fileIn >> currString;

		//3d vertex : "v 1.0000000 0.52524242 5.12312345"
		if (currString == "v")
		{
			NVECTOR3 currPoint(0, 0, 0);
			fileIn >> currPoint.x >> currPoint.y >> currPoint.z;
			pointList.push_back(currPoint);
		}

		//vertex normal "vn 1.0000000 0.52524242 5.12312345"
		if (currString == "vn")
		{
			NVECTOR3 currNormal(0, 0, 0);
			fileIn >> currNormal.x >> currNormal.y >> currNormal.z;
			VNormalList.push_back(currNormal);
		}

		//texture coordinate "vt 1.0000000 0.0000000"
		if (currString == "vt")
		{
			NVECTOR2 currTexCoord(0, 0);
			fileIn >> currTexCoord.x >> currTexCoord.y;
			texcoordList.push_back(currTexCoord);
		}

		//face : if this face is trangles(OBJ also support quads or curves etc..)
		//, the combination will be 
		//  vertex index  / texcoord index  /  vnormal index (for each vertex)
		// like "1/3/4" "5/6/7"
		if (currString == "f")
		{
			for (UINT i = 0;i < 3;++i)
			{
				//static UINT currUniqueVertexCount = 0;
				N_LoadOBJ_vertexInfoIndex currVertex = { 0,0,0 };
				std::stringstream tmpSStream;
				std::string tmpString;

				//then a "1/2/3" "4/1/3" like string will be input
				fileIn >> tmpString;

				//replace "/" with SPACE ,so STD::STRINGSTREAM will automatically format
				//the string into 3 separate part
				for (auto& c : tmpString) { if (c == '/')c = ' '; }

				tmpSStream << tmpString;
				tmpSStream >> currVertex.vertexID >> currVertex.texcoordID >> currVertex.vertexNormalID;
				--currVertex.texcoordID;
				--currVertex.vertexID;
				--currVertex.vertexNormalID;

				//this will be an n^2 searching....optimization will be needed
				//non-existed element will be created
				BOOL IsVertexExist = FALSE;
				UINT  existedVertexIndex = 0;
				for (UINT i = 0;i <vertexInfoList.size();i++)
				{
					//in DEBUG mode ,[] operator will be a big performance overhead
					if (vertexInfoList[i] == currVertex)
					{
						IsVertexExist = TRUE;
						existedVertexIndex = i;
						break;
					}
				}

				if (!IsVertexExist)
				{
					vertexInfoList.push_back(currVertex);
					//the newest vertex.....
					refIndexBuffer.push_back(vertexInfoList.size() - 1);
				}
				else
				{
					refIndexBuffer.push_back(existedVertexIndex);
				}
			}
		}
	}

	fileIn.close();

	// All interested data are acquired, now convert to VB/IB
	refVertexBuffer.resize(vertexInfoList.size());
	for (UINT i = 0;i < refVertexBuffer.size();++i)
	{
		N_DefaultVertex tmpVertex = {};

		//several indices which can retrieve vertex information
		N_LoadOBJ_vertexInfoIndex& indicesCombination = vertexInfoList.at(i);
		tmpVertex.Pos = pointList.at(indicesCombination.vertexID);
		tmpVertex.Normal = VNormalList.at(indicesCombination.vertexNormalID);
		tmpVertex.TexCoord = texcoordList.at(indicesCombination.texcoordID);
		tmpVertex.Color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		//tangent
		NVECTOR3 tmpVec(-tmpVertex.Normal.z, 0, tmpVertex.Normal.x);
		D3DXVec3Cross(&tmpVertex.Tangent, &tmpVertex.Normal, &tmpVec);
		D3DXVec3Normalize(&tmpVertex.Tangent, &tmpVertex.Tangent);

		//.......
		refVertexBuffer.at(i) = (tmpVertex);
	}

	return TRUE;
}
