
/***********************************************************************

							IRenderInfracstructure
		encapsulation of common, low-level D3D operation/states
		init by IFactory in IRenderer, and provide service for other
		render modules

************************************************************************/

#pragma once

namespace Noise3D
{
	class IRenderInfrastructure :
		private IFileIO
	{
	public:

		//Render target view/depth stencil view type
		enum NOISE_RENDER_STAGE
		{
			NORMAL_DRAWING,
			POST_PROCESSING
		};

		enum NOISE_VERTEX_TYPE
		{
			DEFAULT,
			SIMPLE
		};
		
		bool		Init(UINT bufferWidth, UINT bufferHeight,HWND renderWindowHandle);

		void		SwitchToFullScreenMode();

		void		SwitchToWindowedMode();

		void		SetInputAssembler(NOISE_VERTEX_TYPE vertexType,ID3D11Buffer* pVB,ID3D11Buffer* pIB,D3D11_PRIMITIVE_TOPOLOGY topo);

		void		SetRasterState(NOISE_FILLMODE iFillMode, NOISE_CULLMODE iCullMode);

		void		SetBlendState(NOISE_BLENDMODE iBlendMode);

		void		SetDepthStencilState(bool enableDepthTest);

		void		SetSampler(IShaderVariableManager::NOISE_SHADER_VAR_SAMPLER sampler,NOISE_SAMPLERMODE mode);

		//(2018.1.24)param can change to meet needs
		void		SetRtvAndDsv(NOISE_RENDER_STAGE stage);

		void		SetRTTViewsReference(ID3D11RenderTargetView* pRTV_A, ID3D11RenderTargetView* pRTV_B, ID3D11DepthStencilView* pDSV_A, ID3D11DepthStencilView* pDSV_B);

		void		SetPostProcessRemainingPassCount(uint32_t passCount);

		//clear render target view(s) and depth stencil view(s)
		void		ClearRtvAndDsv(const NVECTOR4& color = NVECTOR4(0, 0, 0, 0.0f));

		void		UpdateCameraMatrix(ICamera* const pCamera);

		void		SwapChainPresent();

		IShaderVariableManager*		GetRefToShaderVarMgr();

		//serve as intermediate role
		ID3D11ShaderResourceView* GetTextureSRV(ITextureManager* pMgr, N_UID uid);

		ID3D11ShaderResourceView* GetTextureSRV(ITexture* pTex);

		uint32_t	GetBackBufferWidth();

		uint32_t	GetBackBufferHeight();

		uint32_t	GetMsaaSampleCount();

		HWND		GetRenderWindowHWND();

		uint32_t	GetRenderWindowWidth();

		uint32_t	GetRenderWindowHeight();

	private:

		IRenderInfrastructure();

		~IRenderInfrastructure();

		friend IFactory<IRenderInfrastructure>;

		bool		mFunction_Init_CreateSwapChainAndRTVandDSVandViewport(UINT bufferWidth, UINT bufferHeight, UINT cMsaaSampleCount, HWND renderWindowHandle);

		bool		mFunction_Init_CreateBlendState();

		bool		mFunction_Init_CreateRasterState();

		bool		mFunction_Init_CreateSamplerState();

		bool		mFunction_Init_CreateDepthStencilState();

		bool		mFunction_Init_CreateEffectFromFile(NFilePath fxPath);

		const uint32_t cMsaaSampleCount = 1;
		HWND		mRenderWindowHWND;
		uint32_t	mBackBufferWidth;
		uint32_t	mBackBufferHeight;
		uint32_t	mPostProcessRemainingPassCount;

		IDXGISwapChain*						m_pSwapChain;
		ID3D11RenderTargetView*			m_pRenderTargetViewOfBackBuffer;//RTV for back buffer
		ID3D11DepthStencilView*			m_pDepthStencilViewOfBackBuffer;//DSV for back buffer
		IShaderVariableManager*			m_pRefShaderVarMgr;//singleton of shader var manager
		ID3D11RenderTargetView*			m_pRefOffScreenRtv_A;//RTV for RTT
		ID3D11DepthStencilView*			m_pRefOffScreenDsv_A;//DSV for RTT
		ID3D11RenderTargetView*			m_pRefOffScreenRtv_B;//RTV for RTT
		ID3D11DepthStencilView*			m_pRefOffScreenDsv_B;//DSV for RTT

		ID3D11RasterizerState*			m_pRasterState_Solid_CullNone;
		ID3D11RasterizerState*			m_pRasterState_Solid_CullBack;
		ID3D11RasterizerState*			m_pRasterState_Solid_CullFront;
		ID3D11RasterizerState*			m_pRasterState_WireFrame_CullFront;
		ID3D11RasterizerState*			m_pRasterState_WireFrame_CullNone;
		ID3D11RasterizerState*			m_pRasterState_WireFrame_CullBack;
		ID3D11BlendState*					m_pBlendState_Opaque;
		ID3D11BlendState*					m_pBlendState_AlphaTransparency;
		ID3D11BlendState*					m_pBlendState_ColorAdd;
		ID3D11BlendState*					m_pBlendState_ColorMultiply;
		ID3D11DepthStencilState*		m_pDepthStencilState_EnableDepthTest;
		ID3D11DepthStencilState*		m_pDepthStencilState_DisableDepthTest;
		ID3D11SamplerState*				m_pSamplerState_FilterLinear;
	};
}