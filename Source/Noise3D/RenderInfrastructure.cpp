
/***********************************************************************

							IRenderInfracstructure
		encapsulation of common D3D operation/states
		init once by IRenderer, and provide service for other
		render modules

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

IRenderInfrastructure::IRenderInfrastructure():
	mEnableDepthTest(true),
	mEnablePostProcessing(false)
{
}

IRenderInfrastructure::~IRenderInfrastructure()
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
	ReleaseCOM(m_pRenderTargetViewForDisplay);
	ReleaseCOM(m_pRTTShaderResourceView);
	ReleaseCOM(m_pSwapChain);

}

bool	IRenderInfrastructure::Init(UINT BufferWidth, UINT BufferHeight, bool IsWindowed)
{
	//init d3d infrastructure
	const uint32_t msaaSampleCount = 1;
	const uint32_t msaaQuality = 0;

	if (!mFunction_Init_CreateSwapChainAndRTVandDSVandViewport(BufferWidth, BufferHeight, IsWindowed, msaaQuality, msaaSampleCount))
	{
		ERROR_MSG("IRenderer : failed to Init D3D Infrastructure.");
		return false;
	};

	if (!mFunction_Init_CreateRenderToTextureViews(BufferWidth, BufferHeight, msaaQuality, msaaSampleCount))
	{
		ERROR_MSG("IRenderer : failed to Post Processing Infrastructure.");
		return false;
	};

	HRESULT hr = S_OK;

	//get effect Technique interfaces
	/*m_pFX_Tech_DrawMesh = g_pFX->GetTechniqueByName("DrawMesh");
	m_pFX_Tech_Solid3D = g_pFX->GetTechniqueByName("DrawSolid3D");
	m_pFX_Tech_Solid2D = g_pFX->GetTechniqueByName("DrawSolid2D");
	m_pFX_Tech_Textured2D = g_pFX->GetTechniqueByName("DrawTextured2D");
	m_pFX_Tech_DrawSky = g_pFX->GetTechniqueByName("DrawSky");
	m_pFX_Tech_DrawText2D = g_pFX->GetTechniqueByName("DrawText2D");*/

#pragma region Create Input Layout
	//default vertex input layout
	D3DX11_PASS_DESC passDesc;
	g_pFX->GetTechniqueByName("DrawMesh")->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = g_pd3dDevice11->CreateInputLayout(
		&g_VertexDesc_Default[0],
		g_VertexDesc_Default_ElementCount,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&g_pVertexLayout_Default);
	HR_DEBUG(hr, "IRenderer : Create Default input layout failed！");

	//simple vertex input layout
	g_pFX->GetTechniqueByName("DrawSolid3D")->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = g_pd3dDevice11->CreateInputLayout(
		&g_VertexDesc_Simple[0],
		g_VertexDesc_Simple_ElementCount,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&g_pVertexLayout_Simple);
	HR_DEBUG(hr, "IRenderer : Create Simple input Layout failed！");

#pragma endregion Create Input Layout

	// Create Fx Variable
	m_pRefShaderVarMgr = IShaderVariableManager::GetSingleton();
	if (m_pRefShaderVarMgr == nullptr)
	{
		ERROR_MSG("IRenderer: effect Initialization failure! (shader variable error)");
		return false;
	};

	//Create Various kinds of states
	if (!mFunction_Init_CreateRasterState())return false;
	if (!mFunction_Init_CreateBlendState())return false;
	if (!mFunction_Init_CreateSamplerState())return false;
	if (!mFunction_Init_CreateDepthStencilState())return false;

	mBackBufferWidth = BufferWidth;
	mBackBufferHeight = BufferHeight;

	return true;
}

void IRenderInfrastructure::SetInputAssembler(NOISE_VERTEX_TYPE vertexType, ID3D11Buffer * pVB, ID3D11Buffer * pIB, D3D11_PRIMITIVE_TOPOLOGY topo)
{
	switch (vertexType)
	{
		case NOISE_VERTEX_TYPE::DEFAULT:
		{
			g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Default);
			g_pImmediateContext->IASetVertexBuffers(0, 1, &pVB, &g_cVBstride_Default, &g_cVBoffset);
			g_pImmediateContext->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);
			g_pImmediateContext->IASetPrimitiveTopology(topo);
			break;
		}

		case NOISE_VERTEX_TYPE::SIMPLE:
		{
			g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
			g_pImmediateContext->IASetVertexBuffers(0, 1, &pVB, &g_cVBstride_Simple, &g_cVBoffset);
			g_pImmediateContext->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);
			g_pImmediateContext->IASetPrimitiveTopology(topo);
			break;
		}
	}
}

void	IRenderInfrastructure::SetRasterState(NOISE_FILLMODE iFillMode, NOISE_CULLMODE iCullMode)
{
	//fillMode , or "which primitive to draw, point?line?triangle?" , affect the decision of topology
	//like if user chooses WIREFRAME mode , the topology will be LINELIST

	switch (iFillMode)
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

void	IRenderInfrastructure::SetBlendState(NOISE_BLENDMODE iBlendMode)
{
	float tmpBlendFactor[4] = { 0,0,0,0 };
	switch (iBlendMode)
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

void	IRenderInfrastructure::SetDepthStencilState()
{
	if (mEnableDepthTest)
	{
		g_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState_EnableDepthTest, 0xffffffff);
	}
	else
	{
		g_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState_DisableDepthTest, 0xffffffff);
	}
}

void IRenderInfrastructure::SetSampler(IShaderVariableManager::NOISE_SHADER_VAR_SAMPLER sampler, NOISE_SAMPLERMODE mode)
{
	switch (mode)
	{
		case NOISE_SAMPLERMODE::LINEAR:
		{
			m_pRefShaderVarMgr->SetSampler(sampler, 0, m_pSamplerState_FilterLinear);
			break;
		}

		default:
	}

}

void	IRenderInfrastructure::SetRtvAndDsv(NOISE_RENDER_STAGE stage)
{
	//the decision of how to set RTV/DSV according to state of post-processing
	//WILL BE made here.
	//(though some effect might require 2+ passessor 2+ RTV (like deferred rendering?), 
	//and the branches count might be tremendous.
	//but someone has to do it= = so there you go.

	if (stage == NOISE_RENDER_STAGE::NORMAL_DRAWING)
	{
		//directly render to back buffer
		g_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetViewForDisplay, m_pDepthStencilView);
	}
	else if (stage ==NOISE_RENDER_STAGE::POST_PROCESSING)
	{
		if (mEnablePostProcessing)
		{
			//render to off-screen texture first
			//(Note that RTV & DSV should be set at the same time
			g_pImmediateContext->OMSetRenderTargets(1, &m_pRTTRenderTargetView, m_pRTTDepthStencilView);
		}
		else
		{
			ERROR_MSG("IRenderInfrastructure: this is a bug! shouldn't run inside here.");
		}
	}
}

void IRenderInfrastructure::ClearRtvAndDsv(const NVECTOR4 & color)
{
	//choose RTV and DSV to clear (target varys when post-processing is on or off)
	if (mEnablePostProcessing)
	{
		float ClearColor[4] = { color.x,color.y,color.z,color.w };
		g_pImmediateContext->ClearRenderTargetView(m_pRenderTargetViewForDisplay, ClearColor);
		g_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		g_pImmediateContext->ClearRenderTargetView(m_pRTTRenderTargetView, ClearColor);
		g_pImmediateContext->ClearDepthStencilView(m_pRTTDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
	else
	{
		float ClearColor[4] = { color.x,color.y,color.z,color.w };
		g_pImmediateContext->ClearRenderTargetView(m_pRenderTargetViewForDisplay, ClearColor);
		g_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void	IRenderInfrastructure::UpdateCameraMatrix(ICamera* const pCamera)
{
	//update camera matrices
	NMATRIX tmpMatrix;
	pCamera->GetProjMatrix(tmpMatrix);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::PROJECTION, tmpMatrix);

	pCamera->GetViewMatrix(tmpMatrix);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::VIEW, tmpMatrix);

	pCamera->GetInvViewMatrix(tmpMatrix);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::VIEW_INV, tmpMatrix);

	NVECTOR3 camPos = pCamera->GetPosition();
	m_pRefShaderVarMgr->SetVector3(IShaderVariableManager::NOISE_SHADER_VAR_VECTOR::CAMERA_POS3, camPos);
}

void IRenderInfrastructure::SwapChainPresent()
{
	m_pSwapChain->Present(0, 0);
}


/***********************************************************************
									P R I V A T E
************************************************************************/

bool	IRenderInfrastructure::mFunction_Init_CreateSwapChainAndRTVandDSVandViewport(UINT BufferWidth, UINT BufferHeight, bool IsWindowed, UINT msaaQuality, UINT msaaSampleCount)
{
	//check multi-sample capability
	//the support level of MSAA might vary among hardwares
	bool enableMSAA = false;
	g_pd3dDevice11->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, msaaSampleCount, &msaaQuality);
	if (msaaQuality > 0)enableMSAA = true;

	//-----------------SWAP CHAIN------------------
	//Swap Chain description
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = BufferWidth;
	swapChainDesc.BufferDesc.Height = BufferHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = GetRoot()->GetRenderWindowHWND();
	swapChainDesc.Windowed = IsWindowed;
	swapChainDesc.SampleDesc.Count = (enableMSAA == true ? msaaSampleCount : 1);//if MSAA enabled, RT/DS buffer must have same quality
	swapChainDesc.SampleDesc.Quality = (enableMSAA == true ? msaaQuality : 0);

	//use COM's QueryInterface to get desired interface
	IDXGIDevice *dxgiDevice = 0;
	g_pd3dDevice11->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	IDXGIAdapter *dxgiAdapter = 0;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	IDXGIFactory *dxgiFactory = 0;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

	//Create Swap chain
	HRESULT hr = dxgiFactory->CreateSwapChain(
		g_pd3dDevice11,
		&swapChainDesc,
		&m_pSwapChain);
	HR_DEBUG(hr, "IRenderer: failed to create SwapChain!");

	dxgiFactory->Release();
	dxgiDevice->Release();
	dxgiAdapter->Release();

	//Create default Render Target View:
	//a texture2D is implicitly created when a swap chain is initialized.
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	HR_DEBUG(hr, "IRenderer: failed to get back buffer from swap chain.");

	hr = g_pd3dDevice11->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetViewForDisplay);
	pBackBuffer->Release();
	HR_DEBUG(hr, "IRenderer : failed to create render target view");

	//--------DSV : Depth Stencil View  for back buffer-----------
	D3D11_TEXTURE2D_DESC DSBufferDesc;
	DSBufferDesc.Width = BufferWidth;
	DSBufferDesc.Height = BufferHeight;
	DSBufferDesc.MipLevels = 1;
	DSBufferDesc.ArraySize = 1;
	DSBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSBufferDesc.SampleDesc.Count = (enableMSAA ? msaaSampleCount : 1);//if MSAA enabled, RT/DS buffer must have same quality
	DSBufferDesc.SampleDesc.Quality = (enableMSAA ? msaaQuality : 0);
	DSBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	DSBufferDesc.CPUAccessFlags = 0;
	DSBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DSBufferDesc.MiscFlags = 0;

	ID3D11Texture2D* pDepthStencilBuffer;
	g_pd3dDevice11->CreateTexture2D(&DSBufferDesc, 0, &pDepthStencilBuffer);
	hr = g_pd3dDevice11->CreateDepthStencilView(pDepthStencilBuffer, nullptr, &m_pDepthStencilView);

	pDepthStencilBuffer->Release();
	HR_DEBUG(hr, "IRenderer: failed to create depth stencil buffer");

	//--------------------------VIEWPORT--------------------------------
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)BufferWidth;
	vp.Height = (FLOAT)BufferHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, &vp);

	return true;
}

bool IRenderInfrastructure::mFunction_Init_CreateRenderToTextureViews(UINT bufferWidth, UINT bufferHeight, UINT msaaQuality, UINT msaaSampleCount)
{
	//check multi-sample capability
	//the support level of MSAA might vary among hardwares
	bool enableMSAA = false;
	g_pd3dDevice11->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, msaaSampleCount, &msaaQuality);
	if (msaaQuality > 0)enableMSAA = true;

	//Create Render Target View
	//RTV should be created from a explicitly created ID3D11Texture2D,
	//or a implicitly created Texture2D of Swap chain's back buffer
	//NOTE: desc.BindFlags is a KEYPOINT for implementing Render-To-Texture
	D3D11_TEXTURE2D_DESC backBufferTexDesc;
	backBufferTexDesc.Width = bufferWidth;
	backBufferTexDesc.Height = bufferHeight;
	backBufferTexDesc.MipLevels = 1;
	backBufferTexDesc.ArraySize = 1;
	backBufferTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	backBufferTexDesc.SampleDesc.Count = (enableMSAA ? msaaSampleCount : 1);//if MSAA enabled, RT/DS buffer must have same quality
	backBufferTexDesc.SampleDesc.Quality = (enableMSAA ? msaaQuality : 0);
	backBufferTexDesc.Usage = D3D11_USAGE_DEFAULT;
	backBufferTexDesc.CPUAccessFlags = 0;
	backBufferTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	backBufferTexDesc.MiscFlags = 0;

	ID3D11Texture2D* pBackBufferTexture = nullptr;
	HRESULT hr = g_pd3dDevice11->CreateTexture2D(&backBufferTexDesc, nullptr, &pBackBufferTexture);
	HR_DEBUG(hr, "IRenderer: failed to create back buffer texture2D.");

	//Create RTV
	hr = g_pd3dDevice11->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pRTTRenderTargetView);
	if (FAILED(hr))
	{
		ReleaseCOM(pBackBufferTexture);
		ERROR_MSG("IRenderer: failed to create Render Target View");
		return false;
	}

	//Create RTT shader resource view
	hr = g_pd3dDevice11->CreateShaderResourceView(pBackBufferTexture, nullptr, &m_pRTTShaderResourceView);
	if (FAILED(hr))
	{
		ReleaseCOM(pBackBufferTexture);
		ERROR_MSG("IRenderer: failed to create RTT SRV");
		return false;
	}

	//texture2D can be released after RTV is created
	ReleaseCOM(pBackBufferTexture);

	//DSV for RTT texture
	//the size(width,height) must match the back buffer of RTV
	D3D11_TEXTURE2D_DESC DSBufferDesc;
	DSBufferDesc.Width = bufferWidth;
	DSBufferDesc.Height = bufferHeight;
	DSBufferDesc.MipLevels = 1;
	DSBufferDesc.ArraySize = 1;
	DSBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSBufferDesc.SampleDesc.Count = (enableMSAA ? msaaSampleCount : 1);//if MSAA enabled, RT/DS buffer must have same quality
	DSBufferDesc.SampleDesc.Quality = (enableMSAA ? msaaQuality : 0);
	DSBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	DSBufferDesc.CPUAccessFlags = 0;
	DSBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DSBufferDesc.MiscFlags = 0;

	ID3D11Texture2D* pDepthStencilBuffer;
	g_pd3dDevice11->CreateTexture2D(&DSBufferDesc, 0, &pDepthStencilBuffer);
	hr = g_pd3dDevice11->CreateDepthStencilView(pDepthStencilBuffer, nullptr, &m_pRTTDepthStencilView);
	pDepthStencilBuffer->Release();
	HR_DEBUG(hr, "IRenderer: failed to create depth stencil buffer");

	return true;
}

bool	IRenderInfrastructure::mFunction_Init_CreateBlendState()
{

	//source color : the first color in blending equation
	HRESULT hr = S_OK;

	D3D11_BLEND_DESC tmpBlendDesc;
	tmpBlendDesc.AlphaToCoverageEnable = false; // ???related to multi-sampling
	tmpBlendDesc.IndependentBlendEnable = false; //determine if 8 simultaneous render targets are rendered with same blend state
	tmpBlendDesc.RenderTarget[0].BlendEnable = false;
	tmpBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	tmpBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	tmpBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	tmpBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	tmpBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	tmpBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tmpBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pd3dDevice11->CreateBlendState(&tmpBlendDesc, &m_pBlendState_Opaque);
	HR_DEBUG(hr, "Create blend state(opaque) failed!!");

	tmpBlendDesc.AlphaToCoverageEnable = false; // ???related to multi-sampling
	tmpBlendDesc.IndependentBlendEnable = false; //determine if 8 simultaneous render targets are rendered with same blend state
	tmpBlendDesc.RenderTarget[0].BlendEnable = true;
	tmpBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	tmpBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	tmpBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	tmpBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	tmpBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	tmpBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tmpBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pd3dDevice11->CreateBlendState(&tmpBlendDesc, &m_pBlendState_ColorAdd);
	HR_DEBUG(hr, "Create blend state(Color Add) failed!!");

	tmpBlendDesc.AlphaToCoverageEnable = false; // ???related to multi-sampling
	tmpBlendDesc.IndependentBlendEnable = false; //determine if 8 simultaneous render targets are rendered with same blend state
	tmpBlendDesc.RenderTarget[0].BlendEnable = true;
	tmpBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
	tmpBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	tmpBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	tmpBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	tmpBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	tmpBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tmpBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pd3dDevice11->CreateBlendState(&tmpBlendDesc, &m_pBlendState_ColorMultiply);
	HR_DEBUG(hr, "Create blend state(Color Filter) failed!!");

	tmpBlendDesc.AlphaToCoverageEnable = false; // ???related to multi-sampling
	tmpBlendDesc.IndependentBlendEnable = false; //determine if 8 simultaneous render targets are rendered with same blend state
	tmpBlendDesc.RenderTarget[0].BlendEnable = true;
	tmpBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;//what about D3D11_BLEND_SRC1_COLOR
	tmpBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	tmpBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	tmpBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	tmpBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	tmpBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tmpBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pd3dDevice11->CreateBlendState(&tmpBlendDesc, &m_pBlendState_AlphaTransparency);
	HR_DEBUG(hr, "Create blend state(Transparency) failed!!");

	return true;
};

bool	IRenderInfrastructure::mFunction_Init_CreateRasterState()
{
	HRESULT hr = S_OK;
	//创建预设的光栅化state
	//Create Raster State;If you want various Raster State,you should pre-Create all of them in the beginning
	D3D11_RASTERIZER_DESC tmpRasterStateDesc;//光栅化设置
	ZeroMemory(&tmpRasterStateDesc, sizeof(D3D11_RASTERIZER_DESC));
	tmpRasterStateDesc.AntialiasedLineEnable = true;//抗锯齿设置
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

	return true;
};

bool	IRenderInfrastructure::mFunction_Init_CreateSamplerState()
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

	return true;
};

bool	IRenderInfrastructure::mFunction_Init_CreateDepthStencilState()
{
	HRESULT hr = S_OK;
	//depth stencil state
	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(dssDesc));
	dssDesc.DepthEnable = true;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.StencilEnable = false;
	hr = g_pd3dDevice11->CreateDepthStencilState(&dssDesc, &m_pDepthStencilState_EnableDepthTest);
	HR_DEBUG(hr, "Create Depth Stencil State #1 Failed!!!");


	ZeroMemory(&dssDesc, sizeof(dssDesc));
	dssDesc.DepthEnable = false;
	dssDesc.StencilEnable = false;
	hr = g_pd3dDevice11->CreateDepthStencilState(&dssDesc, &m_pDepthStencilState_DisableDepthTest);
	HR_DEBUG(hr, "Create Depth Stencil State #2 Failed!!!");

	return true;
};

bool	IRenderInfrastructure::mFunction_Init_CreateEffectFromFile(NFilePath fxPath)
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
		fxPath.c_str(), 0, 0, 0, "fx_5_0",
		shaderFlags, 0, 0, &compiledFX,
		&compilationMsg, 0);

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
		0, g_pd3dDevice11, &g_pFX, nullptr);

	HR_DEBUG(hr, "Create Basic Effect Fail!");

	return true;
};
