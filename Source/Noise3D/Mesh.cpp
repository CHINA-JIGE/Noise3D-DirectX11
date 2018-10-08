
/***********************************************************************

										Mesh

***********************************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::D3D;

Mesh::Mesh():
	mBoundingBox({0,0,0},{0,0,0}),
	m_pVB_Gpu(nullptr),
	m_pIB_Gpu(nullptr)
{
	SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
};

Mesh::~Mesh()
{
	ReleaseCOM(m_pVB_Gpu);
	ReleaseCOM(m_pIB_Gpu);
}

void Mesh::ResetMaterialToDefault()
{
	SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
}

void Mesh::SetMaterial(N_UID matName)
{
	N_MeshSubsetInfo tmpSubset;
	tmpSubset.startPrimitiveID = 0;
	tmpSubset.primitiveCount = mIB_Mem.size()/3;//count of triangles
	tmpSubset.matName = matName;
	
	//because this SetMaterial aim to the entire mesh (all primitives) ,so
	//previously-defined material will be wiped,and set to this material
	mSubsetInfoList.clear();
	mSubsetInfoList.push_back(tmpSubset);
}

void Mesh::SetSubsetList(const std::vector<N_MeshSubsetInfo>& subsetList)
{
	mSubsetInfoList = subsetList;
}

void Mesh::GetSubsetList(std::vector<N_MeshSubsetInfo>& outRefSubsetList)
{
	outRefSubsetList = mSubsetInfoList;
}

UINT Mesh::GetIndexCount()
{
	return mIB_Mem.size();
}

UINT Mesh::GetTriangleCount()
{
	return mIB_Mem.size()/3;
}

void Mesh::GetVertex(UINT iIndex, N_DefaultVertex& outVertex)
{
	if (iIndex < mVB_Mem.size())
	{
		outVertex = mVB_Mem.at(iIndex);
	}
}

const std::vector<N_DefaultVertex>*		Mesh::GetVertexBuffer()const 
{
	return &mVB_Mem;
}

const std::vector<UINT>* Mesh::GetIndexBuffer() const
{
	return &mIB_Mem;
}

N_Box Mesh::ComputeBoundingBox()
{
	mFunction_ComputeBoundingBox();

	return mBoundingBox;
};

/***********************************************************************
								PRIVATE					                    
***********************************************************************/
//this function could be externally invoked by ModelLoader..etc
bool Mesh::mFunction_UpdateDataToVideoMem(const std::vector<N_DefaultVertex>& targetVB,const std::vector<UINT>& targetIB)
{
	//check if buffers have been created
	ReleaseCOM(m_pVB_Gpu);
	mVB_Mem.clear();
	ReleaseCOM(m_pIB_Gpu);
	mIB_Mem.clear();

	//this function could be externally invoked by ModelLoader..etc
	mVB_Mem =targetVB;
	mIB_Mem = targetIB;


#pragma region CreateGpuBuffers
	//Prepare to update to video memory, fill in SUBRESOURCE description structure
	D3D11_SUBRESOURCE_DATA tmpInitData_Vertex;
	ZeroMemory(&tmpInitData_Vertex, sizeof(tmpInitData_Vertex));
	tmpInitData_Vertex.pSysMem = &mVB_Mem.at(0);
	UINT vertexCount = mVB_Mem.size();

	D3D11_SUBRESOURCE_DATA tmpInitData_Index;
	ZeroMemory(&tmpInitData_Index, sizeof(tmpInitData_Index));
	tmpInitData_Index.pSysMem = &mIB_Mem.at(0);
	UINT indexCount = mIB_Mem.size();

	//------Create VERTEX BUFFER
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = sizeof(N_DefaultVertex)* vertexCount;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	//Create Buffers
	int hr = 0;
	hr = g_pd3dDevice11->CreateBuffer(&vbd, &tmpInitData_Vertex, &m_pVB_Gpu);
	HR_DEBUG(hr, "Mesh : Failed to create vertex buffer ! ");


	D3D11_BUFFER_DESC ibd;
	ibd.ByteWidth = sizeof(int) * indexCount;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	//Create Buffers
	hr = g_pd3dDevice11->CreateBuffer(&ibd, &tmpInitData_Index, &m_pIB_Gpu);
	HR_DEBUG(hr, "Mesh : Failed to create index buffer ! ");

#pragma endregion CreateGpuBuffers

	return true;
}

bool Mesh::mFunction_UpdateDataToVideoMem()
{
	ReleaseCOM(m_pVB_Gpu);
	ReleaseCOM(m_pIB_Gpu);

#pragma region CreateGpuBuffers
	//Prepare to update to video memory, fill in SUBRESOURCE description structure
	D3D11_SUBRESOURCE_DATA tmpInitData_Vertex;
	ZeroMemory(&tmpInitData_Vertex, sizeof(tmpInitData_Vertex));
	tmpInitData_Vertex.pSysMem = &mVB_Mem.at(0);
	UINT vertexCount = mVB_Mem.size();

	D3D11_SUBRESOURCE_DATA tmpInitData_Index;
	ZeroMemory(&tmpInitData_Index, sizeof(tmpInitData_Index));
	tmpInitData_Index.pSysMem = &mIB_Mem.at(0);
	UINT indexCount = mIB_Mem.size();

	//------Create VERTEX BUFFER
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = sizeof(N_DefaultVertex)* vertexCount;
	vbd.Usage = D3D11_USAGE_DEFAULT;//这个是GPU能对其读写,IMMUTABLE是GPU只读
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0; //CPU啥都干不了  D3D_USAGE
	vbd.MiscFlags = 0;//D3D11_RESOURCE_MISC_RESOURCE 具体查MSDN
	vbd.StructureByteStride = 0;

	//Create Buffers
	int hr = 0;
	hr = g_pd3dDevice11->CreateBuffer(&vbd, &tmpInitData_Vertex, &m_pVB_Gpu);
	HR_DEBUG(hr, "Mesh : Failed to create vertex buffer ! ");


	D3D11_BUFFER_DESC ibd;
	ibd.ByteWidth = sizeof(int) * indexCount;
	ibd.Usage = D3D11_USAGE_DEFAULT;//这个是GPU能对其读写,IMMUTABLE是GPU只读
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0; //CPU啥都干不了  D3D_USAGE
	ibd.MiscFlags = 0;//D3D11_RESOURCE_MISC_RESOURCE 具体查MSDN
	ibd.StructureByteStride = 0;

	//Create Buffers
	hr = g_pd3dDevice11->CreateBuffer(&ibd, &tmpInitData_Index, &m_pIB_Gpu);
	HR_DEBUG(hr, "Mesh : Failed to create index buffer ! ");

#pragma endregion CreateGpuBuffers

	return true;
};

void Mesh::mFunction_ComputeBoundingBox()
{
	//计算包围盒.......重载1

	UINT i = 0;
	NVECTOR3 tmpV;

	//遍历所有顶点，算出包围盒3分量均最 小/大 的两个顶点
	for (i = 0;i < mVB_Mem.size();i++)
	{
		if (i == 0)
		{
			//initialization
			tmpV = mVB_Mem.at(i).Pos;
			mBoundingBox.min = mVB_Mem.at(0).Pos;
			mBoundingBox.max = mVB_Mem.at(0).Pos;
		}
		//N_DEFAULT_VERTEX
		tmpV = mVB_Mem.at(i).Pos;
		if (tmpV.x <( mBoundingBox.min.x)) { mBoundingBox.min.x = tmpV.x; }
		if (tmpV.y <(mBoundingBox.min.y)) { mBoundingBox.min.y = tmpV.y; }
		if (tmpV.z <(mBoundingBox.min.z)) { mBoundingBox.min.z = tmpV.z; }

		if (tmpV.x >(mBoundingBox.max.x)) { mBoundingBox.max.x = tmpV.x; }
		if (tmpV.y >(mBoundingBox.max.y)) { mBoundingBox.max.y = tmpV.y; }
		if (tmpV.z >(mBoundingBox.max.z)) { mBoundingBox.max.z = tmpV.z; }
	}
	mBoundingBox.max += AffineTransform::GetPosition();
	mBoundingBox.min += AffineTransform::GetPosition();
}

void Mesh::mFunction_ComputeBoundingBox(std::vector<NVECTOR3>* pVertexBuffer)
{
	//计算包围盒.......重载2

	UINT i = 0;
	NVECTOR3 tmpV;
	//遍历所有顶点，算出包围盒3分量均最 小/大 的两个顶点
	for (i = 0;i < pVertexBuffer->size();i++)
	{
		if (i == 0)
		{
			//initialization
			mBoundingBox.min = mVB_Mem.at(0).Pos;
			mBoundingBox.max = mVB_Mem.at(0).Pos;
		}
		tmpV = pVertexBuffer->at(i);
		//N_DEFAULT_VERTEX
		tmpV = mVB_Mem.at(i).Pos;
		if (tmpV.x <(mBoundingBox.min.x)) { mBoundingBox.min.x = tmpV.x; }
		if (tmpV.y <(mBoundingBox.min.y)) { mBoundingBox.min.y = tmpV.y; }
		if (tmpV.z <(mBoundingBox.min.z)) { mBoundingBox.min.z = tmpV.z; }

		if (tmpV.x >(mBoundingBox.max.x)) { mBoundingBox.max.x = tmpV.x; }
		if (tmpV.y >(mBoundingBox.max.y)) { mBoundingBox.max.y = tmpV.y; }
		if (tmpV.z >(mBoundingBox.max.z)) { mBoundingBox.max.z = tmpV.z; }
	}
	mBoundingBox.max += AffineTransform::GetPosition();
	mBoundingBox.min += AffineTransform::GetPosition();
}
