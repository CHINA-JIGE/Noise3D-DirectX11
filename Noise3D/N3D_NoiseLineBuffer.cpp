
/***********************************************************************

						类：NOISE Line Buffer

							简述：负责渲染

************************************************************************/
#include "Noise3D.h"


NoiseLineBuffer::NoiseLineBuffer()
{
	mCanUpdateToGpu = FALSE;
	mVertexBufferByteSize = 0;
	m_pFatherScene = NULL;
	m_pVertexBuffer = NULL;
	m_pVertexInMem = new std::vector<N_SimpleVertex>;
}


NoiseLineBuffer::~NoiseLineBuffer()
{
	ReleaseCOM(m_pVertexBuffer);
}


void NoiseLineBuffer::AddLine3D(NVECTOR3 v1, NVECTOR3 v2, NVECTOR4 color1, NVECTOR4 color2)
{
	//.....................
	N_SimpleVertex tmpVertex;
	tmpVertex.Pos = v1;
	tmpVertex.Color = color1;
	m_pVertexInMem->push_back(tmpVertex);

	tmpVertex.Pos = v2;
	tmpVertex.Color = color2;
	m_pVertexInMem->push_back(tmpVertex);

	mCanUpdateToGpu = TRUE;
}


void NoiseLineBuffer::DeleteLine3D(UINT index)
{
	std::vector<N_SimpleVertex>::iterator tmp_iter;
	if (index < m_pVertexInMem->size())
	{
		tmp_iter += index;
		m_pVertexInMem->erase(tmp_iter);
	}
	mCanUpdateToGpu = TRUE;
}


BOOL NoiseLineBuffer::AddToRenderList()
{
	if (m_pFatherScene == NULL)
	{
		DEBUG_MSG("NoiseLineBuffer: NoiseScene Has Not been created!", "", "");
		return FALSE;
	}
	m_pFatherScene->m_pChildRenderer->m_pRenderList_Line->push_back(this);

	//Update Data to GPU if data is not up to date
	if (mCanUpdateToGpu)
	{
		mFunction_UpdateToGpu();
		mCanUpdateToGpu = FALSE;
	}
	return TRUE;
}


UINT NoiseLineBuffer::GetLineCount()
{
	return (UINT)(m_pVertexInMem->size()/2);
}




/***********************************************************************
						PRIVATE
***********************************************************************/

BOOL NoiseLineBuffer::mFunction_InitVB(UINT mVertexCount)
{
	//Create VERTEX BUFFER
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = mVertexCount * sizeof(N_SimpleVertex);
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0; 
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;


	D3D11_SUBRESOURCE_DATA tmpInitData;
	tmpInitData.pSysMem = &m_pVertexInMem->at(0);
	tmpInitData.SysMemPitch = 0;
	tmpInitData.SysMemSlicePitch = 0;


	mVertexBufferByteSize = vbd.ByteWidth;

	//Create Buffers
	HRESULT hr = 0;
	hr = g_pd3dDevice->CreateBuffer(&vbd, &tmpInitData, &m_pVertexBuffer);
	HR_DEBUG(hr, "VERTEX BUFFER创建失败");

	return TRUE;
}

void NoiseLineBuffer::mFunction_UpdateToGpu()
{
	if (m_pVertexBuffer == NULL)
	{
		mFunction_InitVB(m_pVertexInMem->size());
	}


	//要先判断是否需要扩张Vertex Buffer的规模
	if ((m_pVertexInMem->size() *sizeof(N_SimpleVertex))  > mVertexBufferByteSize)
	{
		//如果之前创建过VB，那就只能先销毁再创建
		if (m_pVertexBuffer)
		{
			m_pVertexBuffer->Release();
			mFunction_InitVB(m_pVertexInMem->size());
		}
	}

	g_pImmediateContext->UpdateSubresource(m_pVertexBuffer, 0, 0, &m_pVertexInMem->at(0), 0, 0);
}
