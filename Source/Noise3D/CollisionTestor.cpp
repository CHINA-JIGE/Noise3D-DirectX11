
/***********************************************************************

								cpp £ºCollision Testor
				provide all kinds of collision detection
				to all kinds of objects

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

ICollisionTestor::ICollisionTestor():
	m_pRefShaderVarMgr(nullptr)
{

}


ICollisionTestor::~ICollisionTestor()
{
	ReleaseCOM(m_pSOCpuReadableBuffer);
	ReleaseCOM(m_pSOGpuWriteableBuffer);
	ReleaseCOM(m_pSOQuery);
	ReleaseCOM(m_pDSS_DisableDepthTest);
}

void ICollisionTestor::Picking_GpuBased(IMesh * pMesh, const NVECTOR2 & mouseNormalizedCoord, std::vector<NVECTOR3>& outCollidedPointList)
{
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Default);
	g_pImmediateContext->IASetVertexBuffers(0, 1, &pMesh->m_pVB_Gpu, &g_cVBstride_Default, &g_cVBoffset);
	g_pImmediateContext->IASetIndexBuffer(pMesh->m_pIB_Gpu, DXGI_FORMAT_R32_UINT, 0);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_pImmediateContext->OMSetBlendState(nullptr, NULL, 0x00000000);//disable color drawing??
	g_pImmediateContext->OMSetDepthStencilState(m_pDSS_DisableDepthTest, 0x00000000);

	//picking info
	m_pRefShaderVarMgr->SetVector2(IShaderVariableManager::NOISE_SHADER_VAR_VECTOR::PICKING_RAY_NORMALIZED_DIR_XY, mouseNormalizedCoord);

	//update camera Info
	ICamera* pCamera = GetScene()->GetCamera();
	NMATRIX projMatrix, viewMatrix, invProjMatrix, invViewMatrix;
	pCamera->GetProjMatrix(projMatrix);
	pCamera->GetViewMatrix(viewMatrix);
	pCamera->GetInvViewMatrix(invViewMatrix);
	NVECTOR3 camPos = pCamera->GetPosition();
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::PROJECTION, projMatrix);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::VIEW, viewMatrix);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::VIEW_INV, invViewMatrix);
	m_pRefShaderVarMgr->SetVector3(IShaderVariableManager::NOISE_SHADER_VAR_VECTOR::CAMERA_POS3, camPos);


	//update target tested mesh world Matrix
	NMATRIX worldMat, worldInvTransMat;
	pMesh->GetWorldMatrix(worldMat, worldInvTransMat);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::WORLD, worldMat);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::WORLD_INV_TRANSPOSE, worldInvTransMat);


	UINT offset = 0;
	ID3D11Buffer* pNullBuff = nullptr;
	//apply technique first !!!!!!!!!!!!!!!!!!!!!!!!then set SO Buffer
	m_pFX_Tech_Picking->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
	g_pImmediateContext->SOSetTargets(1, &m_pSOGpuWriteableBuffer, &offset);

	//issue a draw call (with Begin() and End(), between which SO statistic will be recorded
	UINT indexCount = pMesh->GetIndexBuffer()->size();
	g_pImmediateContext->Begin(m_pSOQuery);
	g_pImmediateContext->DrawIndexed(indexCount, 0, 0);
	g_pImmediateContext->End(m_pSOQuery);
	g_pImmediateContext->SOSetTargets(1, &pNullBuff, &offset);

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
	UINT returnedPrimCount = UINT(queriedStat.NumPrimitivesWritten);
	//PrimitivesStorageNeeded :: actually that initialized SO buffer 
	//might be not big enough hold all SO data from shaders


	//RETRIEVE RESULT!!
	//but first, we need to copy data from 'default' buffer to 'staging' buffer
	g_pImmediateContext->CopyResource(m_pSOCpuReadableBuffer, m_pSOGpuWriteableBuffer);
	//use device context->map
	D3D11_MAPPED_SUBRESOURCE mappedSR;
	HRESULT hr = S_OK;
	hr = g_pImmediateContext->Map(m_pSOCpuReadableBuffer, 0, D3D11_MAP_READ, NULL, &mappedSR);

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

	g_pImmediateContext->Unmap(m_pSOCpuReadableBuffer,0);

}

UINT ICollisionTestor::Picking_GpuBased(IMesh * pMesh, const NVECTOR2 & mouseNormalizedCoord)
{
	//preparation is similar to another PICKING

	g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Default);
	g_pImmediateContext->IASetVertexBuffers(0, 1, &pMesh->m_pVB_Gpu, &g_cVBstride_Default, &g_cVBoffset);
	g_pImmediateContext->IASetIndexBuffer(pMesh->m_pIB_Gpu, DXGI_FORMAT_R32_UINT, 0);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_pImmediateContext->OMSetBlendState(nullptr, NULL, 0x00000000);//disable color drawing??
	g_pImmediateContext->OMSetDepthStencilState(m_pDSS_DisableDepthTest,0x00000000) ;

	//picking info
	m_pRefShaderVarMgr->SetVector2(IShaderVariableManager::NOISE_SHADER_VAR_VECTOR::PICKING_RAY_NORMALIZED_DIR_XY, mouseNormalizedCoord);

	//update camera Info
	ICamera* pCamera = GetScene()->GetCamera();
	NMATRIX projMatrix, viewMatrix, invProjMatrix, invViewMatrix;
	pCamera->GetProjMatrix(projMatrix);
	pCamera->GetViewMatrix(viewMatrix);
	pCamera->GetInvViewMatrix(invViewMatrix);
	NVECTOR3 camPos = pCamera->GetPosition();
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::PROJECTION, projMatrix);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::VIEW, viewMatrix);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::VIEW_INV, invViewMatrix);
	m_pRefShaderVarMgr->SetVector3(IShaderVariableManager::NOISE_SHADER_VAR_VECTOR::CAMERA_POS3, camPos);

	//update target tested mesh world Matrix
	NMATRIX worldMat, worldInvTransMat;
	pMesh->GetWorldMatrix(worldMat, worldInvTransMat);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::WORLD, worldMat);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::WORLD_INV_TRANSPOSE, worldInvTransMat);



	UINT offset =0;
	ID3D11Buffer* pNullBuff= nullptr;
	//apply technique first !!!!!!!!!!!!!!!!!!!!!!!!then set SO Buffer
	m_pFX_Tech_Picking->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
	g_pImmediateContext->SOSetTargets(1, &m_pSOGpuWriteableBuffer, &offset);

	//issue a draw call (with Begin() and End(), between which SO statistic will be recorded
	UINT indexCount = pMesh->GetIndexBuffer()->size();
	g_pImmediateContext->Begin(m_pSOQuery);
	g_pImmediateContext->DrawIndexed(indexCount, 0, 0);
	g_pImmediateContext->End(m_pSOQuery);
	g_pImmediateContext->SOSetTargets(1, &pNullBuff, &offset);

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
	return UINT(queriedStat.NumPrimitivesWritten);
	//PrimitivesStorageNeeded :: actually that initialized SO buffer 
	//might be not big enough hold all SO data from shaders

}

/*************************************************

							P R I V A T E

************************************************/


bool ICollisionTestor::mFunction_Init()
{
	m_pRefShaderVarMgr = IShaderVariableManager::GetSingleton();
	if (m_pRefShaderVarMgr == nullptr)
	{
		ERROR_MSG("Collision Testor: Initialization failed! (Shader var manager)");
		return false;
	}

	//Create Id3d11query to get SO info
	//so this page for more info
	//https://www.gamedev.net/blog/272/entry-1913400-using-d3d11-stream-out-for-debugging/
	D3D11_QUERY_DESC queryDesc;
	queryDesc.Query = D3D11_QUERY_SO_STATISTICS;//this enum for usage, D3d11_query_xxx
	queryDesc.MiscFlags = NULL;


	HRESULT hr = g_pd3dDevice11->CreateQuery(&queryDesc, &m_pSOQuery);
	HR_DEBUG(hr, "Collision Testor : Initialization failed! (ID3D11Query for SO Info)");


	//SO Buffer initial data
	N_MinizedVertex initArr[c_maxSOVertexCount];
	ZeroMemory(initArr, sizeof(N_MinizedVertex) * c_maxSOVertexCount);
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = initArr;


	//create 2 Stream Output buffer (one for gpu write(default) , one for cpu read(staging))
	// 1. GPU writeable SO buffer
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
	desc.ByteWidth = c_maxSOVertexCount * sizeof(N_MinizedVertex);//only float4 position
	desc.CPUAccessFlags = NULL;
	desc.Usage = D3D11_USAGE_DEFAULT;//the slowest usage but read from VideoMem is allowed
	desc.MiscFlags = NULL;
	desc.StructureByteStride = NULL;
	hr = g_pd3dDevice11->CreateBuffer(&desc, &initData, &m_pSOGpuWriteableBuffer);
	
	HR_DEBUG(hr, "Collision Testor : failed to create Stream Out Buffer! #1" + std::to_string(hr));

	// 2. CPU readable SO data buffer , Simply re-use the above struct
	desc.BindFlags =NULL;//if we 
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	hr = g_pd3dDevice11->CreateBuffer(&desc, &initData, &m_pSOCpuReadableBuffer);

	HR_DEBUG(hr, "Collision Testor : failed to create Stream Out Buffer #2!" + std::to_string(hr));

	//Create DSS
	if (!mFunction_InitDSS())return false;

	//Create Techiniques from FX
	m_pFX_Tech_Picking = g_pFX->GetTechniqueByName("PickingIntersection");

	return true;
}

inline bool ICollisionTestor::mFunction_InitDSS()
{
	HRESULT hr = S_OK;
	//depth stencil state
	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(dssDesc));
	dssDesc.DepthEnable = TRUE;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthEnable = FALSE;
	dssDesc.StencilEnable = FALSE;
	hr = g_pd3dDevice11->CreateDepthStencilState(&dssDesc, &m_pDSS_DisableDepthTest);
	HR_DEBUG(hr, "Collision Testor : Create Depth Stencil State Failed!!!");
	return true;
}
