
/***********************************************************************

							class£ºModel Processor

				Desc: all kind of operation on processing model

***********************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

float IModelProcessor::static_PositionEqualThreshold = 0.01f;

IModelProcessor::IModelProcessor()
{

}

IModelProcessor::~IModelProcessor()
{

}

void IModelProcessor::WeldVertices(IMesh * pTargetMesh)
{
	//get ref to vertex buffer
	const std::vector<N_DefaultVertex>&  vb = *(pTargetMesh->GetVertexBuffer());
	const std::vector<UINT>& ib = *(pTargetMesh->GetIndexBuffer());

	// -----Vertices Welding-----
	// position-duplicated vertices are assumption.
	//hash-map will be used to remove duplicated vertices in terms of POSITION,
	//then attribute will be combined according to several overlapped vertices

	//hash method of hashmap
	struct HashVertex
	{
		inline float operator()(const N_DefaultVertex& v) const
		{
			//3 factors are arbitrarily 
			constexpr float factor1 = 0.3568f;
			constexpr float factor2 = 0.2479f;
			constexpr float factor3 = 1.0f - factor1 - factor2;
			const NVECTOR3& pos = v.Pos;
			//return pos.x * factor1 + pos.y * factor2 + pos.z * factor3;
			return pos.x * factor1 + pos.y * factor2 + pos.z * factor3;
		}
	};


	//key_eq: position must be precisely the same
	struct EqVertex
	{
		inline bool operator()(const N_DefaultVertex& v1, const N_DefaultVertex& v2) const
		{
			const NVECTOR3& pos1 = v1.Pos;
			const NVECTOR3& pos2 = v2.Pos;
			return (pos1.x == pos2.x && pos1.y == pos2.y && pos1.z == pos2.z);
		}
	};


	//hash-map, <vector , index>  pair
	std::unordered_map<N_DefaultVertex, UINT, HashVertex, EqVertex> point2IndexHashMap;
	std::vector<N_DefaultVertex> uniqueVertexList;//it's been said that iterate an unordered_map isn't efficient
	std::vector<UINT> indicesList;
	std::vector<int> vertexRepeatCountList;

	//1, traverse vertex buffer and insert Vertex into hash-map to remove duplication
	//this is the BOTTLE-NECK of PERFORMANCE
	for (UINT i = 0; i < vb.size(); ++i)
	{
		//index is re-arranged
		UINT newIndex = point2IndexHashMap.size();
		point2IndexHashMap.insert(std::make_pair(vb.at(i), newIndex));

	}

	//Default_Vertex's memory is zero-ed
	uniqueVertexList.resize(point2IndexHashMap.size(), N_DefaultVertex());
	vertexRepeatCountList.resize(point2IndexHashMap.size(), 0);

	//2, traverse (original) index buffer to generate new indices,
	//new indices is derived from unique-vertex-list
	for (auto& index: ib)
	{
		//new index now fit unique vertex list
		const N_DefaultVertex& v = vb.at(index);
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
		N_DefaultVertex& v = uniqueVertexList.at(i);
		v *= (1.0f / float(vertexRepeatCountList.at(i)));
		D3DXVec3Normalize(&v.Normal,&v.Normal);
		D3DXVec3Normalize(&v.Tangent, &v.Tangent);
	}

	//remember!!! update to GPU
	pTargetMesh->mFunction_UpdateDataToVideoMem(uniqueVertexList, indicesList);

}

void IModelProcessor::WeldVertices(IMesh * pTargetMesh, float PositionEqualThreshold)
{
	//get ref to vertex buffer
	const std::vector<N_DefaultVertex>&  vb = *(pTargetMesh->GetVertexBuffer());
	const std::vector<UINT>& ib = *(pTargetMesh->GetIndexBuffer());

	// -----Vertices Welding-----
	// position-duplicated vertices are assumption.
	//hash-map will be used to remove duplicated vertices in terms of POSITION,
	//then attribute will be combined according to several overlapped vertices

	//hash method of hashmap
	struct HashVertex
	{
		inline float operator()(const N_DefaultVertex& v) const
		{
			//3 factors are arbitrarily 
			constexpr float factor1 = 0.33333f;//0.3568f;
			constexpr float factor2 = 0.33333f;//0.2479f;
			constexpr float factor3 = 1.0f - factor1 - factor2;
			const NVECTOR3& pos = v.Pos;
			//return pos.x * factor1 + pos.y * factor2 + pos.z * factor3;
			return pos.x * factor1 + pos.y * factor2 + pos.z * factor3;
		}
	};


	//Key_eq: position might be not precisely at the same position
	static_PositionEqualThreshold = PositionEqualThreshold;
	struct EqVertex2
	{
		inline bool operator()(const N_DefaultVertex& v1, const N_DefaultVertex& v2) const
		{
			const NVECTOR3& pos1 = v1.Pos;
			const NVECTOR3& pos2 = v2.Pos;
			//return (pos1.x == pos2.x && pos1.y == pos2.y && pos1.z == pos2.z);
			return (abs(pos1.x - pos2.x) + abs(pos1.y - pos2.y) + abs(pos1.z - pos2.z) <= static_PositionEqualThreshold);
		}
	};


	//hash-map, <vector , index>  pair
	std::unordered_map<N_DefaultVertex, UINT, HashVertex, EqVertex2> point2IndexHashTable;
	std::vector<N_DefaultVertex> uniqueVertexList;//it's been said that iterate an unordered_map isn't efficient
	std::vector<UINT> indicesList;
	std::vector<int> vertexRepeatCountList;

	//1, traverse vertex buffer and insert Vertex into hash-map to remove duplication
	//this is the BOTTLE-NECK of PERFORMANCE
	for (UINT i = 0; i < vb.size(); ++i)
	{
		//index is re-arranged
		UINT newIndex = point2IndexHashTable.size();
		point2IndexHashTable.insert(std::make_pair(vb.at(i), newIndex));

	}

	//Default_Vertex's memory is zero-ed
	uniqueVertexList.resize(point2IndexHashTable.size(), N_DefaultVertex());
	vertexRepeatCountList.resize(point2IndexHashTable.size(), 0);

	//2, traverse (original) index buffer to generate new indices,
	//new indices is derived from unique-vertex-list
	for (auto& index : ib)
	{
		//new index now fit unique vertex list
		const N_DefaultVertex& v = vb.at(index);
		UINT newIndex = point2IndexHashTable.at(v);
		indicesList.push_back(newIndex);

		//because all vertex attribute are cleared to zero initially,
		//attributes can be sumed up, and NORMALIZE later
		uniqueVertexList.at(newIndex) += v;
		vertexRepeatCountList.at(newIndex) += 1;
	}

	//get the mean value of vertex attribute
	for (UINT i = 0; i<uniqueVertexList.size(); ++i)
	{
		//all attribute multiply the same factor
		N_DefaultVertex& v = uniqueVertexList.at(i);
		v *= (1.0f / float(vertexRepeatCountList.at(i)));
		D3DXVec3Normalize(&v.Normal, &v.Normal);
		D3DXVec3Normalize(&v.Tangent, &v.Tangent);
	}

	//remember!!! update to GPU
	pTargetMesh->mFunction_UpdateDataToVideoMem(uniqueVertexList, indicesList);
}

void IModelProcessor::MeshSimplify(IMesh * pTargetMesh, float PositionEqualThreshold, float visualImportanceWeightThreshold)
{
	//In Mesh Simplification based on vertex clustering , feature preserving is important
	//so some VISUALLY important vertices (maybe some vertex with large curvature)
	//are added to the hash map in the very first place,
	//in case that those vertices are eliminated (absorbed)
	//by vertex clustering

	//get ref to vertex buffer
	const std::vector<N_DefaultVertex>&  vb = *(pTargetMesh->GetVertexBuffer());
	const std::vector<UINT>& ib = *(pTargetMesh->GetIndexBuffer());

	//hash method of hashmap
	struct HashVertex
	{
		//inline float operator()(const N_DefaultVertex& v) const
		inline int operator()(const N_DefaultVertex& v) const
		{
			//3 factors are arbitrarily 
			constexpr float factor1 = 0.33333f;//0.3568f;
			constexpr float factor2 = 0.33333f;//0.2479f;
			constexpr float factor3 = 1.0f - factor1 - factor2;
			const NVECTOR3& pos = v.Pos;
			//return pos.x * factor1 + pos.y * factor2 + pos.z * factor3;
			return int(pos.x * factor1 + pos.y * factor2 + pos.z * factor3);
		}
	};

	//Key_eq: position might be not precisely at the same position
	static_PositionEqualThreshold = PositionEqualThreshold;
	struct EqVertex2
	{
		inline bool operator()(const N_DefaultVertex& v1, const N_DefaultVertex& v2) const
		{
			const NVECTOR3& pos1 = v1.Pos;
			const NVECTOR3& pos2 = v2.Pos;
			//return (pos1.x == pos2.x && pos1.y == pos2.y && pos1.z == pos2.z);
			return (abs(pos1.x - pos2.x) + abs(pos1.y - pos2.y) + abs(pos1.z - pos2.z) <= static_PositionEqualThreshold);
		}
	};


	//hash-map, <vector , index>  pair
	std::unordered_map<N_DefaultVertex, UINT, HashVertex, EqVertex2> point2IndexHashTable(10);
	std::vector<N_DefaultVertex> uniqueVertexList;//it's been said that iterate an unordered_map isn't efficient
	std::vector<UINT> indicesList;
	std::vector<int> vertexRepeatCountList;

	//1, traverse vertex buffer and insert Vertex into hash-map to remove duplication
	//this is the BOTTLE-NECK of PERFORMANCE
	for (UINT i = 0; i < vb.size(); ++i)
	{
		//index is re-arranged
		UINT newIndex = point2IndexHashTable.size();
		point2IndexHashTable.insert(std::make_pair(vb.at(i), newIndex));

	}

	//Default_Vertex's memory is zero-ed
	uniqueVertexList.resize(point2IndexHashTable.size(), N_DefaultVertex());
	vertexRepeatCountList.resize(point2IndexHashTable.size(), 0);

	//2, traverse (original) index buffer to generate new indices,
	//new indices is derived from unique-vertex-list
	for (auto& index : ib)
	{
		//new index now fit unique vertex list
		const N_DefaultVertex& v = vb.at(index);
		UINT newIndex = point2IndexHashTable.at(v);
		indicesList.push_back(newIndex);

		//because all vertex attribute are cleared to zero initially,
		//attributes can be sumed up, and NORMALIZE later
		uniqueVertexList.at(newIndex) += v;
		vertexRepeatCountList.at(newIndex) += 1;
	}

	//get the mean value of vertex attribute
	for (UINT i = 0; i<uniqueVertexList.size(); ++i)
	{
		//all attribute multiply the same factor
		N_DefaultVertex& v = uniqueVertexList.at(i);
		v *= (1.0f / float(vertexRepeatCountList.at(i)));
		D3DXVec3Normalize(&v.Normal, &v.Normal);
		D3DXVec3Normalize(&v.Tangent, &v.Tangent);
	}

	//remember!!! update to GPU
	pTargetMesh->mFunction_UpdateDataToVideoMem(uniqueVertexList, indicesList);
}

void IModelProcessor::Smooth_Laplacian(IMesh * pTargetMesh)
{
	//First compute ADJACENT list (but whether to store adjacent need to be discussed)
	N_AdjacentList adjList;

	//get ref to vertex buffer
	std::vector<N_DefaultVertex>&  vb = (pTargetMesh->mVB_Mem);
	const std::vector<UINT>& ib = *(pTargetMesh->GetIndexBuffer());
	std::vector<NVECTOR3> tmpPosList;

	//one "adjacent index list" for each vertex
	adjList.resize(vb.size());

	//.........
	for (UINT i=0;i<ib.size()-2;i+=3)
	{
		UINT idx1 = ib.at(i);
		UINT idx2 = ib.at(i + 1);
		UINT idx3 = ib.at(i + 2);

		//-----find duplicate index in adjacentIndexList of current vertex
		bool findDuplicate1 = false;
		bool findDuplicate2 = false;
		for (auto& index : adjList.at(idx1))
		{
			if (index == idx2) {findDuplicate1 = true; break;}
			if (index == idx3) { findDuplicate2 = true; break; }
		}
		if (!findDuplicate1)	adjList.at(idx1).push_back(idx2); 
		if (!findDuplicate2)	adjList.at(idx1).push_back(idx3);


		//-----find another duplication
		findDuplicate1 = false;
		findDuplicate2 = false;
		for (auto& index : adjList.at(idx2))
		{
			if (index == idx1) { findDuplicate1 = true; break; }
			if (index == idx3) { findDuplicate2 = true; break; }
		}
		if (!findDuplicate1)	adjList.at(idx2).push_back(idx1);
		if (!findDuplicate2)	adjList.at(idx2).push_back(idx3);


		//-----find another duplication
		findDuplicate1 = false;
		findDuplicate2 = false;
		for (auto& index : adjList.at(idx3))
		{
			if (index == idx1) { findDuplicate1 = true; break; }
			if (index == idx2) { findDuplicate2 = true; break; }
		}
		if (!findDuplicate1)	adjList.at(idx3).push_back(idx1);
		if (!findDuplicate2)	adjList.at(idx3).push_back(idx2);

	}

	//done for generation of adjacent information
	//now apply "Laplacian Smooth Operator"
	//that is to calculate the average position of all adjacent vertices
	for (UINT i = 0; i < adjList.size(); ++i)
	{
		auto& indexList = adjList.at(i);

		NVECTOR3 averagePos(0,0,0);

		//traverse adjacent vertices and sum up those vectors
		for (auto idx : indexList)
		{
			const NVECTOR3& adjVertex = vb.at(idx).Pos;
			averagePos += adjVertex;
		}

		//divided by adjacent vertices count
		averagePos /= float(indexList.size());

		//set vertices
		tmpPosList.push_back(averagePos);
	}

	for (UINT i = 0; i < tmpPosList.size(); ++i)vb.at(i).Pos = tmpPosList.at(i);

	//update to gpu
	pTargetMesh->mFunction_UpdateDataToVideoMem();

}
