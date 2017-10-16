
/***********************************************************************

                           h：IRenderer
				desc: transfer data to Graphic memory
				and use gpu to render

************************************************************************/

#pragma once

namespace Noise3D
{
	//Constant Buffer Structure, GPU Memory : 128 byte alignment
	struct N_CbPerFrame
	{
		N_DirLightDesc		mDirectionalLight_Dynamic[10];//放心 已经对齐了
		N_PointLightDesc	mPointLight_Dynamic[10];
		N_SpotLightDesc		mSpotLight_Dynamic[10];
		int			mDirLightCount_Dynamic;
		int			mPointLightCount_Dynamic;
		int			mSpotLightCount_Dynamic;
		int			mIsLightingEnabled_Dynamic;
	};

	struct N_CbPerObject
	{
		NMATRIX	mWorldMatrix;
		NMATRIX	mWorldInvTransposeMatrix;
	};

	struct N_CbPerSubset:public  N_BasicMaterialDesc
	{
		void SetBaseMat(const N_MaterialDesc& mat)
		{
			memcpy_s(this, sizeof(N_BasicMaterialDesc), &mat, sizeof(N_BasicMaterialDesc));
		}

		//only update these SWTICHES var to GPU, 
		//cos' texture show up as a shaderResource(update in another way)
		int			IsDiffuseMapValid;
		int			IsNormalMapValid;
		int			IsSpecularMapValid;
		int			IsEnvironmentMapValid;
	};

	struct N_CbRarely
	{
		//———————static light————————
		N_DirLightDesc		mDirectionalLight_Static[50];
		N_PointLightDesc	mPointLight_Static[50];
		N_SpotLightDesc		mSpotLight_Static[50];
		int		mDirLightCount_Static;
		int		mPointLightCount_Static;
		int		mSpotLightCount_Static;
		int		mIsLightingEnabled_Static;
	};

	struct N_CbCameraInfo
	{
		NMATRIX projMatrix;
		NMATRIX	viewMatrix;
		NMATRIX invProjMatrix;
		NMATRIX invViewMatrix;
		NVECTOR3	camPos;	float mPad1;
	};

	struct N_CbAtmosphere
	{
		NVECTOR3	mFogColor;
		int				mIsFogEnabled;
		float				mFogNear;
		float				mFogFar;
		float				mSkyBoxWidth;
		float				mSkyBoxHeight;
		float				mSkyBoxDepth;
		float				mPad1;
	};

	struct N_CbDrawText2D
	{
		NVECTOR4 	mTextColor;
		NVECTOR4	mTextGlowColor;
	};

	class /*_declspec(dllexport)*/ IRenderer :
		private IFileIO
	{
	friend class IScene;//father node

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

		bool			mFunction_Init(UINT BufferWidth, UINT BufferHeight, bool IsWindowed);

		bool			mFunction_Init_CreateSwapChainAndRTVandDSVandViewport(UINT BufferWidth, UINT BufferHeight, bool IsWindowed);//render target view, depth stencil view

		bool			mFunction_Init_CreateBlendState();

		bool			mFunction_Init_CreateRasterState();

		bool			mFunction_Init_CreateSamplerState();

		bool			mFunction_Init_CreateDepthStencilState();

		bool			mFunction_Init_CreateEffectFromFile(NFilePath fxPath);

		//--------------------BASIC OPERATION---------------------
		void				mFunction_AddToRenderList_GraphicObj(IGraphicObject* pGraphicObj, std::vector<IGraphicObject*>* pList);

		void				mFunction_AddToRenderList_Text(IBasicTextInfo* pText, std::vector<IBasicTextInfo*>* pList);

		void				mFunction_SetRasterState(NOISE_FILLMODE iFillMode, NOISE_CULLMODE iCullMode);

		void				mFunction_SetBlendState(NOISE_BLENDMODE iBlendMode);

		void				mFunction_CameraMatrix_Update(ICamera* const pCamera);


		//----------------MESHES-----------------------
		void				mFunction_RenderMeshInList_UpdateCbPerObject(IMesh* const pMesh);

		void				mFunction_RenderMeshInList_UpdateCbPerFrame(ICamera*const pCamera);

		void				mFunction_RenderMeshInList_UpdateCbPerSubset(IMesh* const pMesh, UINT subsetID);//return subset primitive count

		void				mFunction_RenderMeshInList_UpdateCbRarely();


		//----------------GRAPHIC OBJECT-----------------------
		void				mFunction_GraphicObj_Update_RenderTextured2D(N_UID texName);

		void				mFunction_GraphicObj_RenderLine3DInList(ICamera*const pCamera, std::vector<IGraphicObject*>* pList);

		void				mFunction_GraphicObj_RenderPoint3DInList(ICamera*const pCamera, std::vector<IGraphicObject*>* pList);

		void				mFunction_GraphicObj_RenderLine2DInList(std::vector<IGraphicObject*>* pList);

		void				mFunction_GraphicObj_RenderPoint2DInList(std::vector<IGraphicObject*>* pList);

		void				mFunction_GraphicObj_RenderTriangle2DInList(std::vector<IGraphicObject*>* pList);


		//----------------TEXT-----------------------
		void				mFunction_TextGraphicObj_Update_TextInfo(N_UID uid, ITextureManager* pTexMgr, N_CbDrawText2D& cbText);

		void				mFunction_TextGraphicObj_Render(std::vector<IBasicTextInfo*>* pList);


		//----------------ATMOSPHERE-----------------------
		void				mFunction_Atmosphere_Fog_Update(IAtmosphere*const pAtmo,ITextureManager* const pTexMgr);

		//true for enable Sky Dome
		bool				mFunction_Atmosphere_SkyDome_Update(IAtmosphere*const pAtmo, ITextureManager* const pTexMgr,N_UID& outSkyDomeTexName);

		//true for enable Sky Box
		bool				mFunction_Atmosphere_SkyBox_Update(IAtmosphere*const pAtmo, ITextureManager* const pTexMgr, N_UID& outSkyBoxTexName);

		void				mFunction_Atmosphere_UpdateCbAtmosphere(IAtmosphere*const pAtmo, ITextureManager* const pTexMgr,bool enableSkyBox, bool enableSkyDome, const N_UID& skyDomeTexName, const N_UID& skyBoxTexName);

	private:

		friend IFactory<IRenderer>;

		//构造函数
		IRenderer();

		~IRenderer();

		UINT				mMainBufferWidth;
		UINT				mMainBufferHeight;

		std::vector <IMesh*>*					m_pRenderList_Mesh;
		std::vector <IGraphicObject*>* 	m_pRenderList_CommonGraphicObj;//for user-defined graphic obj rendering
		std::vector <IBasicTextInfo*>*	m_pRenderList_TextDynamic;//for dynamic Text Rendering(including other info)
		std::vector <IBasicTextInfo*>*	m_pRenderList_TextStatic;//for static Text Rendering(including other info)
		std::vector <IAtmosphere*>*		m_pRenderList_Atmosphere;

		IDXGISwapChain*							m_pSwapChain;
		ID3D11RenderTargetView*				m_pRenderTargetView;//RTV
		ID3D11DepthStencilView*				m_pDepthStencilView;//DSV
		ID3D11RasterizerState*					m_pRasterState_Solid_CullNone;
		ID3D11RasterizerState*					m_pRasterState_Solid_CullBack;
		ID3D11RasterizerState*					m_pRasterState_Solid_CullFront;
		ID3D11RasterizerState*					m_pRasterState_WireFrame_CullFront;
		ID3D11RasterizerState*					m_pRasterState_WireFrame_CullNone;
		ID3D11RasterizerState*					m_pRasterState_WireFrame_CullBack;
		ID3D11BlendState*							m_pBlendState_Opaque;
		ID3D11BlendState*							m_pBlendState_AlphaTransparency;
		ID3D11BlendState*							m_pBlendState_ColorAdd;
		ID3D11BlendState*							m_pBlendState_ColorMultiply;
		ID3D11DepthStencilState*				m_pDepthStencilState_EnableDepthTest;
		ID3D11DepthStencilState*				m_pDepthStencilState_DisableDepthTest;
		ID3D11SamplerState*						m_pSamplerState_FilterLinear;
		

		//在App中先定义好所有Struct再一次更新
		bool												mCanUpdateCbCameraMatrix;
		N_CbPerFrame							m_CbPerFrame;
		N_CbPerObject							m_CbPerObject;
		N_CbPerSubset							m_CbPerSubset;
		N_CbRarely									m_CbRarely;
		N_CbCameraInfo							m_CbCameraInfo;
		N_CbAtmosphere						m_CbAtmosphere;
		N_CbDrawText2D						m_CbDrawText2D;

		//用于从app更新到Gpu的接口
		ID3DX11EffectTechnique*			m_pFX_Tech_Default;
		ID3DX11EffectTechnique*			m_pFX_Tech_Solid3D;
		ID3DX11EffectTechnique*			m_pFX_Tech_Solid2D;
		ID3DX11EffectTechnique*			m_pFX_Tech_Textured2D;
		ID3DX11EffectTechnique*			m_pFX_Tech_DrawText2D;
		ID3DX11EffectTechnique*			m_pFX_Tech_DrawSky;
		ID3DX11EffectConstantBuffer*	m_pFX_CbPerObject;
		ID3DX11EffectConstantBuffer*	m_pFX_CbPerFrame;
		ID3DX11EffectConstantBuffer*	m_pFX_CbPerSubset;
		ID3DX11EffectConstantBuffer*	m_pFX_CbRarely;
		ID3DX11EffectConstantBuffer*	m_pFX_CbSolid3D;
		ID3DX11EffectConstantBuffer*	m_pFX_CbAtmosphere;
		ID3DX11EffectConstantBuffer*	m_pFX_CbDrawText2D;
		ID3DX11EffectShaderResourceVariable* m_pFX_Texture_Diffuse;
		ID3DX11EffectShaderResourceVariable* m_pFX_Texture_Normal;
		ID3DX11EffectShaderResourceVariable* m_pFX_Texture_Specular;
		ID3DX11EffectShaderResourceVariable* m_pFX_Texture_CubeMap;
		ID3DX11EffectShaderResourceVariable* m_pFX2D_Texture_Diffuse;
		ID3DX11EffectSamplerVariable*	m_pFX_SamplerState_Default;
	};
}
