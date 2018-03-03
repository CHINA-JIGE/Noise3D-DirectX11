
/***********************************************************************

							class：NoiseMesh

				Desc: encapsule a Mesh class that can be 

***********************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

IMesh::IMesh():
	mRotationX_Pitch(0.0f),
	mRotationY_Yaw(0.0f),
	mRotationZ_Roll(0.0f),
	mScaleX(1.0f),
	mScaleY(1.0f),
	mScaleZ(1.0f),
	mPosition(0,0,0),
	mBoundingBox({0,0,0},{0,0,0}),
	m_pVB_Gpu(nullptr),
	m_pIB_Gpu(nullptr)
{
	D3DXMatrixIdentity(&mMatrixWorld);
	D3DXMatrixIdentity(&mMatrixWorldInvTranspose);
	SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
};

IMesh::~IMesh()
{
	ReleaseCOM(m_pVB_Gpu);
	ReleaseCOM(m_pIB_Gpu);
}

void IMesh::ResetMaterialToDefault()
{
	SetMaterial(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
}

void IMesh::SetMaterial(N_UID matName)
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

void IMesh::SetSubsetList(const std::vector<N_MeshSubsetInfo>& subsetList)
{
	mSubsetInfoList = subsetList;
}

void IMesh::GetSubsetList(std::vector<N_MeshSubsetInfo>& outRefSubsetList)
{
	outRefSubsetList = mSubsetInfoList;
}


void IMesh::SetPosition(float x,float y,float z)
{
	mPosition.x =x;
	mPosition.y =y;
	mPosition.z =z;
}

void IMesh::SetPosition(const NVECTOR3 & pos)
{
	mPosition = pos;
}

NVECTOR3 IMesh::GetPosition()
{
	return mPosition;
}

void IMesh::SetRotation(float angleX, float angleY, float angleZ)
{
	mRotationX_Pitch	= angleX;
	mRotationY_Yaw		= angleY;
	mRotationZ_Roll		= angleZ;
}

void IMesh::SetRotationX_Pitch(float angleX)
{
	mRotationX_Pitch = angleX;
};

void IMesh::SetRotationY_Yaw(float angleY)
{
	mRotationY_Yaw = angleY;
};

void IMesh::SetRotationZ_Roll(float angleZ)
{
	mRotationZ_Roll = angleZ;
}

void IMesh::RotateX_Pitch(float angleX)
{
	mRotationX_Pitch += angleX;
}

void IMesh::RotateY_Yaw(float angleY)
{
	mRotationY_Yaw += angleY;
}

void IMesh::RotateZ_Roll(float angleZ)
{
	mRotationZ_Roll += angleZ;
}

float IMesh::GetRotationX_Pitch()
{
	return mRotationX_Pitch;
}

float IMesh::GetRotationY_Yaw()
{
	return mRotationY_Yaw;
}

float IMesh::GetRotationZ_Roll()
{
	return mRotationZ_Roll;
}

void IMesh::SetScale(float scaleX, float scaleY, float scaleZ)
{
	mScaleX = scaleX;
	mScaleY = scaleY;
	mScaleZ = scaleZ;
}

void IMesh::SetScaleX(float scaleX)
{
	mScaleX = scaleX;
}

void IMesh::SetScaleY(float scaleY)
{
	mScaleY = scaleY;
}

void IMesh::SetScaleZ(float scaleZ)
{
	mScaleZ = scaleZ;
}

UINT IMesh::GetIndexCount()
{
	return mIB_Mem.size();
}

UINT IMesh::GetTriangleCount()
{
	return mIB_Mem.size()/3;
}

void IMesh::GetVertex(UINT iIndex, N_DefaultVertex& outVertex)
{
	if (iIndex < mVB_Mem.size())
	{
		outVertex = mVB_Mem.at(iIndex);
	}
}

const std::vector<N_DefaultVertex>*		IMesh::GetVertexBuffer()const 
{
	return &mVB_Mem;
}

const std::vector<UINT>* IMesh::GetIndexBuffer() const
{
	return &mIB_Mem;
}

void IMesh::GetWorldMatrix(NMATRIX & outWorldMat, NMATRIX& outWorldInvTransposeMat)
{
	mFunction_UpdateWorldMatrix();
	outWorldMat = mMatrixWorld;
	outWorldInvTransposeMat = mMatrixWorldInvTranspose;
}

N_Box IMesh::ComputeBoundingBox()
{
	mFunction_ComputeBoundingBox();

	return mBoundingBox;
};

/***********************************************************************
								PRIVATE					                    
***********************************************************************/
//this function could be externally invoked by ModelLoader..etc
bool IMesh::mFunction_UpdateDataToVideoMem(const std::vector<N_DefaultVertex>& targetVB,const std::vector<UINT>& targetIB)
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

bool IMesh::mFunction_UpdateDataToVideoMem()
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


void	IMesh::mFunction_UpdateWorldMatrix()
{
	D3DXMATRIX	tmpMatrixScaling;
	D3DXMATRIX	tmpMatrixTranslation;
	D3DXMATRIX	tmpMatrixRotation;
	D3DXMATRIX	tmpMatrix;
	float tmpDeterminant;

	//initialize
	D3DXMatrixIdentity(&tmpMatrix);
		
	//1.scale
	D3DXMatrixScaling(&tmpMatrixScaling, mScaleX, mScaleY, mScaleZ);

	//2.rotate
	D3DXMatrixRotationYawPitchRoll(&tmpMatrixRotation, mRotationY_Yaw, mRotationX_Pitch, mRotationZ_Roll);

	//3.translate
	D3DXMatrixTranslation(&tmpMatrixTranslation, mPosition.x, mPosition.y, mPosition.z);

	//4.concatenate scaling/rotation/translation（跟viewMatrix有点区别）
	D3DXMatrixMultiply(&tmpMatrix,&tmpMatrixScaling,&tmpMatrixRotation);
	D3DXMatrixMultiply(&mMatrixWorld, &tmpMatrix, &tmpMatrixTranslation);

	//world InvTranspose for normal transformation
	D3DXMatrixInverse(&mMatrixWorldInvTranspose,&tmpDeterminant,&mMatrixWorld);

	//Be careful about the row/column major of the matrix and transposes
	//D3DXMatrixTranspose(&mMatrixWorld,&mMatrixWorld);

	//Transpose of worldInverse 
	//(2018.2.12 there was a bug here, that the source matrix is set to 'tmpMatrix'!!)
	//god damn it @#$%#^
	D3DXMatrixTranspose(&mMatrixWorldInvTranspose,&mMatrixWorldInvTranspose);
}

void IMesh::mFunction_ComputeBoundingBox()
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
	D3DXVec3Add(&mBoundingBox.max, &mBoundingBox.max, &mPosition);
	D3DXVec3Add(&mBoundingBox.min, &mBoundingBox.min, &mPosition);
}

void IMesh::mFunction_ComputeBoundingBox(std::vector<NVECTOR3>* pVertexBuffer)
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
	D3DXVec3Add(&mBoundingBox.max, &mBoundingBox.max, &mPosition);
	D3DXVec3Add(&mBoundingBox.min, &mBoundingBox.min, &mPosition);
}
