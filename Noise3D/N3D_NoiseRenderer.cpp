
/***********************************************************************

                           类：NOISE Renderer

						简述：负责渲染

************************************************************************/

#include "Noise3D.h"

static UINT VBstride_Default = sizeof(N_DefaultVertex);		//VertexBuffer的每个元素的字节跨度
static UINT VBstride_Simple = sizeof(N_SimpleVertex);
static UINT VBoffset = 0;				//VertexBuffer顶点序号偏移 因为从头开始所以offset是0

static NoiseMesh*							tmp_pMesh;
static NoiseCamera*						tmp_pCamera;
static NoiseAtmosphere*				tmp_pAtmo;
static D3DX11_TECHNIQUE_DESC	tmp_pTechDesc;


NoiseRenderer::NoiseRenderer()
{
	m_pFatherScene	= nullptr;
	mCanUpdateCbCameraMatrix = FALSE;
	m_pRenderList_Mesh= new std::vector <NoiseMesh*>;
	m_pRenderList_CommonGraphicObj	= new std::vector<NoiseGraphicObject*>;
	m_pRenderList_TextDynamic	= new std::vector<Noise2DTextDynamic*>;//for Text Rendering
	m_pRenderList_TextStatic = new std::vector<Noise2DTextStatic*>;//for Text Rendering
	m_pRenderList_GUIGraphicObj	= new std::vector<NoiseGraphicObject*>;//for GUI rendering
	m_pRenderList_Atmosphere	= new std::vector<NoiseAtmosphere*>;
	m_pFX = nullptr;
	m_pFX_Tech_Default = nullptr;
	m_pFX_Tech_Solid3D = nullptr;
	m_pFX_Tech_Solid2D = nullptr;
	m_pFX_Tech_Textured2D = nullptr;
	m_pFX_Tech_DrawSky = nullptr;
	m_FillMode = NOISE_FILLMODE_SOLID;
	m_CullMode = NOISE_CULLMODE_NONE;
	m_BlendMode = NOISE_BLENDMODE_OPAQUE;
};

void NoiseRenderer::Destroy()
{
	ReleaseCOM(m_pFX);
	ReleaseCOM(m_pRasterState_Solid_CullNone);
	ReleaseCOM(m_pRasterState_Solid_CullBack);
	ReleaseCOM(m_pRasterState_Solid_CullFront);
	ReleaseCOM(m_pRasterState_WireFrame_CullFront);
	ReleaseCOM(m_pRasterState_WireFrame_CullNone);
	ReleaseCOM(m_pRasterState_WireFrame_CullBack);
	ReleaseCOM(m_pBlendState_AlphaTransparency);
	ReleaseCOM(m_pBlendState_ColorAdd);
	ReleaseCOM(m_pBlendState_ColorMultiply);
	ReleaseCOM(m_pBlendState_Opaque);
	ReleaseCOM(m_pDepthStencilState_DisableDepthTest);
	ReleaseCOM(m_pDepthStencilState_EnableDepthTest);
};

void	NoiseRenderer::RenderMeshes()
{
	//validation before rendering
	if (m_pFatherScene->m_pChildMaterialMgr == nullptr)
	{
		DEBUG_MSG1("Noise Renderer : Material Mgr has not been created");
		return;
	};
	if (m_pFatherScene->m_pChildTextureMgr == nullptr)
	{
		DEBUG_MSG1("Noise Renderer : Texture Mgr has not been created");
		return;
	};


	UINT i = 0, j = 0, k = 0;
	 tmp_pCamera = m_pFatherScene->GetCamera();

	//更新ConstantBuffer:修改过就更新(cbRarely)
	 mFunction_RenderMeshInList_UpdateCbRarely();


	//更新ConstantBuffer:每帧更新一次 (cbPerFrame)
	 mFunction_RenderMeshInList_UpdateCbPerFrame();

	 //更新ConstantBuffer : Proj / View Matrix
	 mFunction_CameraMatrix_Update();


#pragma region Render Mesh
	 //for every mesh
	for(i = 0;	i<(m_pRenderList_Mesh->size());	i++)
	{
		//取出渲染列表中的mesh指针
		tmp_pMesh = m_pRenderList_Mesh->at(i);

		//更新ConstantBuffer:每物体更新一次(cbPerObject)
		mFunction_RenderMeshInList_UpdateCbPerObject();

		//更新完cb就准备开始draw了
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Default);
		g_pImmediateContext->IASetVertexBuffers(0,1,&tmp_pMesh->m_pVertexBuffer,&VBstride_Default,&VBoffset);
		g_pImmediateContext->IASetIndexBuffer(tmp_pMesh->m_pIndexBuffer,DXGI_FORMAT_R32_UINT,0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//设置fillmode和cullmode
		mFunction_SetRasterState(m_FillMode,m_CullMode);

		//设置blend state
		mFunction_SetBlendState(m_BlendMode);

		//设置samplerState
		m_pFX_SamplerState_Default->SetSampler(0, m_pSamplerState_FilterAnis);

		//设置depth/Stencil State
		g_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState_EnableDepthTest, 0xffffffff);


		//for every subset
		UINT meshSubsetCount = tmp_pMesh->m_pSubsetInfoList->size();
		for (j = 0;j < meshSubsetCount;j++)
		{
			//更新ConstantBuffer:每Subset,在一个mesh里面有不同Material的都算一个subset
			mFunction_RenderMeshInList_UpdateCbPerSubset(j);

			//遍历所用tech的所有pass ---- index starts from 1
			m_pFX_Tech_Default->GetDesc(&tmp_pTechDesc);
			for (k = 0;k < tmp_pTechDesc.Passes; k++)
			{
				m_pFX_Tech_Default->GetPassByIndex(k)->Apply(0, g_pImmediateContext);
				g_pImmediateContext->DrawIndexed(tmp_pMesh->m_IndexCount, 0, 0);
			}
		}
}
#pragma endregion Render Mesh

}

void NoiseRenderer::RenderGraphicObjects()
{
	//validation before rendering
	if (m_pFatherScene->m_pChildTextureMgr == nullptr)
	{
		DEBUG_MSG1("Noise Renderer : Texture Mgr has not been created");
		return;
	};


	//设置fillmode和cullmode
	mFunction_SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_NONE);

	//设置blend state
	mFunction_SetBlendState(m_BlendMode);

	//设置samplerState
	m_pFX_SamplerState_Default->SetSampler(0, m_pSamplerState_FilterAnis);

	//设置depth/Stencil State
	g_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState_EnableDepthTest, 0xffffffff);

	//render various kinds of primitives
	mFunction_GraphicObj_RenderLine2DInList(m_pRenderList_CommonGraphicObj);
	mFunction_GraphicObj_RenderLine3DInList(m_pRenderList_CommonGraphicObj);
	mFunction_GraphicObj_RenderPoint3DInList(m_pRenderList_CommonGraphicObj);
	mFunction_GraphicObj_RenderPoint2DInList(m_pRenderList_CommonGraphicObj);
	mFunction_GraphicObj_RenderTriangle2DInList(m_pRenderList_CommonGraphicObj);
}

void NoiseRenderer::RenderAtmosphere()
{
	//validation before rendering
	if (m_pFatherScene->m_pChildMaterialMgr == nullptr)
	{
		DEBUG_MSG1("Noise Renderer : Material Mgr has not been created");
		return;
	};
	if (m_pFatherScene->m_pChildTextureMgr == nullptr)
	{
		DEBUG_MSG1("Noise Renderer : Texture Mgr has not been created");
		return;
	};

	//...................
	UINT i = 0;
	tmp_pCamera = m_pFatherScene->GetCamera();


	//update view/proj matrix
	mFunction_CameraMatrix_Update();



	//actually there is only 1 atmosphere because you dont need more 
	for (i = 0;i < m_pRenderList_Atmosphere->size();i++)
	{
		tmp_pAtmo = m_pRenderList_Atmosphere->at(i);

		if (tmp_pAtmo == nullptr)continue;

		//enable/disable fog effect 
		mFunction_Atmosphere_Fog_Update();


	#pragma region Draw Sky

		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pAtmo->m_pVB_Gpu_Sky, &VBstride_Simple, &VBoffset);
		g_pImmediateContext->IASetIndexBuffer(tmp_pAtmo->m_pIB_Gpu_Sky, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//......Set States
		mFunction_SetRasterState(m_FillMode, m_CullMode);
		mFunction_SetBlendState(m_BlendMode);
		m_pFX_SamplerState_Default->SetSampler(0, m_pSamplerState_FilterAnis);
		g_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState_EnableDepthTest, 0xffffffff);


		//update Vertices or atmo param to GPU
		//shaders will decide to draw skybox or sky dome
		//( there are SkyboxValid & SkyDomeValid BOOL)
		mFunction_Atmosphere_SkyDome_Update();
		mFunction_Atmosphere_SkyBox_Update();
		mFunction_Atmosphere_UpdateCbAtmosphere();


		//traverse passes in one technique ---- pass index starts from 1
		m_pFX_Tech_DrawSky->GetDesc(&tmp_pTechDesc);
		for (UINT k = 0;k < tmp_pTechDesc.Passes; k++)
		{
			m_pFX_Tech_DrawSky->GetPassByIndex(k)->Apply(0, g_pImmediateContext);
			g_pImmediateContext->DrawIndexed(tmp_pAtmo->m_pIB_Mem_Sky->size(), 0, 0);
		}

		//allow atmosphere to "add to render list" again 
		tmp_pAtmo->mFogHasBeenAddedToRenderList = FALSE;

	#pragma endregion Draw Sky
	}

}

void NoiseRenderer::RenderTexts()
{
	//validation before rendering
	if (m_pFatherScene->m_pChildTextureMgr == nullptr)
	{
		DEBUG_MSG1("Noise Renderer : Texture Mgr has not been created");
		return;
	};

	mFunction_SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_NONE);
	mFunction_SetBlendState(m_BlendMode);
	m_pFX_SamplerState_Default->SetSampler(0, m_pSamplerState_FilterAnis);
	g_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState_EnableDepthTest, 0xffffffff);

	//render TEXT
	mFunction_TextGraphicObj_RenderTriangles();
}

void NoiseRenderer::RenderGUIObjects()
{
	//validation before rendering
	if (m_pFatherScene->m_pChildTextureMgr == nullptr)
	{
		DEBUG_MSG1("Noise Renderer : Texture Mgr has not been created");
		return;
	};

	mFunction_SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_NONE);
	mFunction_SetBlendState(m_BlendMode);
	m_pFX_SamplerState_Default->SetSampler(0, m_pSamplerState_FilterAnis);
	g_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState_EnableDepthTest, 0xffffffff);
	
	//render internal graphic objects
	mFunction_GraphicObj_RenderTriangle2DInList(m_pRenderList_GUIGraphicObj);
}


void NoiseRenderer::AddOjectToRenderList(NoiseMesh& obj)
{
	m_pRenderList_Mesh->push_back(&obj);
};

void NoiseRenderer::AddOjectToRenderList(NoiseGraphicObject& obj)
{
	mFunction_GraphicObj_AddToRenderList(&obj, m_pRenderList_CommonGraphicObj);
}

void NoiseRenderer::AddOjectToRenderList(NoiseAtmosphere& obj)
{
	m_pRenderList_Atmosphere->push_back(&obj);
	//fog color will only be rendered after ADDTORENDERLIST();
	obj.mFogHasBeenAddedToRenderList = TRUE;
};

void NoiseRenderer::AddOjectToRenderList(NoiseGUIButton & obj)
{
	mFunction_GraphicObj_AddToRenderList(obj.m_pGraphicObj, m_pRenderList_GUIGraphicObj);
};

void NoiseRenderer::AddOjectToRenderList(NoiseGUIScrollBar & obj)
{
	mFunction_GraphicObj_AddToRenderList(obj.m_pGraphicObj, m_pRenderList_GUIGraphicObj);
	mFunction_GraphicObj_AddToRenderList(obj.m_pButtonScrolling->m_pGraphicObj, m_pRenderList_GUIGraphicObj);
};

void NoiseRenderer::AddOjectToRenderList(NoiseGUITextBox & obj)
{
	//internal graphic object
	mFunction_GraphicObj_AddToRenderList(obj.m_pGraphicObj, m_pRenderList_GUIGraphicObj);
};

void NoiseRenderer::AddOjectToRenderList(Noise2DTextDynamic & obj)
{
	obj.mFunction_UpdateGraphicObject();
	m_pRenderList_TextDynamic->push_back(&obj);
	//Update Data to GPU if data is not up to date , 6 object types for now
	for (UINT i = 0;i <NOISE_GRAPHIC_OBJECT_BUFFER_COUNT;i++)
	{
		if (obj.m_pGraphicObj->mCanUpdateToGpu[i])
		{
			obj.m_pGraphicObj->mFunction_UpdateVerticesToGpu(i);
			obj.m_pGraphicObj->mCanUpdateToGpu[i] = FALSE;
			if (i == NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D)obj.m_pGraphicObj->mFunction_GenerateRectSubsetInfo();
		}
	}
}

void NoiseRenderer::AddOjectToRenderList(Noise2DTextStatic & obj)
{
	obj.mFunction_UpdateGraphicObject();
	m_pRenderList_TextStatic->push_back(&obj);
	//Update Data to GPU if data is not up to date , 5 object types for now
	for (UINT i = 0;i <NOISE_GRAPHIC_OBJECT_BUFFER_COUNT;i++)
	{
		if (obj.m_pGraphicObj->mCanUpdateToGpu[i])
		{
			obj.m_pGraphicObj->mFunction_UpdateVerticesToGpu(i);
			obj.m_pGraphicObj->mCanUpdateToGpu[i] = FALSE;
			if (i == NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D)obj.m_pGraphicObj->mFunction_GenerateRectSubsetInfo();
		}
	}
};


void	NoiseRenderer::ClearBackground(NVECTOR4 color)
{
	float ClearColor[4] = { color.x,color.y,color.z,color.w };
	g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, ClearColor );
	//我擦！！！纠结这么久原来是要clearDepth!!!!!!
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView,
		D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,1.0f,0);
};

void	NoiseRenderer::RenderToScreen()
{
		g_pSwapChain->Present(0, 0 );

		//reset some state
		mCanUpdateCbCameraMatrix = TRUE;

		//clear render list
		m_pRenderList_CommonGraphicObj->clear();
		m_pRenderList_Mesh->clear();
		m_pRenderList_Atmosphere->clear();
};

void NoiseRenderer::SetFillMode(NOISE_FILLMODE iMode)
{
	m_FillMode = iMode;
}

void NoiseRenderer::SetCullMode(NOISE_CULLMODE iMode)
{
	m_CullMode = iMode;
}

void NoiseRenderer::SetBlendingMode(NOISE_BLENDMODE iMode)
{
	m_BlendMode = iMode;
};



/************************************************************************
                                            PRIVATE                        
************************************************************************/
BOOL	NoiseRenderer::mFunction_Init()
{
	HRESULT hr = S_OK;

	mFunction_Init_CreateEffectFromMemory("shader\\Main.fxo");

	//创建Technique
	m_pFX_Tech_Default =	m_pFX->GetTechniqueByName("DefaultDraw");
	m_pFX_Tech_Solid3D =	m_pFX->GetTechniqueByName("DrawSolid3D");
	m_pFX_Tech_Solid2D =	m_pFX->GetTechniqueByName("DrawSolid2D");
	m_pFX_Tech_Textured2D = m_pFX->GetTechniqueByName("DrawTextured2D");
	m_pFX_Tech_DrawSky = m_pFX->GetTechniqueByName("DrawSky");
	m_pFX_Tech_DrawText2D =	m_pFX->GetTechniqueByName("DrawText2D");

#pragma region Create Input Layout
	//default vertex input layout
	D3DX11_PASS_DESC passDesc;
	m_pFX_Tech_Default->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = g_pd3dDevice11->CreateInputLayout(
		&g_VertexDesc_Default[0],
		g_VertexDesc_Default_ElementCount,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&g_pVertexLayout_Default);
	HR_DEBUG(hr, "创建input Layout失败！");

	//simple vertex input layout
	m_pFX_Tech_Solid3D->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = g_pd3dDevice11->CreateInputLayout(
		&g_VertexDesc_Simple[0],
		g_VertexDesc_Simple_ElementCount,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&g_pVertexLayout_Simple);
	HR_DEBUG(hr, "创建input Layout失败！");
#pragma endregion Create Input Layout

#pragma region Create Fx Variable
	//创建Cbuffer
	m_pFX_CbPerFrame=m_pFX->GetConstantBufferByName("cbPerFrame");
	m_pFX_CbPerObject=m_pFX->GetConstantBufferByName("cbPerObject");
	m_pFX_CbPerSubset = m_pFX->GetConstantBufferByName("cbPerSubset");
	m_pFX_CbRarely=m_pFX->GetConstantBufferByName("cbRarely");
	m_pFX_CbSolid3D = m_pFX->GetConstantBufferByName("cbCameraMatrix");
	m_pFX_CbAtmosphere = m_pFX->GetConstantBufferByName("cbAtmosphere");
	m_pFX_CbDrawText2D = m_pFX->GetConstantBufferByName("cbDrawText2D");

	//纹理
	m_pFX_Texture_Diffuse = m_pFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	m_pFX_Texture_Normal = m_pFX->GetVariableByName("gNormalMap")->AsShaderResource();
	m_pFX_Texture_Specular = m_pFX->GetVariableByName("gSpecularMap")->AsShaderResource();
	m_pFX_Texture_CubeMap = m_pFX->GetVariableByName("gCubeMap")->AsShaderResource();
	m_pFX2D_Texture_Diffuse = m_pFX->GetVariableByName("g2D_DiffuseMap")->AsShaderResource();

	//sampler state ( it needs a name in FX so for the time being I had to use D3DX11Effect
	m_pFX_SamplerState_Default = m_pFX->GetVariableByName("samplerDefault")->AsSampler();

#pragma endregion Create Fx Variable

	//Create Various kinds of states
	if (!mFunction_Init_CreateRasterState())return FALSE;
	if (!mFunction_Init_CreateBlendState())return FALSE;
	if (!mFunction_Init_CreateSamplerState())return FALSE;
	if (!mFunction_Init_CreateDepthStencilState())return FALSE;

	return TRUE;
}

BOOL	NoiseRenderer::mFunction_Init_CreateBlendState()
{

	//source color : the first color in blending equation
	HRESULT hr = S_OK;

	D3D11_BLEND_DESC tmpBlendDesc;
	tmpBlendDesc.AlphaToCoverageEnable = FALSE; // ???related to multi-sampling
	tmpBlendDesc.IndependentBlendEnable = FALSE; //determine if 8 simultaneous render targets are rendered with same blend state
	tmpBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	tmpBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	tmpBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	tmpBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	tmpBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	tmpBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	tmpBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tmpBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pd3dDevice11->CreateBlendState(&tmpBlendDesc, &m_pBlendState_Opaque);
	HR_DEBUG(hr, "Create blend state(opaque) failed!!");


	tmpBlendDesc.AlphaToCoverageEnable = FALSE; // ???related to multi-sampling
	tmpBlendDesc.IndependentBlendEnable = FALSE; //determine if 8 simultaneous render targets are rendered with same blend state
	tmpBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	tmpBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	tmpBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	tmpBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	tmpBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	tmpBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	tmpBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tmpBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pd3dDevice11->CreateBlendState(&tmpBlendDesc, &m_pBlendState_ColorAdd);
	HR_DEBUG(hr, "Create blend state(Color Add) failed!!");


	tmpBlendDesc.AlphaToCoverageEnable = FALSE; // ???related to multi-sampling
	tmpBlendDesc.IndependentBlendEnable = FALSE; //determine if 8 simultaneous render targets are rendered with same blend state
	tmpBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	tmpBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
	tmpBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	tmpBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	tmpBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	tmpBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	tmpBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tmpBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pd3dDevice11->CreateBlendState(&tmpBlendDesc, &m_pBlendState_ColorMultiply);
	HR_DEBUG(hr, "Create blend state(Color Filter) failed!!");

	tmpBlendDesc.AlphaToCoverageEnable = FALSE; // ???related to multi-sampling
	tmpBlendDesc.IndependentBlendEnable = FALSE; //determine if 8 simultaneous render targets are rendered with same blend state
	tmpBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	tmpBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;//what about D3D11_BLEND_SRC1_COLOR
	tmpBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	tmpBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	tmpBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	tmpBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	tmpBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tmpBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pd3dDevice11->CreateBlendState(&tmpBlendDesc, &m_pBlendState_AlphaTransparency);
	HR_DEBUG(hr, "Create blend state(Transparency) failed!!");

	return TRUE;
};

BOOL	NoiseRenderer::mFunction_Init_CreateRasterState()
{
	HRESULT hr = S_OK;
	//创建预设的光栅化state
	//Create Raster State;If you want various Raster State,you should pre-Create all of them in the beginning
	D3D11_RASTERIZER_DESC tmpRasterStateDesc;//光栅化设置
	ZeroMemory(&tmpRasterStateDesc, sizeof(D3D11_RASTERIZER_DESC));
	tmpRasterStateDesc.AntialiasedLineEnable = TRUE;//抗锯齿设置
	tmpRasterStateDesc.CullMode = D3D11_CULL_NONE;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_SOLID;
	hr = g_pd3dDevice11->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_Solid_CullNone);
	HR_DEBUG(hr, "Create m_pRasterState_Solid_CullNone failed");

	tmpRasterStateDesc.CullMode = D3D11_CULL_NONE;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = g_pd3dDevice11->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_WireFrame_CullNone);
	HR_DEBUG(hr, "Create m_pRasterState_WireFrame_CullNone failed");

	tmpRasterStateDesc.CullMode = D3D11_CULL_BACK;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_SOLID;
	hr = g_pd3dDevice11->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_Solid_CullBack);
	HR_DEBUG(hr, "Create m_pRasterState_Solid_CullBack failed");

	tmpRasterStateDesc.CullMode = D3D11_CULL_BACK;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = g_pd3dDevice11->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_WireFrame_CullBack);
	HR_DEBUG(hr, "Create m_pRasterState_WireFrame_CullBack failed");

	tmpRasterStateDesc.CullMode = D3D11_CULL_FRONT;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_SOLID;
	hr = g_pd3dDevice11->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_Solid_CullFront);
	HR_DEBUG(hr, "Create m_pRasterState_Solid_CullFront failed");

	tmpRasterStateDesc.CullMode = D3D11_CULL_FRONT;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = g_pd3dDevice11->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_WireFrame_CullFront);
	HR_DEBUG(hr, "Createm_pRasterState_WireFrame_CullFront failed");

	return TRUE;
};

BOOL	NoiseRenderer::mFunction_Init_CreateSamplerState()
{
	HRESULT hr = S_OK;

	//sampler state (well,maybe I should use ID3DX11SamplerStateVar to Update sampler state to GPU
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory(&samDesc, sizeof(samDesc));
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samDesc.MaxAnisotropy = 4;
	hr = g_pd3dDevice11->CreateSamplerState(&samDesc, &m_pSamplerState_FilterAnis);
	HR_DEBUG(hr, "Create Sampler State failed!!");

	return TRUE;
};

BOOL	NoiseRenderer::mFunction_Init_CreateDepthStencilState()
{
	HRESULT hr = S_OK;
	//depth stencil state
	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(dssDesc));
	dssDesc.DepthEnable = TRUE;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.StencilEnable = FALSE;
	hr = g_pd3dDevice11->CreateDepthStencilState(&dssDesc, &m_pDepthStencilState_EnableDepthTest);
	HR_DEBUG(hr, "Create Depth Stencil State Failed!!!");


	ZeroMemory(&dssDesc, sizeof(dssDesc));
	dssDesc.DepthEnable = FALSE;
	dssDesc.StencilEnable = FALSE;
	hr = g_pd3dDevice11->CreateDepthStencilState(&dssDesc, &m_pDepthStencilState_DisableDepthTest);
	HR_DEBUG(hr, "Create Depth Stencil State Failed!!!");

	return TRUE;
};

BOOL	NoiseRenderer::mFunction_Init_CreateEffectFromFile(LPCWSTR fxPath)
{
	HRESULT hr = S_OK;

	DWORD shaderFlags = 0;
#if defined(DEBUG)||defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
	ID3D10Blob*	compiledFX;
	ID3D10Blob*	compilationMsg;
	
	//编译fx文件
	hr = D3DX11CompileFromFile(
		fxPath,0,0,0,"fx_5_0",
		shaderFlags,0,0,&compiledFX,
		&compilationMsg,0);

	//看看编译有无错误信息
	//To see if there is compiling error
	if (compilationMsg != 0)
	{
		DEBUG_MSG1("Noise Renderer : Shader Compilation Failed !!");
		ReleaseCOM(compilationMsg);
	}


	hr = D3DX11CreateEffectFromMemory(
		compiledFX->GetBufferPointer(),
		compiledFX->GetBufferSize(),
		0,g_pd3dDevice11,&m_pFX);
	HR_DEBUG(hr,"Create Basic Effect Fail!");
	ReleaseCOM(compiledFX);

	return TRUE;
};

BOOL	NoiseRenderer::mFunction_Init_CreateEffectFromMemory(char* compiledShaderPath)
{
	std::vector<char> compiledShader;

	//加载fxo文件
	if (!NoiseFileManager::ImportFile_PURE(compiledShaderPath, &compiledShader))
	{
		return FALSE; 
	}

	//创建fx特效框架
	HRESULT hr = S_OK;
	hr = D3DX11CreateEffectFromMemory(&compiledShader[0], compiledShader.size(), 0, g_pd3dDevice11, &m_pFX);
	HR_DEBUG(hr,"load compiled shader failed");

	return TRUE;
}



void		NoiseRenderer::mFunction_SetRasterState(NOISE_FILLMODE iFillMode, NOISE_CULLMODE iCullMode)
{
	//fillMode , or "which primitive to draw, point?line?triangle?" , affect the decision of topology
	//like if user chooses WIREFRAME mode , the topology will be LINELIST

	switch(iFillMode)
	{
	//Solid Mode
	case NOISE_FILLMODE_SOLID:
		switch (iCullMode)
		{
		case NOISE_CULLMODE_NONE:
			g_pImmediateContext->RSSetState(m_pRasterState_Solid_CullNone);
			break;

		case NOISE_CULLMODE_FRONT:
			g_pImmediateContext->RSSetState(m_pRasterState_Solid_CullFront);
			break;

		case NOISE_CULLMODE_BACK:
			g_pImmediateContext->RSSetState(m_pRasterState_Solid_CullBack);
			break;

		default:
			break;
		}
		break;
	

	//WireFrame
	case NOISE_FILLMODE_WIREFRAME:
		switch (iCullMode)
		{
		case NOISE_CULLMODE_NONE:
			g_pImmediateContext->RSSetState(m_pRasterState_WireFrame_CullNone);
			break;

		case NOISE_CULLMODE_FRONT:
			g_pImmediateContext->RSSetState(m_pRasterState_WireFrame_CullFront);
			break;

		case NOISE_CULLMODE_BACK:
			g_pImmediateContext->RSSetState(m_pRasterState_WireFrame_CullBack);
			break;

		default:
			break;
		}
		break;



	//render points
	case 	NOISE_FILLMODE_POINT:
		g_pImmediateContext->RSSetState(m_pRasterState_WireFrame_CullNone);
		//g_pImmediateContext->RSSetState(m_pRasterState_Solid_CullNone);
		break;

	default:
		break;
	}
}

void		NoiseRenderer::mFunction_SetBlendState(NOISE_BLENDMODE iBlendMode)
{
	float tmpBlendFactor[4] = { 0,0,0,0 };
	switch (m_BlendMode)
	{
	case NOISE_BLENDMODE_OPAQUE:
		g_pImmediateContext->OMSetBlendState(m_pBlendState_Opaque, tmpBlendFactor, 0xffffffff);
		break;

	case NOISE_BLENDMODE_ADDITIVE:
		g_pImmediateContext->OMSetBlendState(m_pBlendState_ColorAdd, tmpBlendFactor, 0xffffffff);
		break;

	case NOISE_BLENDMODE_ALPHA:
		g_pImmediateContext->OMSetBlendState(m_pBlendState_AlphaTransparency, tmpBlendFactor, 0xffffffff);
		break;

	case NOISE_BLENDMODE_COLORFILTER:
		g_pImmediateContext->OMSetBlendState(m_pBlendState_ColorMultiply, tmpBlendFactor, 0xffffffff);
		break;

	}
}

void		NoiseRenderer::mFunction_CameraMatrix_Update()
{
	if (mCanUpdateCbCameraMatrix)
	{
		//update proj matrix
		tmp_pCamera->mFunction_UpdateProjMatrix();
		m_CbCameraMatrix.mProjMatrix = *(tmp_pCamera->m_pMatrixProjection);

		// update view matrix
		tmp_pCamera->mFunction_UpdateViewMatrix();
		m_CbCameraMatrix.mViewMatrix = *(tmp_pCamera->m_pMatrixView);

		//——————更新到GPU——————
		m_pFX_CbSolid3D->SetRawValue(&m_CbCameraMatrix, 0, sizeof(m_CbCameraMatrix));

		//..........
		mCanUpdateCbCameraMatrix = FALSE;
	}
};


void		NoiseRenderer::mFunction_RenderMeshInList_UpdateCbRarely()
{
	
	BOOL tmpCanUpdateCbRarely = FALSE;

	//————更新Static Light——————
	NoiseLightManager* tmpLightMgr = m_pFatherScene->m_pChildLightMgr;

	if((tmpLightMgr != NULL)&& (tmpLightMgr->mCanUpdateStaticLights))
	{
		int tmpLight_Dir_Count = tmpLightMgr->m_pLightList_Dir_Static->size();
		int tmpLight_Point_Count=tmpLightMgr->m_pLightList_Point_Static->size();
		int tmpLight_Spot_Count=tmpLightMgr->m_pLightList_Spot_Static->size();

		m_CbRarely.mIsLightingEnabled_Static			=		tmpLightMgr->mIsDynamicLightingEnabled;
		m_CbRarely.mDirLightCount_Static				=		tmpLight_Dir_Count;
		m_CbRarely.mPointLightCount_Static			=		tmpLight_Point_Count;
		m_CbRarely.mSpotLightCount_Static				=		tmpLight_Spot_Count;

		int i =0;

		for(i=0; i<(tmpLight_Dir_Count); i++)
		{m_CbRarely.mDirectionalLight_Static[i]	=	(tmpLightMgr->m_pLightList_Dir_Static->at(i));}

		for(i=0; i<(tmpLight_Point_Count); i++)
		{m_CbRarely.mPointLight_Static[i]			=	(tmpLightMgr->m_pLightList_Point_Static->at(i));}

		for(i=0; i<(tmpLight_Spot_Count); i++)
		{m_CbRarely.mSpotLight_Static[i]			=	(tmpLightMgr->m_pLightList_Spot_Static->at(i));}

		//更新 “可更新”状态，保证static light 只进行初始化
		tmpLightMgr->mCanUpdateStaticLights = FALSE;
	}


	//——————更新到GPU——————
	if(tmpCanUpdateCbRarely == TRUE)
	{
		m_pFX_CbRarely->SetRawValue(&m_CbRarely,0,sizeof(m_CbRarely));
	};
};

void		NoiseRenderer::mFunction_RenderMeshInList_UpdateCbPerFrame()
{



	//————更新Dynamic Light————
	NoiseLightManager* tmpLightMgr = m_pFatherScene->m_pChildLightMgr;
	if(tmpLightMgr != NULL)
	{
		int tmpLight_Dir_Count = tmpLightMgr->m_pLightList_Dir_Dynamic->size();
		int tmpLight_Point_Count=tmpLightMgr->m_pLightList_Point_Dynamic->size();
		int tmpLight_Spot_Count=tmpLightMgr->m_pLightList_Spot_Dynamic->size();

		m_CbPerFrame.mIsLightingEnabled_Dynamic			=		tmpLightMgr->mIsDynamicLightingEnabled;
		m_CbPerFrame.mDirLightCount_Dynamic					=		tmpLight_Dir_Count;
		m_CbPerFrame.mPointLightCount_Dynamic				=		tmpLight_Point_Count;
		m_CbPerFrame.mSpotLightCount_Dynamic				=		tmpLight_Spot_Count;
		m_CbPerFrame.mCamPos											=		*(tmp_pCamera->m_pPosition);

		int i =0;

		for(i=0; i<(tmpLight_Dir_Count); i++)
		{m_CbPerFrame.mDirectionalLight_Dynamic[i]	=	*(tmpLightMgr->m_pLightList_Dir_Dynamic->at(i));}

		for(i=0; i<(tmpLight_Point_Count); i++)
		{m_CbPerFrame.mPointLight_Dynamic[i]			=	*(tmpLightMgr->m_pLightList_Point_Dynamic->at(i));}

		for(i=0; i<(tmpLight_Spot_Count); i++)
		{m_CbPerFrame.mSpotLight_Dynamic[i]			=	*(tmpLightMgr->m_pLightList_Spot_Dynamic->at(i));}

	}


	//————更新到GPU——————
	m_pFX_CbPerFrame->SetRawValue(&m_CbPerFrame,0,sizeof(m_CbPerFrame));
};

void		NoiseRenderer::mFunction_RenderMeshInList_UpdateCbPerSubset(UINT subsetID)
{
		//we dont accept invalid material ,but accept invalid texture
		UINT	 currSubsetMatID = tmp_pMesh->m_pSubsetInfoList->at(subsetID).matID;
		currSubsetMatID = mFunction_ValidateMaterialID_UsingMatMgr(currSubsetMatID);

		//otherwise if the material is valid
		//then we should check if its child textureS are valid too 
		N_Material tmpMat = m_pFatherScene->m_pChildMaterialMgr->m_pMaterialList->at(currSubsetMatID);
		ID3D11ShaderResourceView* tmp_pSRV = nullptr;
		m_CbPerSubset.basicMaterial = tmpMat.baseMaterial;

		//first validate if ID is valid (within range / valid ID) valid== return original texID
		m_CbPerSubset.IsDiffuseMapValid = (mFunction_ValidateTextureID_UsingTexMgr(tmpMat.diffuseMapID,NOISE_TEXTURE_TYPE_COMMON) 
			== NOISE_MACRO_INVALID_TEXTURE_ID ? FALSE : TRUE);
		m_CbPerSubset.IsNormalMapValid = (mFunction_ValidateTextureID_UsingTexMgr(tmpMat.normalMapID, NOISE_TEXTURE_TYPE_COMMON) 
			== NOISE_MACRO_INVALID_TEXTURE_ID ? FALSE : TRUE);
		m_CbPerSubset.IsSpecularMapValid	= (mFunction_ValidateTextureID_UsingTexMgr(tmpMat.specularMapID, NOISE_TEXTURE_TYPE_COMMON) 
			== NOISE_MACRO_INVALID_TEXTURE_ID ? FALSE : TRUE);
		m_CbPerSubset.IsEnvironmentMapValid = (mFunction_ValidateTextureID_UsingTexMgr(tmpMat.cubeMap_environmentMapID, NOISE_TEXTURE_TYPE_CUBEMAP)
			== NOISE_MACRO_INVALID_TEXTURE_ID ? FALSE : TRUE);


		//update textures, bound corresponding ShaderResourceView to the pipeline
		//if tetxure is  valid ,then set diffuse map
		if (m_CbPerSubset.IsDiffuseMapValid)
		{
			tmp_pSRV = m_pFatherScene->m_pChildTextureMgr->m_pTextureObjectList->at(tmpMat.diffuseMapID).m_pSRV;
			m_pFX_Texture_Diffuse->SetResource(tmp_pSRV);
		}

		//if tetxure is  valid ,then set normal map
		if (m_CbPerSubset.IsNormalMapValid)
		{
			tmp_pSRV = m_pFatherScene->m_pChildTextureMgr->m_pTextureObjectList->at(tmpMat.normalMapID).m_pSRV;
			m_pFX_Texture_Normal->SetResource(tmp_pSRV);
		}

		//if tetxure is  valid ,then set specular map
		if (m_CbPerSubset.IsSpecularMapValid)
		{
			tmp_pSRV = m_pFatherScene->m_pChildTextureMgr->m_pTextureObjectList->at(tmpMat.specularMapID).m_pSRV;
			m_pFX_Texture_Specular->SetResource(tmp_pSRV);
		}

		//if tetxure is  valid ,then set environment map (cube map)
		if (m_CbPerSubset.IsEnvironmentMapValid)
		{
			tmp_pSRV = m_pFatherScene->m_pChildTextureMgr->m_pTextureObjectList->at(tmpMat.cubeMap_environmentMapID).m_pSRV;
			m_pFX_Texture_CubeMap->SetResource(tmp_pSRV);//environment map is a cube map
		}
		

		//transmit all data to gpu
		m_pFX_CbPerSubset->SetRawValue(&m_CbPerSubset, 0, sizeof(m_CbPerSubset));
	
};

void		NoiseRenderer::mFunction_RenderMeshInList_UpdateCbPerObject()
{
	//————更新World Matrix————
	tmp_pMesh->mFunction_UpdateWorldMatrix();
	m_CbPerObject.mWorldMatrix =	*(tmp_pMesh->m_pMatrixWorld);
	m_CbPerObject.mWorldInvTransposeMatrix = *(tmp_pMesh->m_pMatrixWorldInvTranspose);

	//——————更新到GPU——————
	m_pFX_CbPerObject->SetRawValue(&m_CbPerObject,0,sizeof(m_CbPerObject));
};


void		NoiseRenderer::mFunction_GraphicObj_AddToRenderList(NoiseGraphicObject* pGraphicObj, std::vector<NoiseGraphicObject*>* pList)
{
	pList->push_back(pGraphicObj);

	//Update Data to GPU if data is not up to date , 5 object types for now
	for (UINT i = 0;i < NOISE_GRAPHIC_OBJECT_BUFFER_COUNT;i++)
	{
		if (pGraphicObj->mCanUpdateToGpu[i])
		{
			pGraphicObj->mFunction_UpdateVerticesToGpu(i);
			pGraphicObj->mCanUpdateToGpu[i] = FALSE;
			// rectangle buffer must generate a subset list
			if (i == NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D)pGraphicObj->mFunction_GenerateRectSubsetInfo();
		}
	}
}

void		NoiseRenderer::mFunction_GraphicObj_Update_RenderTextured2D(UINT TexID)
{
	//Get Shader Resource View
	ID3D11ShaderResourceView* tmp_pSRV = NULL;

	//......
	TexID = mFunction_ValidateTextureID_UsingTexMgr(TexID,NOISE_TEXTURE_TYPE_COMMON);

	if (TexID != NOISE_MACRO_INVALID_TEXTURE_ID)
	{
		tmp_pSRV = m_pFatherScene->m_pChildTextureMgr->m_pTextureObjectList->at(TexID).m_pSRV;
		m_pFX2D_Texture_Diffuse->SetResource(tmp_pSRV);
	}
}

void		NoiseRenderer::mFunction_GraphicObj_RenderLine3DInList(std::vector<NoiseGraphicObject*>* pList)
{
	tmp_pCamera = m_pFatherScene->GetCamera();

	//更新ConstantBuffer : Proj / View Matrix
	mFunction_CameraMatrix_Update();

	//更新完cb就可以开始draw了
	ID3D11Buffer* tmp_pVB = NULL;
	for (UINT i = 0;i < pList->size();i++)
	{
		UINT vCount = pList->at(i)->m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D]->size();
		if (vCount == 0)continue;

		//settings
		tmp_pVB = pList->at(i)->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &VBstride_Simple, &VBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		//设置fillmode和cullmode
		mFunction_SetRasterState(NOISE_FILLMODE_WIREFRAME, NOISE_CULLMODE_NONE);

		//draw line 一个pass就够了
		m_pFX_Tech_Solid3D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		if(vCount>0)g_pImmediateContext->Draw(vCount, 0);
	}


}

void		NoiseRenderer::mFunction_GraphicObj_RenderPoint3DInList(std::vector<NoiseGraphicObject*>* pList)
{
	tmp_pCamera = m_pFatherScene->GetCamera();

	//update view/proj matrix
	mFunction_CameraMatrix_Update();

	//更新完cb就可以开始draw了
	ID3D11Buffer* tmp_pVB = NULL;
	for (UINT i = 0;i < pList->size();i++)
	{
		UINT vCount = pList->at(i)->m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D]->size();
		if (vCount == 0)continue;

		//settings
		tmp_pVB = pList->at(i)->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &VBstride_Simple, &VBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		//设置fillmode和cullmode
		mFunction_SetRasterState(NOISE_FILLMODE_POINT, NOISE_CULLMODE_NONE);

		//draw line 一个pass就够了
		m_pFX_Tech_Solid3D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		if (vCount>0)g_pImmediateContext->Draw(vCount, 0);
	}

}

void		NoiseRenderer::mFunction_GraphicObj_RenderLine2DInList(std::vector<NoiseGraphicObject*>* pList)
{

	//prepare to draw , various settings.....
	ID3D11Buffer* tmp_pVB = NULL;
	for (UINT i = 0;i < pList->size();i++)
	{
		UINT vCount = pList->at(i)->m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D]->size();
		if (vCount == 0)continue;

		//settings
		tmp_pVB = pList->at(i)->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &VBstride_Simple, &VBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		//设置fillmode和cullmode
		mFunction_SetRasterState(NOISE_FILLMODE_WIREFRAME, NOISE_CULLMODE_NONE);

		//draw line 一个pass就够了
		m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		if (vCount>0)g_pImmediateContext->Draw(vCount, 0);
	}

};

void		NoiseRenderer::mFunction_GraphicObj_RenderPoint2DInList(std::vector<NoiseGraphicObject*>* pList)
{
	//prepare to draw , various settings.....
	ID3D11Buffer* tmp_pVB = NULL;
	for (UINT i = 0;i < pList->size();i++)
	{
		UINT vCount = pList->at(i)->m_pVB_Mem[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D]->size();
		if (vCount == 0)continue;
		
		//settings
		tmp_pVB = pList->at(i)->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &VBstride_Simple, &VBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		//设置fillmode和cullmode
		mFunction_SetRasterState(NOISE_FILLMODE_POINT, NOISE_CULLMODE_NONE);

		//draw line 一个pass就够了
		m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		if (vCount>0)g_pImmediateContext->Draw(vCount, 0);
	}

};

void		NoiseRenderer::mFunction_GraphicObj_RenderTriangle2DInList(std::vector<NoiseGraphicObject*>* pList)
{
	//prepare to draw , various settings.....
	ID3D11Buffer* tmp_pVB = NULL;


	for (UINT i = 0;i < pList->size();i++)
	{

		//----------------------1,draw common triangle----------------------
		tmp_pVB = pList->at(i)->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &VBstride_Simple, &VBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT j = 0, vCount = 0;
		//traverse all region list , to decide use which tech to draw (textured or not)

		m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		vCount = pList->at(i)->GetTriangle2DCount() * 3;
		if (vCount>0)g_pImmediateContext->Draw(pList->at(i)->GetTriangle2DCount()*3,0);
		

		//---------------------2,draw rectangles---------------------
		tmp_pVB = pList->at(i)->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &VBstride_Simple, &VBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (auto tmpRegion : *(pList->at(i)->m_pRectSubsetInfoList))
		{
			//if current Rectangle disable Texture ,then draw in a solid way
			if (tmpRegion.texID == NOISE_MACRO_INVALID_TEXTURE_ID)
			{
				m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
			}
			else
			{
				//------Draw 2D Common Texture-----
				mFunction_GraphicObj_Update_RenderTextured2D(tmpRegion.texID);
				m_pFX_Tech_Textured2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
			}

			//draw 
			vCount= tmpRegion.vertexCount;
			if(vCount>0)g_pImmediateContext->Draw(tmpRegion.vertexCount, tmpRegion.startID);
		}

	}

	//清空渲染列表
	pList->clear();
}

void		NoiseRenderer::mFunction_TextGraphicObj_Update_TextInfo(UINT texID, NoiseTextureManager* pTexMgr, N_CbDrawText2D& cbText)
{
	//Get Shader Resource View
	ID3D11ShaderResourceView* tmp_pSRV = NULL;

	//......
	//texID = mFunction_ValidateTextureID_UsingTexMgr(texID, NOISE_TEXTURE_TYPE_TEXT,NOISE_TEXTURE_ACCESS_PERMISSION_FONTMGR);

	if (texID != NOISE_MACRO_INVALID_TEXTURE_ID)
	{
		HRESULT hr = S_OK;
		m_CbDrawText2D = (N_CbDrawText2D)cbText;
		//update Constant buffer
		m_pFX_CbDrawText2D->SetRawValue(&m_CbDrawText2D, 0, sizeof(m_CbDrawText2D));

		//update textures
		tmp_pSRV = pTexMgr->m_pTextureObjectList->at(texID).m_pSRV;
		m_pFX2D_Texture_Diffuse->SetResource(tmp_pSRV);
	}
}

void		NoiseRenderer::mFunction_TextGraphicObj_RenderTriangles()
{
	//prepare to draw , various settings.....
	ID3D11Buffer* tmp_pVB = NULL;

	//I didn't use template because this function is type-dependent
	//i dont want it to be type-unsafe

	for (UINT i = 0;i < m_pRenderList_TextStatic->size();i++)
	{
		tmp_pVB = m_pRenderList_TextStatic->at(i)->m_pGraphicObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &VBstride_Simple, &VBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//设置fillmode和cullmode
		mFunction_SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_NONE);

		UINT j = 0, vCount = 0;
		//traverse all region list , to decide use which tech to draw (textured or not)

		//---------------------draw rectangles---------------------
		for (auto tmpRegion : *(m_pRenderList_TextStatic->at(i)->m_pGraphicObj->m_pRectSubsetInfoList))
		{
			//if current Rectangle disable Texture ,then draw in a solid way
			if (tmpRegion.texID == NOISE_MACRO_INVALID_TEXTURE_ID)
			{
				m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
			}
			else
			{
				//fill cb struct......
				N_CbDrawText2D tmpCbText;
				ZeroMemory(&tmpCbText, sizeof(tmpCbText));
				tmpCbText.mTextColor = *(m_pRenderList_TextStatic->at(i)->m_pTextColor);
				tmpCbText.mTextGlowColor = *(m_pRenderList_TextStatic->at(i)->m_pTextGlowColor);

				//update colors of text(cb & srv)
				mFunction_TextGraphicObj_Update_TextInfo(tmpRegion.texID, m_pRenderList_TextStatic->at(i)->m_pFatherFontMgr->m_pTexMgr, tmpCbText);
				m_pFX_Tech_DrawText2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
			}

			//draw 
			vCount = tmpRegion.vertexCount;
			if (vCount>0)g_pImmediateContext->Draw(tmpRegion.vertexCount , tmpRegion.startID);
		}

	}
	
	for (UINT i = 0;i < m_pRenderList_TextDynamic->size();i++)
	{
		tmp_pVB = m_pRenderList_TextDynamic->at(i)->m_pGraphicObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &VBstride_Simple, &VBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//设置fillmode和cullmode
		mFunction_SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_NONE);

		UINT j = 0, vCount = 0;
		//traverse all region list , to decide use which tech to draw (textured or not)

		//---------------------draw rectangles---------------------
		for (auto tmpRegion : *(m_pRenderList_TextDynamic->at(i)->m_pGraphicObj->m_pRectSubsetInfoList))
		{
			//if current Rectangle disable Texture ,then draw in a solid way
			if (tmpRegion.texID == NOISE_MACRO_INVALID_TEXTURE_ID)
			{
				m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
			}
			else
			{
				//fill cb struct......
				N_CbDrawText2D tmpCbText;
				ZeroMemory(&tmpCbText, sizeof(tmpCbText));
				tmpCbText.mTextColor = *(m_pRenderList_TextDynamic->at(i)->m_pTextColor);
				tmpCbText.mTextGlowColor = *(m_pRenderList_TextDynamic->at(i)->m_pTextGlowColor);

				//update colors of text(cb & srv)
				mFunction_TextGraphicObj_Update_TextInfo(tmpRegion.texID, m_pRenderList_TextDynamic->at(i)->m_pFatherFontMgr->m_pTexMgr, tmpCbText);
				m_pFX_Tech_DrawText2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
			}

			//draw 
			vCount = tmpRegion.vertexCount;
			if (vCount>0)g_pImmediateContext->Draw(tmpRegion.vertexCount, tmpRegion.startID);
		}

	}

	//clear render list
	m_pRenderList_TextStatic->clear();
	m_pRenderList_TextDynamic->clear();
};

void		NoiseRenderer::mFunction_Atmosphere_Fog_Update()
{
	if (tmp_pAtmo->mFogCanUpdateToGpu)
	{
		//update fog param
		m_CbAtmosphere.mFogColor = *(tmp_pAtmo->m_pFogColor);
		m_CbAtmosphere.mFogFar = tmp_pAtmo->mFogFar;
		m_CbAtmosphere.mFogNear = tmp_pAtmo->mFogNear;
		m_CbAtmosphere.mIsFogEnabled = (BOOL)(tmp_pAtmo->mFogEnabled && tmp_pAtmo->mFogHasBeenAddedToRenderList);

		//udpate to GPU
		m_pFX_CbAtmosphere->SetRawValue(&m_CbAtmosphere, 0, sizeof(m_CbAtmosphere));
		tmp_pAtmo->mFogCanUpdateToGpu = FALSE;
	}
};

void		NoiseRenderer::mFunction_Atmosphere_SkyDome_Update()
{
	//validate texture and update BOOL value to gpu
	UINT skyDomeTexID = tmp_pAtmo->mSkyDomeTextureID;

	//check skyType
	if(tmp_pAtmo->mSkyType == NOISE_ATMOSPHERE_SKYTYPE_DOME)
	{
		//if texture pass ID validation and match current-set skytype
		m_CbAtmosphere.mIsSkyDomeValid = (mFunction_ValidateTextureID_UsingTexMgr(skyDomeTexID,NOISE_TEXTURE_TYPE_COMMON) == NOISE_MACRO_INVALID_TEXTURE_ID ? FALSE : TRUE);
	}
	else
	{
		m_CbAtmosphere.mIsSkyDomeValid = FALSE;
	}

};

void		NoiseRenderer::mFunction_Atmosphere_SkyBox_Update()
{
	//skybox uses cube map to texture the box
	UINT skyboxTexID = tmp_pAtmo->mSkyBoxCubeTextureID;

	//check skyType
	if (tmp_pAtmo->mSkyType == NOISE_ATMOSPHERE_SKYTYPE_BOX)
	{
		//skybox texture must be a cube map
		m_CbAtmosphere.mIsSkyBoxValid		= (mFunction_ValidateTextureID_UsingTexMgr(skyboxTexID, NOISE_TEXTURE_TYPE_CUBEMAP) == NOISE_MACRO_INVALID_TEXTURE_ID ? FALSE : TRUE);
		m_CbAtmosphere.mSkyBoxWidth		= tmp_pAtmo->mSkyBoxWidth;
		m_CbAtmosphere.mSkyBoxHeight	= tmp_pAtmo->mSkyBoxHeight;
		m_CbAtmosphere.mSkyBoxDepth		= tmp_pAtmo->mSkyBoxDepth;
		/*if (!m_CbAtmosphere.mIsSkyBoxValid)
		{
			DEBUG_MSG1("Noise Atmosphere Skybox : Texture Invalid!");
		}*/
	}
	else
	{
		m_CbAtmosphere.mIsSkyBoxValid = FALSE;
	}

};

void		NoiseRenderer::mFunction_Atmosphere_UpdateCbAtmosphere()
{
	//update valid texture to gpu
	if (m_CbAtmosphere.mIsSkyDomeValid)
	{
		//tmp_pAtmo->mSkyDomeTextureID has been validated in UPDATE function
		auto tmp_pSRV = m_pFatherScene->m_pChildTextureMgr->m_pTextureObjectList->at(tmp_pAtmo->mSkyDomeTextureID).m_pSRV;
		m_pFX_Texture_Diffuse->SetResource(tmp_pSRV);
	}


	//update skybox cube map to gpu
	if (m_CbAtmosphere.mIsSkyBoxValid)
	{
		//tmp_pAtmo->mSkyBoxTextureID has been validated  in UPDATE function
		//but how do you validate it's a valid cube map ?????
		auto tmp_pSRV = m_pFatherScene->m_pChildTextureMgr->m_pTextureObjectList->at(tmp_pAtmo->mSkyBoxCubeTextureID).m_pSRV;
		m_pFX_Texture_CubeMap->SetResource(tmp_pSRV);
	}


	m_pFX_CbAtmosphere->SetRawValue(&m_CbAtmosphere, 0, sizeof(m_CbAtmosphere));
};


inline UINT	NoiseRenderer::mFunction_ValidateTextureID_UsingTexMgr(UINT texID, NOISE_TEXTURE_TYPE texType)
{
	//tex mgr had been validated
	NoiseTextureManager*		tmpTexMgr = m_pFatherScene->m_pChildTextureMgr;

	//invoke validation function in Tex Mgr
	UINT outTexID = tmpTexMgr->mFunction_ValidateTextureID(texID, texType);

	return outTexID;
}

inline UINT	NoiseRenderer::mFunction_ValidateMaterialID_UsingMatMgr(UINT matID)
{
	//mat mgr had been validated
	NoiseMaterialManager*	tmpMatMgr = m_pFatherScene->m_pChildMaterialMgr;

	//if matID is out of range, a default mateial should be returned
	if (matID >= tmpMatMgr->m_pMaterialList->size())
	{
		return NOISE_MACRO_DEFAULT_MATERIAL_ID;
	}

	//a no-problem texID
	return matID;
}
