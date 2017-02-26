
/***********************************************************************

							class£ºModel Processor

				Desc: all kind of operation on loaded model

***********************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

IModelProcessor::IModelProcessor()
{

}

IModelProcessor::~IModelProcessor()
{

}

void IModelProcessor::WeldVertices(IMesh * pTargetMesh)
{
	//get ref to vertex buffer
	std::vector<N_DefaultVertex>&  vb = *(pTargetMesh->m_pVB_Mem);

	// -----Vertices Welding-----
	// position-duplicated vertices are assumption.
	//hash-map will be used to remove duplicated vertices in terms of POSITION,
	//then attribute will be combined according to several overlapped vertices

	//hash method of hashmap
	struct HashVertex
	{
		float operator()(const N_DefaultVertex& v) const
		{
			//3 factors are arbitrarily 
			const float factor1 = 0.31579f;
			const float factor2 = 0.2468f;
			const float factor3 = 1.0f - factor1 - factor2;
			return v.Pos.x * factor1 + v.Pos.y * factor2 + v.Pos.z * factor3;
		}
	};

	//Key_eq method of hashmap
	struct EqVertex
	{
		bool operator()(const N_DefaultVertex& v1, const N_DefaultVertex& v2) const
		{
			if (v1.Pos.x == v2.Pos.x && v1.Pos.y == v2.Pos.y && v1.Pos.z == v2.Pos.z)
				return true;
			else 
				return false;
		}
	};

	//hash-map, <vector , index>  pair
	std::unordered_map<N_DefaultVertex, UINT, HashVertex,EqVertex> point2IndexHashMap;
	std::vector<N_DefaultVertex> uniqueVertexList;//it's been said that iterate an unordered_map isn't efficient
	std::vector<UINT> indicesList;
	std::vector<int> vertexRepeatCountList;

	//1, traverse vertex buffer and insert Vertex into hash-map to remove duplication
	for (UINT i = 0; i < vb.size(); ++i)
	{
		//index is re-arranged
		UINT newIndex = point2IndexHashMap.size();
		point2IndexHashMap.insert(std::make_pair(vb.at(i), newIndex));
	}

	//Default_Vertex's memory is zero-ed
	uniqueVertexList.resize(point2IndexHashMap.size(), N_DefaultVertex());
	vertexRepeatCountList.resize(point2IndexHashMap.size(), 0);

	//2, traverse (original) vertex buffer again to generate indices,
	//new indices is derived from unique-vertex-list
	for (auto& v: vb)
	{
		//new index now fit unique vertex list
		UINT newIndex = point2IndexHashMap.at(v);
		indicesList.push_back(newIndex);

		//because all vertex attribute are cleared to zero initially,
		//attributes can be sumed up, and NORMALIZE later
		uniqueVertexList.at(newIndex) += v;
		vertexRepeatCountList.at(newIndex) += 1;
	}

	//get the mean value of vertex attribute
	for (UINT i=0;i<uniqueVertexList.size();++i)
	{
		//all attribute multiply the same factor
		uniqueVertexList.at(i) *= (1.0f / float(vertexRepeatCountList.at(i)));
	}

	//remember!!! update to GPU
	pTargetMesh->mFunction_UpdateDataToVideoMem(uniqueVertexList, indicesList);

}
