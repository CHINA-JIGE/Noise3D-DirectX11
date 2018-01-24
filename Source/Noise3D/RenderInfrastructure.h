
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
		
		bool		Init(UINT bufferWidth, UINT bufferHeight, bool IsWindowed);

		void		SetInputAssembler(NOISE_VERTEX_TYPE vertexType,ID3D11Buffer* pVB,ID3D11Buffer* pIB,D3D11_PRIMITIVE_TOPOLOGY topo);

		void		SetRasterState(NOISE_FILLMODE iFillMode, NOISE_CULLMODE iCullMode);

		void		SetBlendState(NOISE_BLENDMODE iBlendMode);

		void		SetDepthStencilState(bool enableDepthTest);

		void		SetSampler(IShaderVariableManager::NOISE_SHADER_VAR_SAMPLER sampler,NOISE_SAMPLERMODE mode);

		//(2018.1.24)param can change to meet needs
		void		SetRtvAndDsv(NOISE_RENDER_STAGE stage);

		//clear render target view(s) and depth stencil view(s)
		void		ClearRtvAndDsv(const NVECTOR4& color = NVECTOR4(0, 0, 0, 0.0f));

		void		UpdateCameraMatrix(ICamera* const pCamera);

		void		SwapChainPresent();

		IShaderVariableManager* GetRefToShaderVarMgr();

		//intermediate
		ID3D11ShaderResourceView* GetTextureSRV(ITextureManager* pMgr, N_UID uid);

		ID3D11ShaderResourceView* GetTextureSRV(ITexture* pTex);

	private:

		IRenderInfrastructure();

		~IRenderInfrastructure();

		friend IFactory<IRenderInfrastructure>;

		friend class IRenderer;


		bool		mFunction_Init_CreateSwapChainAndRTVandDSVandViewport(UINT bufferWidth, UINT bufferHeight, bool IsWindowed, UINT msaaSampleCount);

		bool		mFunction_Init_CreateRenderToTextureViews(UINT bufferWidth, UINT bufferHeight, UINT msaaSampleCount);

		bool		mFunction_Init_CreateBlendState();

		bool		mFunction_Init_CreateRasterState();

		bool		mFunction_Init_CreateSamplerState();

		bool		mFunction_Init_CreateDepthStencilState();

		bool		mFunction_Init_CreateEffectFromFile(NFilePath fxPath);

		UINT	mBackBufferWidth;
		UINT	mBackBufferHeight;
		bool		mEnablePostProcessing;

		IDXGISwapChain*						m_pSwapChain;
		ID3D11RenderTargetView*			m_pRenderTargetViewForDisplay;//RTV for back buffer
		ID3D11DepthStencilView*			m_pDepthStencilView;//DSV for back buffer
		ID3D11RenderTargetView*			m_pRTTRenderTargetView;//RTV for render-to-texture
		ID3D11ShaderResourceView*		m_pRTTShaderResourceView;//SRV for render-To-Texture
		ID3D11DepthStencilView*			m_pRTTDepthStencilView;//DSV for render-To-Texture
		IShaderVariableManager*			m_pRefShaderVarMgr;//singleton of shader var manager

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