
/***********************************************************************

								cpp £ºCollision Testor
				provide all kinds of collision detection
				to all kinds of objects

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

ICollisionTestor::ICollisionTestor()
{

}


ICollisionTestor::~ICollisionTestor()
{
	m_pSOBuffer->Release();
	m_pFX_CbPicking->Release();
	m_pFX_Tech_Picking->Release();
	m_pFX_CbCameraInfo->Release();
}

void ICollisionTestor::Picking(IMesh * pMesh, const NVECTOR2 & pickPixelCoord, const NVECTOR2 & screenSize, std::vector<NVECTOR3>& outCollidedPointList)
{
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Default);
	g_pImmediateContext->IASetVertexBuffers(0, 1, &pMesh->m_pVB_Gpu, &g_cVBstride_Default, &g_cVBoffset);
	g_pImmediateContext->IASetIndexBuffer(pMesh->m_pIB_Gpu, DXGI_FORMAT_R32_UINT, 0);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_pImmediateContext->OMSetBlendState(nullptr, NULL, 0x00000000);//disable color drawing??

	//update camera Info
	N_CbCameraInfo CbCam;
	ICamera* pCamera = GetScene()->GetCamera();
	pCamera->GetProjMatrix(CbCam.projMatrix);
	pCamera->GetViewMatrix(CbCam.viewMatrix);
	pCamera->GetInvProjMatrix(CbCam.invViewMatrix);
	pCamera->GetInvViewMatrix(CbCam.invProjMatrix);
	CbCam.camPos = pCamera->GetPosition();
	m_pFX_CbCameraInfo->SetRawValue(&CbCam, 0, sizeof(CbCam));

	//update mouse position to GPU to compute picking ray
	N_CbPicking CbPicking;
	CbPicking.pickingRayNormalizedDirXY = pickPixelCoord;
	m_pFX_CbPicking->SetRawValue(&CbPicking, 0, sizeof(CbPicking));

	//!!!!!!!!!!!!!!!!!!!!!!!!set SO Buffer
	const UINT offset[1] = { 0 };
	g_pImmediateContext->SOSetTargets(1, &m_pSOBuffer, offset);

	//issue a draw call (with Begin() and End(), between which SO statistic will be recorded
	UINT indexCount = pMesh->GetIndexBuffer()->size();
	g_pImmediateContext->Begin(m_pSOQuery);
	g_pImmediateContext->DrawIndexed(indexCount, 0, 0);
	g_pImmediateContext->End(m_pSOQuery);

	//!!!!HERE's the problem : how many primitives had been Streamed Output???
	//Use ID3D11Query !!
	D3D11_QUERY_DATA_SO_STATISTICS  queriedStat;

	//this while seems to wait for Gpu's availability
	while (S_OK != g_pImmediateContext->GetData(
		m_pSOQuery,
		&queriedStat,	//different query types yield different return type. in this case, D3D11_QUERY_DATA_SO_STATISTICS
		m_pSOQuery->GetDataSize(),
		0));

	//get number of primitives written to SO buffer
	UINT returnedPrimCount = queriedStat.NumPrimitivesWritten;
	//PrimitivesStorageNeeded :: actually that initialized SO buffer 
	//might be not big enough hold all SO data from shaders


	//RETRIEVE RESULT!!
	D3D11_MAPPED_SUBRESOURCE mappedSR;
	HRESULT hr = S_OK;
	hr = g_pImmediateContext->Map(m_pSOBuffer, 0, D3D11_MAP_READ, NULL, &mappedSR);
	if (FAILED(hr))
	{
		ERROR_MSG("Collosion Testor : failed to retrieve collision data (DeviceContext::Map)");
		return;
	}

	//WARNING: please match the primitive format of 'shader SO' and 'here' 
	//SO primitive vertex format is defined in Effect source file.
	//(2017.1.28)currently POSITION.xyz <--> NVECTOR3
	NVECTOR3* pVecList = reinterpret_cast<NVECTOR3*>(mappedSR.pData);
	for (UINT i = 0; i < returnedPrimCount; ++i)
	{
		outCollidedPointList.push_back(*(pVecList + i));
	}

}

UINT ICollisionTestor::Picking(IMesh * pMesh, const NVECTOR2 & mousePixelCoord, const NVECTOR2 & windowPixelSize)
{
	ERROR_MSG("not implemeneted");
	return 0;
}

/*************************************************

							P R I V A T E

************************************************/


BOOL ICollisionTestor::mFunction_Init()
{
	//Create Id3d11query to get SO info
	//so this page for more info
	//https://www.gamedev.net/blog/272/entry-1913400-using-d3d11-stream-out-for-debugging/
	D3D11_QUERY_DESC queryDesc;
	queryDesc.Query = D3D11_QUERY_SO_STATISTICS;//this enum for usage, D3d11_query_xxx
	queryDesc.MiscFlags = NULL;

	HRESULT hr = g_pd3dDevice11->CreateQuery(&queryDesc, &m_pSOQuery);
	if (FAILED(hr))
	{
		ERROR_MSG("Collsion Testor : Initialization failed! (ID3D11Query for SO Info)")
			return FALSE;
	}


	//SO Buffer initial data
	N_MinizedVertex initArr[c_maxSOVertexCount];
	ZeroMemory(initArr, sizeof(N_MinizedVertex) * c_maxSOVertexCount);
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = initArr;

	//create Stream Output buffer
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
	desc.ByteWidth = c_maxSOVertexCount * sizeof(N_MinizedVertex);//only float4 position
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;//the slowest usage but read from VideoMem is allowed
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	g_pd3dDevice11->CreateBuffer(&desc, &initData, &m_pSOBuffer);


	//Create Techiniques from FX
	m_pFX_Tech_Picking = g_pFX->GetTechniqueByName("PickingIntersection");
	m_pFX_CbCameraInfo = g_pFX->GetConstantBufferByName("cbCameraMatrix");
	m_pFX_CbPicking = g_pFX->GetConstantBufferByName("cbPicking");

	return TRUE;
}
