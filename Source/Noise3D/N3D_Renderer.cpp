
/***********************************************************************

                           类：NOISE Renderer

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

static UINT c_VBstride_Default = sizeof(N_DefaultVertex);		//VertexBuffer的每个元素的字节跨度
static UINT c_VBstride_Simple = sizeof(N_SimpleVertex);
static UINT c_VBoffset = 0;				//VertexBuffer顶点序号偏移 因为从头开始所以offset是0
static IAtmosphere*						tmp_pAtmo;
static D3DX11_TECHNIQUE_DESC	tmp_pTechDesc;


IRenderer::IRenderer()
{
	m_pFatherScene								= nullptr;
	mCanUpdateCbCameraMatrix			= FALSE;
	m_pRenderList_Mesh						= new std::vector <IMesh*>;
	m_pRenderList_CommonGraphicObj	= new std::vector<IGraphicObject*>;
	m_pRenderList_TextDynamic				= new std::vector<IBasicTextInfo*>;//for Text Rendering
	m_pRenderList_TextStatic					= new std::vector<IBasicTextInfo*>;//for Text Rendering
	m_pRenderList_GUIText						= new std::vector<IBasicTextInfo*>;//for GUI Text Rendering
	m_pRenderList_GUIGraphicObj			= new std::vector<IGraphicObject*>;//for GUI common object rendering
	m_pRenderList_Atmosphere				= new std::vector<IAtmosphere*>;
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

void IRenderer::Destroy()
{

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
	ReleaseCOM(m_pSamplerState_FilterAnis);
	ReleaseCOM(m_pFX);
};

void IRenderer::AddObjectToRenderList(IMesh& obj)
{
	m_pRenderList_Mesh->push_back(&obj);
};

void IRenderer::AddObjectToRenderList(IAtmosphere& obj)
{
	m_pRenderList_Atmosphere->push_back(&obj);
	//fog color will only be rendered after ADDTORENDERLIST();
	obj.mFogHasBeenAddedToRenderList = TRUE;
};

void IRenderer::AddObjectToRenderList(IGraphicObject& obj, NOISE_RENDERER_ADDTOLIST_OBJ_TYPE objType)
{
	if (objType == NOISE_RENDERER_ADDTOLIST_OBJ_TYPE_COMMON_OBJECT)
	{
		mFunction_AddToRenderList_GraphicObj(&obj, m_pRenderList_CommonGraphicObj);
	}
	else
	{
		mFunction_AddToRenderList_GraphicObj(&obj, m_pRenderList_GUIGraphicObj);
	}
}

void IRenderer::AddObjectToRenderList(IDynamicText & obj, NOISE_RENDERER_ADDTOLIST_OBJ_TYPE objType)
{
	if (objType == NOISE_RENDERER_ADDTOLIST_OBJ_TYPE_COMMON_OBJECT)
	{
		mFunction_AddToRenderList_Text(&obj, m_pRenderList_TextDynamic);
	}
	else
	{
		mFunction_AddToRenderList_Text(&obj, m_pRenderList_GUIText);
	}
}

void IRenderer::AddObjectToRenderList(IStaticText & obj, NOISE_RENDERER_ADDTOLIST_OBJ_TYPE objType)
{
	if (objType == NOISE_RENDERER_ADDTOLIST_OBJ_TYPE_COMMON_OBJECT)
	{
		mFunction_AddToRenderList_Text(&obj, m_pRenderList_TextStatic);
	}
	else
	{
		mFunction_AddToRenderList_Text(&obj, m_pRenderList_GUIText);
	}
};

void	IRenderer::ClearBackground(NVECTOR4 color)
{
	float ClearColor[4] = { color.x,color.y,color.z,color.w };
	g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, ClearColor );
	//我擦！！！纠结这么久原来是要clearDepth!!!!!!
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView,
		D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,1.0f,0);
};

void	IRenderer::RenderToScreen()
{
		g_pSwapChain->Present(0, 0 );

		//reset some state
		mCanUpdateCbCameraMatrix = TRUE;

		//clear render list
		m_pRenderList_CommonGraphicObj->clear();
		m_pRenderList_Mesh->clear();
		m_pRenderList_Atmosphere->clear();
};

void IRenderer::SetFillMode(NOISE_FILLMODE iMode)
{
	m_FillMode = iMode;
}

void IRenderer::SetCullMode(NOISE_CULLMODE iMode)
{
	m_CullMode = iMode;
}

void IRenderer::SetBlendingMode(NOISE_BLENDMODE iMode)
{
	m_BlendMode = iMode;
};



/************************************************************************
                                            PRIVATE                        
************************************************************************/
BOOL	IRenderer::mFunction_Init()
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
	//ReleaseCOM(g_pd3dDevice11);
	HR_DEBUG(hr, "创建input Layout失败！");

	//simple vertex input layout
	m_pFX_Tech_Solid3D->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = g_pd3dDevice11->CreateInputLayout(
		&g_VertexDesc_Simple[0],
		g_VertexDesc_Simple_ElementCount,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&g_pVertexLayout_Simple);
	//ReleaseCOM(g_pd3dDevice11);
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

	//ReleaseCOM(g_pd3dDevice11);

	return TRUE;
}

BOOL	IRenderer::mFunction_Init_CreateBlendState()
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
	//ReleaseCOM(g_pd3dDevice11);
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
	//ReleaseCOM(g_pd3dDevice11);
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
	//ReleaseCOM(g_pd3dDevice11);
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
	//ReleaseCOM(g_pd3dDevice11);
	HR_DEBUG(hr, "Create blend state(Transparency) failed!!");

	return TRUE;
};

BOOL	IRenderer::mFunction_Init_CreateRasterState()
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
	//ReleaseCOM(g_pd3dDevice11);
	HR_DEBUG(hr, "Create m_pRasterState_Solid_CullNone failed");

	tmpRasterStateDesc.CullMode = D3D11_CULL_NONE;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = g_pd3dDevice11->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_WireFrame_CullNone);
	//ReleaseCOM(g_pd3dDevice11);
	HR_DEBUG(hr, "Create m_pRasterState_WireFrame_CullNone failed");

	tmpRasterStateDesc.CullMode = D3D11_CULL_BACK;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_SOLID;
	hr = g_pd3dDevice11->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_Solid_CullBack);
	//ReleaseCOM(g_pd3dDevice11);
	HR_DEBUG(hr, "Create m_pRasterState_Solid_CullBack failed");

	tmpRasterStateDesc.CullMode = D3D11_CULL_BACK;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = g_pd3dDevice11->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_WireFrame_CullBack);
	//ReleaseCOM(g_pd3dDevice11);
	HR_DEBUG(hr, "Create m_pRasterState_WireFrame_CullBack failed");

	tmpRasterStateDesc.CullMode = D3D11_CULL_FRONT;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_SOLID;
	hr = g_pd3dDevice11->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_Solid_CullFront);
	//ReleaseCOM(g_pd3dDevice11);
	HR_DEBUG(hr, "Create m_pRasterState_Solid_CullFront failed");

	tmpRasterStateDesc.CullMode = D3D11_CULL_FRONT;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = g_pd3dDevice11->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_WireFrame_CullFront);
	//ReleaseCOM(g_pd3dDevice11);
	HR_DEBUG(hr, "Createm_pRasterState_WireFrame_CullFront failed");

	return TRUE;
};

BOOL	IRenderer::mFunction_Init_CreateSamplerState()
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
	//ReleaseCOM(g_pd3dDevice11);
	HR_DEBUG(hr, "Create Sampler State failed!!");

	return TRUE;
};

BOOL	IRenderer::mFunction_Init_CreateDepthStencilState()
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
	//ReleaseCOM(g_pd3dDevice11);
	HR_DEBUG(hr, "Create Depth Stencil State Failed!!!");


	ZeroMemory(&dssDesc, sizeof(dssDesc));
	dssDesc.DepthEnable = FALSE;
	dssDesc.StencilEnable = FALSE;
	hr = g_pd3dDevice11->CreateDepthStencilState(&dssDesc, &m_pDepthStencilState_DisableDepthTest);
	//ReleaseCOM(g_pd3dDevice11);
	HR_DEBUG(hr, "Create Depth Stencil State Failed!!!");

	return TRUE;
};

BOOL	IRenderer::mFunction_Init_CreateEffectFromFile(NFilePath fxPath)
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
	hr = D3DX11CompileFromFileA(
		fxPath.c_str(),0,0,0,"fx_5_0",
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

	return TRUE;
};

BOOL	IRenderer::mFunction_Init_CreateEffectFromMemory(char* compiledShaderPath)
{
	std::vector<char> compiledShader;

	//加载fxo文件
	if (!IFileManager::ImportFile_PURE(compiledShaderPath, compiledShader))
	{
		return FALSE; 
	}

	//创建fx特效框架
	HRESULT hr = S_OK;
	hr = D3DX11CreateEffectFromMemory(&compiledShader[0], compiledShader.size(), 0, g_pd3dDevice11, &m_pFX);
	HR_DEBUG(hr,"load compiled shader failed");

	return TRUE;
}



void		IRenderer::mFunction_SetRasterState(NOISE_FILLMODE iFillMode, NOISE_CULLMODE iCullMode)
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

void		IRenderer::mFunction_SetBlendState(NOISE_BLENDMODE iBlendMode)
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

inline void	 IRenderer::mFunction_CameraMatrix_Update(ICamera* const pCamera)
{
	if (mCanUpdateCbCameraMatrix)
	{
		//update proj matrix
		pCamera->mFunction_UpdateProjMatrix();
		m_CbCameraMatrix.mProjMatrix = *(pCamera->m_pMatrixProjection);

		// update view matrix
		pCamera->mFunction_UpdateViewMatrix();
		m_CbCameraMatrix.mViewMatrix = *(pCamera->m_pMatrixView);

		//——————更新到GPU——————
		m_pFX_CbSolid3D->SetRawValue(&m_CbCameraMatrix, 0, sizeof(m_CbCameraMatrix));

		//..........
		mCanUpdateCbCameraMatrix = FALSE;
	}
};

void		IRenderer::mFunction_AddToRenderList_GraphicObj(IGraphicObject* pGraphicObj, std::vector<IGraphicObject*>* pList)
{
	pList->push_back(pGraphicObj);

	//Update Data to GPU if data is not up to date , 5 object types for now
	for (UINT i = 0;i < NOISE_GRAPHIC_OBJECT_BUFFER_COUNT;i++)
	{
		if (pGraphicObj->mCanUpdateToGpu[i]==TRUE)
		{
			pGraphicObj->mFunction_UpdateVerticesToGpu(i);
			pGraphicObj->mCanUpdateToGpu[i] = FALSE;
			// rectangle buffer must generate a subset list
			if (i == NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D)pGraphicObj->mFunction_GenerateRectSubsetInfo();
		}
	}
}

void		IRenderer::mFunction_AddToRenderList_Text(IBasicTextInfo * pText, std::vector<IBasicTextInfo*>* pList)
{
	pText->mFunction_UpdateGraphicObject();//implemented by derived Text Class
	pList->push_back(pText);
	//Update Data to GPU if data is not up to date , 6 object types for now
	for (UINT i = 0;i <NOISE_GRAPHIC_OBJECT_BUFFER_COUNT;i++)
	{
		if (pText->m_pGraphicObj->mCanUpdateToGpu[i])
		{
			pText->m_pGraphicObj->mFunction_UpdateVerticesToGpu(i);
			pText->m_pGraphicObj->mCanUpdateToGpu[i] = FALSE;
			if (i == NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D)pText->m_pGraphicObj->mFunction_GenerateRectSubsetInfo();
		}
	}
}

