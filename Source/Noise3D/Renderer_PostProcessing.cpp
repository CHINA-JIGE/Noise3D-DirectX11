
/***********************************************************************

                           h£ºrender module for post-processing
				post-processing: render to a new

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

 IRenderModuleForPostProcessing::IRenderModuleForPostProcessing():
	m_pOffScreenRenderTargetView_A(nullptr),
	m_pOffScreenRenderTargetView_B(nullptr),
	m_pOffScreenDepthStencilView_A(nullptr),
	m_pOffScreenDepthStencilView_B(nullptr),
	m_pOffScreenShaderResourceView_A(nullptr),
	m_pOffScreenShaderResourceView_B(nullptr),
	m_pVB_Quad(nullptr),
	m_pFX_Tech_PostProcessing(nullptr),
	m_pFX_Pass_GreyScale(nullptr),
	m_pFX_Pass_Qwerty(nullptr)
{
}

IRenderModuleForPostProcessing::~IRenderModuleForPostProcessing()
{
	ReleaseCOM(m_pOffScreenRenderTargetView_A);
	ReleaseCOM(m_pOffScreenRenderTargetView_B);
	ReleaseCOM(m_pOffScreenDepthStencilView_A);
	ReleaseCOM(m_pOffScreenDepthStencilView_B);
	ReleaseCOM(m_pOffScreenShaderResourceView_A);
	ReleaseCOM(m_pOffScreenShaderResourceView_B);
	ReleaseCOM(m_pVB_Quad);
	ReleaseCOM(m_pFX_Tech_PostProcessing);
	ReleaseCOM(m_pFX_Pass_GreyScale);
	ReleaseCOM(m_pFX_Pass_Qwerty);
}

void IRenderModuleForPostProcessing::AddToPostProcessList_GreyScale(const N_PostProcessGreyScaleDesc & param)
{
	mGreyScaleDescList.push_back(param);

	N_PostProcessPass pass;
	pass.type = NOISE_POST_PROCESS_EFFECT::GreyScale;
	pass.index = mGreyScaleDescList.size()-1;
	mPostProcessEffectQueue.push_back(pass);
}

void IRenderModuleForPostProcessing::AddToPostProcessList_QwertyDistortion(const N_PostProcesQwertyDistortionDesc & param)
{
	mQwertyDescList.push_back(param);

	N_PostProcessPass pass;
	pass.type = NOISE_POST_PROCESS_EFFECT::QwertyDistortion;
	pass.index = mQwertyDescList.size() - 1;
	mPostProcessEffectQueue.push_back(pass);
}

/*******************************************************
								   PROTECTED
********************************************************/

void IRenderModuleForPostProcessing::PostProcess()
{
	uint32_t totalPassCount = mPostProcessEffectQueue.size();
	for (uint32_t passIndex = 0; passIndex < totalPassCount; ++passIndex)
	{
		//settings (IA will be set before executing each task)
		//the queue size decreases (pop_front) after task is performed
		m_pRefRI->SetPostProcessRemainingPassCount(mPostProcessEffectQueue.size());
		m_pRefRI->SetRtvAndDsv(IRenderInfrastructure::NOISE_RENDER_STAGE::POST_PROCESSING);
		m_pRefRI->SetDepthStencilState(false);
		m_pRefRI->SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_NONE);
		m_pRefRI->SetSampler(IShaderVariableManager::NOISE_SHADER_VAR_SAMPLER::DRAW_2D_SAMPLER, NOISE_SAMPLERMODE::LINEAR);
		m_pRefRI->SetBlendState(NOISE_BLENDMODE_OPAQUE);

		//remaining task count must be updated so that the RTV and DSV can be 
		//configured correctly
		N_PostProcessPass& pass = mPostProcessEffectQueue.front();
		switch (pass.type)
		{
		case NOISE_POST_PROCESS_EFFECT::GreyScale:
		{
			uint32_t index = mPostProcessEffectQueue.front().index;
			mFunction_GreyScale(mGreyScaleDescList.at(index));
			break;
		}

		case NOISE_POST_PROCESS_EFFECT::QwertyDistortion:
		{
			uint32_t index = mPostProcessEffectQueue.front().index;
			mFunction_QwertyDistortion(mQwertyDescList.at(index));
			break;
		}

		default:
		{
			ERROR_MSG("IRenderer(Post Processing Module): BUG! No Such post effect!");
			break;
		};
		}

		mPostProcessEffectQueue.pop_front();
	}
}

uint32_t IRenderModuleForPostProcessing::GetPostProcessPassCount()
{
	return mPostProcessEffectQueue.size();
}

void IRenderModuleForPostProcessing::ClearRenderList()
{
	mPostProcessEffectQueue.clear();
	mGreyScaleDescList.clear();
	mQwertyDescList.clear();
}

bool IRenderModuleForPostProcessing::Initialize(IRenderInfrastructure * pRI, IShaderVariableManager * pShaderVarMgr)
{
	m_pRefRI = pRI;
	m_pRefShaderVarMgr = pShaderVarMgr;
	m_pFX_Tech_PostProcessing= g_pFX->GetTechniqueByName("PostProcessing");
	m_pFX_Pass_GreyScale = m_pFX_Tech_PostProcessing->GetPassByName("GreyScale");
	m_pFX_Pass_Qwerty = m_pFX_Tech_PostProcessing->GetPassByName("QwertyDistortion");

	//initialize RTV A&B, DSV A&B. split this two up has less concern 
	//about releasing ID3D11Texture2D* when error occurs
	if (!mFunction_Init_CreateOffScreenRTV(
		m_pRefRI->GetBackBufferWidth(),
		m_pRefRI->GetBackBufferHeight(),
		m_pRefRI->GetMsaaSampleCount()))
	{
		ERROR_MSG("IRenderer: failed to init Post Processing module. (off-screen render targets)");
		return false;
	}

	if (!mFunction_Init_CreateOffScreenDSV(
		m_pRefRI->GetBackBufferWidth(),
		m_pRefRI->GetBackBufferHeight(),
		m_pRefRI->GetMsaaSampleCount()))
	{
		ERROR_MSG("IRenderer: failed to init Post Processing module. (off-screen depth-stencil buffer)");
		return false;
	}

	//initialize a quad vertex buffer (2 triangles that cover the whole screen)
	if (!mFunction_Init_VertexBufferOfQuad())
	{
		ERROR_MSG("IRenderer: failed to init Post Processing module.");
		return false;
	} 

	//pass the reference of off-screen RTV/DSV to render infrastructure 
	m_pRefRI->SetRTTViewsReference(
		m_pOffScreenRenderTargetView_A,
		m_pOffScreenRenderTargetView_B,
		m_pOffScreenDepthStencilView_A,
		m_pOffScreenDepthStencilView_B);

	return true;
}


/*******************************************************
									PRIVATE
********************************************************/

bool IRenderModuleForPostProcessing::mFunction_Init_VertexBufferOfQuad()
{
	//pos3,color4,texCoord2
	//clockwise vertex order ; one quad is composed of 2 triangles
	// 1  ___________ 2
	//		|				 |
	//	0	`````````````````` 3
	N_SimpleVertex quadVertexArray[cQuadVertexCount]=
	{
		{ { -1.0f,-1.0f,0 },{ 0,0,0,0 },{ 0,1 } },//BottomLeft 0
		{ { -1.0f,1.0f,0 },{ 0,0,0,0 },{ 0,0 } },//TopLeft 1
		{ { 1.0f,1.0f,0 },{ 0,0,0,0 },{ 1,0 } },//TopRight 2

		{ { -1.0f,-1.0f,0 },{ 0,0,0,0 },{ 0,1 } },//BottomLeft 0 
		{ { 1.0f,1.0f,0 },{ 0,0,0,0 },{ 1,0 } },//TopRight 2 
		{ { 1.0f,-1.0f,0 },{ 0,0,0,0 },{ 1,1 } }//BottomRight3
	};

	//Prepare to update to video memory, fill in SUBRESOURCE description structure
	D3D11_SUBRESOURCE_DATA tmpInitData;
	ZeroMemory(&tmpInitData, sizeof(tmpInitData));
	tmpInitData.pSysMem = quadVertexArray;

	//------Create VERTEX BUFFER
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = sizeof(N_SimpleVertex)* cQuadVertexCount;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	//Create Buffers
	int hr = 0;
	hr = g_pd3dDevice11->CreateBuffer(&vbd, &tmpInitData, &m_pVB_Quad);
	HR_DEBUG(hr, "IRenderer : failed to init Post processing Quad VB!");

	return true;
}

bool IRenderModuleForPostProcessing::mFunction_Init_CreateOffScreenRTV(UINT bufferWidth, UINT bufferHeight, UINT cMsaaSampleCount)
{
	//check multi-sample capability
	//the support level of MSAA might vary among hardwares
	bool enableMSAA = false;
	UINT msaaQuality = 0;//query via d3d11
	g_pd3dDevice11->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, cMsaaSampleCount, &msaaQuality);
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
	backBufferTexDesc.SampleDesc.Count = (enableMSAA ? cMsaaSampleCount : 1);//if MSAA enabled, RT/DS buffer must have same quality
	backBufferTexDesc.SampleDesc.Quality = (enableMSAA ? msaaQuality - 1 : 0);
	backBufferTexDesc.Usage = D3D11_USAGE_DEFAULT;
	backBufferTexDesc.CPUAccessFlags = 0;
	backBufferTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	backBufferTexDesc.MiscFlags = 0;

	//if one step failed, remember to release created resource
	ID3D11Texture2D* pOffScreenTextureA = nullptr;
	ID3D11Texture2D* pOffScreenTextureB = nullptr;
	HRESULT hr = g_pd3dDevice11->CreateTexture2D(&backBufferTexDesc, nullptr, &pOffScreenTextureA);
	HR_DEBUG(hr, "IRenderer: failed to create back buffer texture2D_A.");

	hr = g_pd3dDevice11->CreateTexture2D(&backBufferTexDesc, nullptr, &pOffScreenTextureB);
	if (FAILED(hr))
	{
		ERROR_MSG("IRenderer: failed to create back buffer texture2D_B.");
		return false;
	}

	//RTV for texture A
	hr = g_pd3dDevice11->CreateRenderTargetView(pOffScreenTextureA, nullptr, &m_pOffScreenRenderTargetView_A);
	if (FAILED(hr))
	{
		ReleaseCOM(pOffScreenTextureA);
		ReleaseCOM(pOffScreenTextureB);
		ERROR_MSG("IRenderer: failed to create Render Target View A");
		return false;
	}

	//RTV for texture B
	hr = g_pd3dDevice11->CreateRenderTargetView(pOffScreenTextureB, nullptr, &m_pOffScreenRenderTargetView_B);
	if (FAILED(hr))
	{
		ReleaseCOM(pOffScreenTextureA);
		ReleaseCOM(pOffScreenTextureB);
		ERROR_MSG("IRenderer: failed to create Render Target View B");
		return false;
	}

	//SRV for texture A
	hr = g_pd3dDevice11->CreateShaderResourceView(pOffScreenTextureA, nullptr, &m_pOffScreenShaderResourceView_A);
	if (FAILED(hr))
	{
		ReleaseCOM(pOffScreenTextureA);
		ReleaseCOM(pOffScreenTextureB);
		ERROR_MSG("IRenderer: failed to create RTT SRV A");
		return false;
	}

	//SRV for texture B
	hr = g_pd3dDevice11->CreateShaderResourceView(pOffScreenTextureB, nullptr, &m_pOffScreenShaderResourceView_B);
	if (FAILED(hr))
	{
		ReleaseCOM(pOffScreenTextureA);
		ReleaseCOM(pOffScreenTextureB);
		ERROR_MSG("IRenderer: failed to create RTT SRV B");
		return false;
	}

	//this texture2D can be released after DSV is created
	ReleaseCOM(pOffScreenTextureA);
	ReleaseCOM(pOffScreenTextureB);

	return true;
}

bool IRenderModuleForPostProcessing::mFunction_Init_CreateOffScreenDSV(UINT bufferWidth, UINT bufferHeight, UINT cMsaaSampleCount)
{
	//check multi-sample capability
	//the support level of MSAA might vary among hardwares
	bool enableMSAA = false;
	UINT msaaQuality = 0;//query via d3d11
	g_pd3dDevice11->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, cMsaaSampleCount, &msaaQuality);
	if (msaaQuality > 0)enableMSAA = true;

	//DSV for RTT texture
	//the size(width,height) must match the back buffer of RTV
	D3D11_TEXTURE2D_DESC DSBufferDesc;
	DSBufferDesc.Width = bufferWidth;
	DSBufferDesc.Height = bufferHeight;
	DSBufferDesc.MipLevels = 1;
	DSBufferDesc.ArraySize = 1;
	DSBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSBufferDesc.SampleDesc.Count = (enableMSAA ? cMsaaSampleCount : 1);//if MSAA enabled, RT/DS buffer must have same quality
	DSBufferDesc.SampleDesc.Quality = (enableMSAA ? msaaQuality - 1 : 0);
	DSBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	DSBufferDesc.CPUAccessFlags = 0;
	DSBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DSBufferDesc.MiscFlags = 0;

	ID3D11Texture2D* pDepthStencilBufferA;
	ID3D11Texture2D* pDepthStencilBufferB;
	HRESULT hr = g_pd3dDevice11->CreateTexture2D(&DSBufferDesc, 0, &pDepthStencilBufferA);
	HR_DEBUG(hr, "IRenderer: failed to create Off screeen Depth-Stencil texture.");

	hr = g_pd3dDevice11->CreateTexture2D(&DSBufferDesc, 0, &pDepthStencilBufferB);
	if (FAILED(hr))
	{
		ReleaseCOM(pDepthStencilBufferB)
		ERROR_MSG("IRenderer: failed to create Off screeen Depth-Stencil texture.");
		return false;
	}

	hr = g_pd3dDevice11->CreateDepthStencilView(pDepthStencilBufferA, nullptr, &m_pOffScreenDepthStencilView_A);
	if (FAILED(hr))
	{
		ReleaseCOM(pDepthStencilBufferA);
		ReleaseCOM(pDepthStencilBufferB);
		ERROR_MSG("IRenderer: failed to create DSV for RTT A");
		return false;
	}

	hr = g_pd3dDevice11->CreateDepthStencilView(pDepthStencilBufferB, nullptr, &m_pOffScreenDepthStencilView_B);
	if (FAILED(hr))
	{
		ReleaseCOM(pDepthStencilBufferA);
		ReleaseCOM(pDepthStencilBufferB);
		ERROR_MSG("IRenderer: failed to create DSV for RTT B");
		return false;
	}

	//this texture2D can be released after DSV is created
	ReleaseCOM(pDepthStencilBufferA);
	ReleaseCOM(pDepthStencilBufferB);

	return true;
}

void IRenderModuleForPostProcessing::mFunction_SetInputShaderResource()
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//ensure compatibility with RTV settings of IRenderInfrastructure
	//"=>" is initial render to texture 
	//"->" is a post process pass
	//0 pass -- =>backBuffer
	//1 pass -- => rtB -> backBuffer
	//2 pass -- => rtA -> rtB ->backBuffer
	//3 pass -- => rtB -> rtA ->rtB ->backBuffer
	//4 pass -- => rtA -> rtB -> rtA -> rtB ->backBuffer
	uint32_t passCount = mPostProcessEffectQueue.size();
	uint32_t passIndexParity = passCount % 2;

	//passCount==0 means directly draw to back buffer
	if (passCount ==0)return;

	if (passIndexParity == 0)
	{
		m_pRefShaderVarMgr->SetTexture(
			IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::POST_PROCESS_PREV_RT, 
			m_pOffScreenShaderResourceView_A);
	}
	else
	{
		m_pRefShaderVarMgr->SetTexture(
			IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::POST_PROCESS_PREV_RT,
			m_pOffScreenShaderResourceView_B);
	}
}

void IRenderModuleForPostProcessing::mFunction_GreyScale(const N_PostProcessGreyScaleDesc& param)
{
	m_pRefRI->SetInputAssembler(
		IRenderInfrastructure::NOISE_VERTEX_TYPE::SIMPLE,
		m_pVB_Quad,nullptr,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//prior RTV is now bound as SRV
	mFunction_SetInputShaderResource();

	//update param

	m_pFX_Pass_GreyScale->Apply(0, g_pImmediateContext);
	g_pImmediateContext->Draw(cQuadVertexCount, 0);
}

void IRenderModuleForPostProcessing::mFunction_QwertyDistortion(const N_PostProcesQwertyDistortionDesc& param)
{
	IMesh* pMesh = param.pScreenDescriptor;
	ICamera* pCam = param.pCamera;

	//though SIMPLE is enough, but i want to make full use of the model loading function of IMesh
	m_pRefRI->SetInputAssembler(
		IRenderInfrastructure::NOISE_VERTEX_TYPE::DEFAULT, 
		pMesh->m_pVB_Gpu, pMesh->m_pIB_Gpu, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//prior RTV is now bound as SRV
	mFunction_SetInputShaderResource();

	//update param
	m_pRefRI->UpdateCameraMatrix(pCam);

	m_pFX_Pass_Qwerty->Apply(0, g_pImmediateContext);
	g_pImmediateContext->DrawIndexed(pMesh->GetIndexCount(),0,0);
}
