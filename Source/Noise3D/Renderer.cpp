
/***********************************************************************

                           类：NOISE Renderer

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

IRenderer::IRenderer()
{
	mCanUpdateCbCameraMatrix			= FALSE;
	m_pRenderList_Mesh							= new std::vector <IMesh*>;
	m_pRenderList_CommonGraphicObj	= new std::vector<IGraphicObject*>;
	m_pRenderList_TextDynamic				= new std::vector<IBasicTextInfo*>;//for Text Rendering
	m_pRenderList_TextStatic					= new std::vector<IBasicTextInfo*>;//for Text Rendering
	m_pRenderList_Atmosphere				= new std::vector<IAtmosphere*>;
	m_pFX_Tech_Default = nullptr;
	m_pFX_Tech_Solid3D = nullptr;
	m_pFX_Tech_Solid2D = nullptr;
	m_pFX_Tech_Textured2D = nullptr;
	m_pFX_Tech_DrawSky = nullptr;
	m_FillMode = NOISE_FILLMODE_SOLID;
	m_CullMode = NOISE_CULLMODE_NONE;
	m_BlendMode = NOISE_BLENDMODE_OPAQUE;
}

IRenderer::~IRenderer()
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
	ReleaseCOM(m_pSamplerState_FilterLinear);
	ReleaseCOM(m_pDepthStencilView);
	ReleaseCOM(m_pRenderTargetView);
	ReleaseCOM(m_pSwapChain);

	delete m_pRenderList_Atmosphere;
	delete m_pRenderList_CommonGraphicObj;
	delete m_pRenderList_Mesh;
	delete m_pRenderList_TextDynamic;
	delete m_pRenderList_TextStatic;
};


void IRenderer::AddObjectToRenderList(IMesh* obj)
{
	m_pRenderList_Mesh->push_back(obj);
};

void IRenderer::AddObjectToRenderList(IAtmosphere* obj)
{
	m_pRenderList_Atmosphere->push_back(obj);
	//fog color will only be rendered after ADDTORENDERLIST();
	obj->mFogHasBeenAddedToRenderList = TRUE;
};

void IRenderer::AddObjectToRenderList(IGraphicObject* obj)
{
	mFunction_AddToRenderList_GraphicObj(obj, m_pRenderList_CommonGraphicObj);
}

void IRenderer::AddObjectToRenderList(IDynamicText* obj)
{
	mFunction_AddToRenderList_Text(obj, m_pRenderList_TextDynamic);
}

void IRenderer::AddObjectToRenderList(IStaticText* obj)
{
	mFunction_AddToRenderList_Text(obj, m_pRenderList_TextStatic);
};

void	IRenderer::ClearBackground(const NVECTOR4& color)
{
	float ClearColor[4] = { color.x,color.y,color.z,color.w };
	g_pImmediateContext->ClearRenderTargetView( m_pRenderTargetView, ClearColor );
	//我擦！！！纠结这么久原来是要clearDepth!!!!!!
	g_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView,
		D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,1.0f,0);
};

void	IRenderer::PresentToScreen()
{
		m_pSwapChain->Present(0, 0 );

		//reset some state
		mCanUpdateCbCameraMatrix = TRUE;

		//clear render list
		m_pRenderList_CommonGraphicObj->clear();
		m_pRenderList_Mesh->clear();
		m_pRenderList_Atmosphere->clear();
		m_pRenderList_TextDynamic->clear();
		m_pRenderList_TextStatic->clear();
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
}

UINT IRenderer::GetMainBufferWidth()
{
	return mMainBufferWidth;
};

UINT IRenderer::GetMainBufferHeight()
{
	return mMainBufferHeight;
};




/************************************************************************
                                            PRIVATE                        
************************************************************************/
BOOL	IRenderer::mFunction_Init(UINT BufferWidth, UINT BufferHeight, BOOL IsWindowed)
{
	//init d3d infrastructure
	if (!mFunction_Init_CreateSwapChainAndRTVandDSVandViewport(BufferWidth, BufferHeight, IsWindowed))
	{
		ERROR_MSG("IRenderer : Init D3D Infrastructure failed.");
		return FALSE;
	};

	HRESULT hr = S_OK;

	//创建Technique
	m_pFX_Tech_Default =	g_pFX->GetTechniqueByName("DefaultDraw");
	m_pFX_Tech_Solid3D =	g_pFX->GetTechniqueByName("DrawSolid3D");
	m_pFX_Tech_Solid2D =	g_pFX->GetTechniqueByName("DrawSolid2D");
	m_pFX_Tech_Textured2D = g_pFX->GetTechniqueByName("DrawTextured2D");
	m_pFX_Tech_DrawSky = g_pFX->GetTechniqueByName("DrawSky");
	m_pFX_Tech_DrawText2D =	g_pFX->GetTechniqueByName("DrawText2D");

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

	HR_DEBUG(hr, "create default input Layout failed！");

	//simple vertex input layout
	m_pFX_Tech_Solid3D->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = g_pd3dDevice11->CreateInputLayout(
		&g_VertexDesc_Simple[0],
		g_VertexDesc_Simple_ElementCount,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&g_pVertexLayout_Simple);

	HR_DEBUG(hr, "create simple input Layout failed！");
#pragma endregion Create Input Layout

#pragma region Create Fx Variable
	//创建Cbuffer
	m_pFX_CbPerFrame=g_pFX->GetConstantBufferByName("cbPerFrame");
	m_pFX_CbPerObject=g_pFX->GetConstantBufferByName("cbPerObject");
	m_pFX_CbPerSubset = g_pFX->GetConstantBufferByName("cbPerSubset");
	m_pFX_CbRarely=g_pFX->GetConstantBufferByName("cbRarely");
	m_pFX_CbSolid3D = g_pFX->GetConstantBufferByName("cbCameraInfo");
	m_pFX_CbAtmosphere = g_pFX->GetConstantBufferByName("cbAtmosphere");
	m_pFX_CbDrawText2D = g_pFX->GetConstantBufferByName("cbDrawText2D");

	//纹理
	m_pFX_Texture_Diffuse = g_pFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	m_pFX_Texture_Normal = g_pFX->GetVariableByName("gNormalMap")->AsShaderResource();
	m_pFX_Texture_Specular = g_pFX->GetVariableByName("gSpecularMap")->AsShaderResource();
	m_pFX_Texture_CubeMap = g_pFX->GetVariableByName("gCubeMap")->AsShaderResource();
	m_pFX2D_Texture_Diffuse = g_pFX->GetVariableByName("g2D_DiffuseMap")->AsShaderResource();

	//sampler state ( it needs a name in FX so for the time being I had to use D3DX11Effect
	m_pFX_SamplerState_Default = g_pFX->GetVariableByName("samplerDefault")->AsSampler();

#pragma endregion Create Fx Variable

	//Create Various kinds of states
	if (!mFunction_Init_CreateRasterState())return FALSE;
	if (!mFunction_Init_CreateBlendState())return FALSE;
	if (!mFunction_Init_CreateSamplerState())return FALSE;
	if (!mFunction_Init_CreateDepthStencilState())return FALSE;

	mMainBufferWidth = BufferWidth;
	mMainBufferHeight = BufferHeight;

	return TRUE;
}

BOOL	IRenderer::mFunction_Init_CreateSwapChainAndRTVandDSVandViewport(UINT BufferWidth, UINT BufferHeight, BOOL IsWindowed)
{
	//check multi-sample capability
	UINT device_MSAA_Quality = 1;//bigger than 1
	UINT device_MSAA_SampleCount = 1;//1 for none,2 for 2xMSAA, 4 ...
	UINT device_MSAA_Enabled = FALSE;

	g_pd3dDevice11->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, device_MSAA_SampleCount, &device_MSAA_Quality);//4x坑锯齿一般都支持，这个返回值一般情况下都大于0
	if (device_MSAA_Quality > 0)
	{
		device_MSAA_Enabled = TRUE;	//4x抗锯齿可以开了
	};

	//-----------------------------------SWAP CHAIN--------------------------------
	//Swap Chain description
	DXGI_SWAP_CHAIN_DESC SwapChainParam;
	ZeroMemory(&SwapChainParam, sizeof(SwapChainParam));
	SwapChainParam.BufferCount = 1;
	SwapChainParam.BufferDesc.Width = BufferWidth;
	SwapChainParam.BufferDesc.Height = BufferHeight;
	SwapChainParam.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainParam.BufferDesc.RefreshRate.Numerator = 60;//	分子= =？
	SwapChainParam.BufferDesc.RefreshRate.Denominator = 1;//分母
	SwapChainParam.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//BACKBUFFER怎么被使用
	SwapChainParam.OutputWindow = GetRoot()->GetRenderWindowHWND();
	SwapChainParam.Windowed = IsWindowed;
	SwapChainParam.SampleDesc.Count = (device_MSAA_Enabled == TRUE ? device_MSAA_SampleCount : 1);//if MSAA enabled, RT/DS buffer must have same quality
	SwapChainParam.SampleDesc.Quality = (device_MSAA_Enabled == TRUE ? device_MSAA_Quality - 1 : 0);//quality之前获取了

	//下面的COM的QueryInterface 用一个接口查询另一个接口
	IDXGIDevice *dxgiDevice = 0;
	g_pd3dDevice11->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	IDXGIAdapter *dxgiAdapter = 0;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	IDXGIFactory *dxgiFactory = 0;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

	//Create Swap chain
	HRESULT hr = dxgiFactory->CreateSwapChain(
	g_pd3dDevice11,		//设备的指针
	&SwapChainParam,	//交换链的描述
	&m_pSwapChain);		//返回的交换链指针
	HR_DEBUG(hr, "SwapChain创建失败！");

	dxgiFactory->Release();
	dxgiDevice->Release();
	dxgiAdapter->Release();

	//------------------------------RTV---------------------
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return FALSE;

	hr = g_pd3dDevice11->CreateRenderTargetView(
		pBackBuffer,
		NULL,					//可以填充一个D3D11_RENDERTARGETVIEW_DESC
		&m_pRenderTargetView);	//返回一个渲染视口

	pBackBuffer->Release();		//已经用完了的临时接口- -


	HR_DEBUG(hr, "创建RENDER TARGET VIEW失败");


	//------------------------------------------DSV--------------------------
	//创建depth/stencil view
	D3D11_TEXTURE2D_DESC DSBufferDesc;
	DSBufferDesc.Width = BufferWidth;
	DSBufferDesc.Height = BufferHeight;
	DSBufferDesc.MipLevels = 1;
	DSBufferDesc.ArraySize = 1;
	DSBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSBufferDesc.SampleDesc.Count = (device_MSAA_Enabled = TRUE ? device_MSAA_SampleCount : 1);//if MSAA enabled, RT/DS buffer must have same quality
	DSBufferDesc.SampleDesc.Quality = (device_MSAA_Enabled = TRUE ? device_MSAA_Quality - 1 : 0);
	DSBufferDesc.Usage = D3D11_USAGE_DEFAULT;	//尽量避免DYNAMIC和STAGING
	DSBufferDesc.CPUAccessFlags = 0;	//CPU不能碰它 GPU才行 这样能够加快
	DSBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;//和PIPELINE的绑定
	DSBufferDesc.MiscFlags = 0;

	ID3D11Texture2D* pDepthStencilBuffer;
	g_pd3dDevice11->CreateTexture2D(&DSBufferDesc, 0, &pDepthStencilBuffer);//创建一个缓冲区
	hr = g_pd3dDevice11->CreateDepthStencilView(
		pDepthStencilBuffer,
		0,
		&m_pDepthStencilView);	//返回一个depth/stencil视口指针

	pDepthStencilBuffer->Release();

	if (FAILED(hr))
	{
		return FALSE;
	};

	//-------------------Set RTV/DSV------------------
	g_pImmediateContext->OMSetRenderTargets(
		1,
		&m_pRenderTargetView,
		m_pDepthStencilView);


	//-------------------------------------------VIEWPORT--------------------------------
	//XY都是-1到1，深度Z是0到1，DX11不会默认创建视口，DX9就会

	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)BufferWidth;		//视口WIDTH 跟后缓冲区一样
	vp.Height = (FLOAT)BufferHeight;	//视口Height
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	//SetViewport 参数1：视口的个数 参数2：视口数组的首地址
	g_pImmediateContext->RSSetViewports(1, &vp);


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
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.MaxAnisotropy = 4;
	hr = g_pd3dDevice11->CreateSamplerState(&samDesc, &m_pSamplerState_FilterLinear);
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
	HR_DEBUG(hr, "Create Depth Stencil State #1 Failed!!!");


	ZeroMemory(&dssDesc, sizeof(dssDesc));
	dssDesc.DepthEnable = FALSE;
	dssDesc.StencilEnable = FALSE;
	hr = g_pd3dDevice11->CreateDepthStencilState(&dssDesc, &m_pDepthStencilState_DisableDepthTest);
	HR_DEBUG(hr, "Create Depth Stencil State #2 Failed!!!");

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
		ERROR_MSG("Noise Renderer : Shader Compilation Failed !!");
		ReleaseCOM(compilationMsg);
	}


	hr = D3DX11CreateEffectFromMemory(
		compiledFX->GetBufferPointer(),
		compiledFX->GetBufferSize(),
		0,g_pd3dDevice11,&g_pFX,nullptr);
	HR_DEBUG(hr,"Create Basic Effect Fail!");

	return TRUE;
};


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

void	 IRenderer::mFunction_CameraMatrix_Update(ICamera* const pCamera)
{
	if (mCanUpdateCbCameraMatrix)
	{
		//update camera matrices
		pCamera->GetProjMatrix(m_CbCameraInfo.projMatrix);

		pCamera->GetViewMatrix(m_CbCameraInfo.viewMatrix);

		pCamera->GetInvProjMatrix(m_CbCameraInfo.invViewMatrix);

		pCamera->GetInvViewMatrix(m_CbCameraInfo.invProjMatrix);

		m_CbCameraInfo.camPos = pCamera->GetPosition();


		//——————更新到GPU——————
		m_pFX_CbSolid3D->SetRawValue(&m_CbCameraInfo, 0, sizeof(m_CbCameraInfo));

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

