
/***********************************************************************

                           h：IRenderer
				desc: transfer data to Graphic memory
				and use gpu to render

************************************************************************/

#pragma once

namespace Noise3D
{


	class /*_declspec(dllexport)*/ IRenderer :
		private IFileIO
	{
	public:

		void			RenderMeshes();

		void			RenderGraphicObjects();

		void			RenderAtmosphere();

		void			RenderTexts();

		void			AddObjectToRenderList(IMesh* obj);

		void			AddObjectToRenderList(IGraphicObject* obj);

		void			AddObjectToRenderList(IAtmosphere* obj);

		void			AddObjectToRenderList(IDynamicText* obj);

		void			AddObjectToRenderList(IStaticText* obj);

		void			ClearBackground(const NVECTOR4& color = NVECTOR4(0, 0, 0, 0.0f));

		void			PresentToScreen();

		UINT		GetMainBufferWidth();

		UINT		GetMainBufferHeight();

	private:

		const UINT	g_cVBstride_Default = sizeof(N_DefaultVertex);		//VertexBuffer的每个元素的字节跨度

		const UINT	g_cVBstride_Simple = sizeof(N_SimpleVertex);

		const UINT	g_cVBoffset = 0;				//VertexBuffer顶点序号偏移 因为从头开始所以offset是0

	private:

		//--------------------INITIALIZATION---------------------
		
		bool			NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_Init(UINT BufferWidth, UINT BufferHeight, bool IsWindowed);

		bool			mFunction_Init_CreateSwapChainAndRTVandDSVandViewport(UINT BufferWidth, UINT BufferHeight, bool IsWindowed);//render target view, depth stencil view

		bool			mFunction_Init_CreateBlendState();

		bool			mFunction_Init_CreateRasterState();

		bool			mFunction_Init_CreateSamplerState();

		bool			mFunction_Init_CreateDepthStencilState();

		bool			mFunction_Init_CreateEffectFromFile(NFilePath fxPath);

		//--------------------BASIC OPERATION---------------------
		void			mFunction_AddToRenderList_GraphicObj(IGraphicObject* pGraphicObj, std::vector<IGraphicObject*>* pList);

		void			mFunction_AddToRenderList_Text(IBasicTextInfo* pText, std::vector<IBasicTextInfo*>* pList);

		void			mFunction_SetRasterState(NOISE_FILLMODE iFillMode, NOISE_CULLMODE iCullMode);

		void			mFunction_SetBlendState(NOISE_BLENDMODE iBlendMode);

		void			mFunction_CameraMatrix_Update(ICamera* const pCamera);


		//----------------MESHES-----------------------
		void			mFunction_RenderMeshInList_UpdatePerObject(IMesh* const pMesh);

		void			mFunction_RenderMeshInList_UpdatePerFrame(ICamera*const pCamera);

		UINT		mFunction_RenderMeshInList_UpdatePerSubset(IMesh* const pMesh, UINT subsetID);//return subset primitive count

		void			mFunction_RenderMeshInList_UpdateRarely();


		//----------------GRAPHIC OBJECT-----------------------
		void			mFunction_GraphicObj_Update_RenderTextured2D(N_UID texName);

		void			mFunction_GraphicObj_RenderLine3DInList(ICamera*const pCamera, std::vector<IGraphicObject*>* pList);

		void			mFunction_GraphicObj_RenderPoint3DInList(ICamera*const pCamera, std::vector<IGraphicObject*>* pList);

		void			mFunction_GraphicObj_RenderLine2DInList(std::vector<IGraphicObject*>* pList);

		void			mFunction_GraphicObj_RenderPoint2DInList(std::vector<IGraphicObject*>* pList);

		void			mFunction_GraphicObj_RenderTriangle2DInList(std::vector<IGraphicObject*>* pList);


		//----------------TEXT-----------------------
		void			mFunction_TextGraphicObj_Update_TextInfo(N_UID uid, ITextureManager* pTexMgr, IBasicTextInfo* pText);

		void			mFunction_TextGraphicObj_Render(std::vector<IBasicTextInfo*>* pList);


		//----------------ATMOSPHERE-----------------------
		void			mFunction_Atmosphere_UpdateFogParameters(IAtmosphere*const pAtmo);

		void			mFunction_Atmosphere_UpdateSkyParameters(IAtmosphere*const pAtmo,bool& outEnabledSkybox,bool& outEnabledSkydome);

	private:

		friend IScene;//for external init

		friend IFactory<IRenderer>;

		//构造函数
		IRenderer();

		~IRenderer();

		IShaderVariableManager* m_pRefShaderVarMgr;

		UINT		mMainBufferWidth;
		UINT		mMainBufferHeight;

		std::vector <IMesh*>*					m_pRenderList_Mesh;
		std::vector <IGraphicObject*>* 	m_pRenderList_CommonGraphicObj;//for user-defined graphic obj rendering
		std::vector <IBasicTextInfo*>*	m_pRenderList_TextDynamic;//for dynamic Text Rendering(including other info)
		std::vector <IBasicTextInfo*>*	m_pRenderList_TextStatic;//for static Text Rendering(including other info)
		std::vector <IAtmosphere*>*		m_pRenderList_Atmosphere;

		IDXGISwapChain*						m_pSwapChain;
		ID3D11RenderTargetView*			m_pRenderTargetView;//RTV
		ID3D11DepthStencilView*			m_pDepthStencilView;//DSV
		ID3D11RasterizerState*				m_pRasterState_Solid_CullNone;
		ID3D11RasterizerState*				m_pRasterState_Solid_CullBack;
		ID3D11RasterizerState*				m_pRasterState_Solid_CullFront;
		ID3D11RasterizerState*				m_pRasterState_WireFrame_CullFront;
		ID3D11RasterizerState*				m_pRasterState_WireFrame_CullNone;
		ID3D11RasterizerState*				m_pRasterState_WireFrame_CullBack;
		ID3D11BlendState*						m_pBlendState_Opaque;
		ID3D11BlendState*						m_pBlendState_AlphaTransparency;
		ID3D11BlendState*						m_pBlendState_ColorAdd;
		ID3D11BlendState*						m_pBlendState_ColorMultiply;
		ID3D11DepthStencilState*			m_pDepthStencilState_EnableDepthTest;
		ID3D11DepthStencilState*			m_pDepthStencilState_DisableDepthTest;
		ID3D11SamplerState*					m_pSamplerState_FilterLinear;

		//用于从app更新到Gpu的接口
		ID3DX11EffectTechnique*			m_pFX_Tech_Default;
		ID3DX11EffectTechnique*			m_pFX_Tech_Solid3D;
		ID3DX11EffectTechnique*			m_pFX_Tech_Solid2D;
		ID3DX11EffectTechnique*			m_pFX_Tech_Textured2D;
		ID3DX11EffectTechnique*			m_pFX_Tech_DrawText2D;
		ID3DX11EffectTechnique*			m_pFX_Tech_DrawSky;

	};
}
