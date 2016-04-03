
/***********************************************************************

                           h：NoiseRenderer3D
						desc: assemble data and use gpu to render

************************************************************************/

#pragma once

//-------------CONSTANT BUFFER STRUCTURE----------------
//GPU Memory : 128 byte alignment
struct N_CbPerFrame
{
	N_DirectionalLight	mDirectionalLight_Dynamic[10];//放心 已经对齐了
	N_PointLight				mPointLight_Dynamic[10];
	N_SpotLight				mSpotLight_Dynamic[10];
	int			mDirLightCount_Dynamic;
	int			mPointLightCount_Dynamic;
	int			mSpotLightCount_Dynamic;
	BOOL		mIsLightingEnabled_Dynamic;
	NVECTOR3	mCamPos;	float mPad1;

};

struct N_CbPerObject
{
	NMATRIX	mWorldMatrix;
	NMATRIX	mWorldInvTransposeMatrix;
};

struct N_CbPerSubset
{
	N_Material_Basic	basicMaterial;
	BOOL			IsDiffuseMapValid;
	BOOL			IsNormalMapValid;
	BOOL			IsSpecularMapValid;
	BOOL			IsEnvironmentMapValid;
};

struct N_CbRarely
{
	//———————static light————————
	N_DirectionalLight mDirectionalLight_Static[50];
	N_PointLight	 mPointLight_Static[50];
	N_SpotLight	mSpotLight_Static[50];
	int		mDirLightCount_Static;
	int		mPointLightCount_Static;
	int		mSpotLightCount_Static;
	int		mIsLightingEnabled_Static;
};

struct N_CbCameraMatrix
{
	NMATRIX mProjMatrix;
	NMATRIX	mViewMatrix;
};

struct N_CbAtmosphere
{
	NVECTOR3	mFogColor;
	int				mIsFogEnabled;
	float				mFogNear;
	float				mFogFar;
	int				mIsSkyDomeValid;
	int				mIsSkyBoxValid;
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

namespace NGlobal
{
	const UINT c_VBstride_Default = sizeof(N_DefaultVertex);		//VertexBuffer的每个元素的字节跨度
	const UINT c_VBstride_Simple = sizeof(N_SimpleVertex);
	const UINT c_VBoffset = 0;				//VertexBuffer顶点序号偏移 因为从头开始所以offset是0
}


class _declspec(dllexport) NoiseRenderer : 
	private NoiseFileManager, 
	public NoiseClassLifeCycle
{

	friend class NoiseScene;

public:
	//构造函数
	NoiseRenderer();

	void			RenderMeshes();

	void			RenderGraphicObjects();

	void			RenderAtmosphere();

	void			RenderTexts();

	void			RenderGUIObjects();//this function will render if Noise GUI is involved

	void			AddObjectToRenderList(NoiseMesh& obj);

	void			AddObjectToRenderList(NoiseGraphicObject& obj, NOISE_RENDERER_ADDTOLIST_OBJ_TYPE objType = NOISE_RENDERER_ADDTOLIST_OBJ_TYPE_COMMON_OBJECT);

	void			AddObjectToRenderList(NoiseAtmosphere& obj);

	void			AddObjectToRenderList(Noise2DTextDynamic& obj, NOISE_RENDERER_ADDTOLIST_OBJ_TYPE objType = NOISE_RENDERER_ADDTOLIST_OBJ_TYPE_COMMON_OBJECT);

	void			AddObjectToRenderList(Noise2DTextStatic& obj, NOISE_RENDERER_ADDTOLIST_OBJ_TYPE objType = NOISE_RENDERER_ADDTOLIST_OBJ_TYPE_COMMON_OBJECT);

	void			ClearBackground(NVECTOR4 color = NVECTOR4(0,0,0,0.0f));

	void			RenderToScreen();

	void			SetFillMode(NOISE_FILLMODE iMode);

	void			SetCullMode(NOISE_CULLMODE iMode);

	void			SetBlendingMode(NOISE_BLENDMODE iMode);



private:

	void				Destroy();

	BOOL			mFunction_Init();

	BOOL			mFunction_Init_CreateBlendState();

	BOOL			mFunction_Init_CreateRasterState();

	BOOL			mFunction_Init_CreateSamplerState();

	BOOL			mFunction_Init_CreateDepthStencilState();

	BOOL			mFunction_Init_CreateEffectFromFile(NFilePath fxPath);

	BOOL			mFunction_Init_CreateEffectFromMemory(char* compiledShaderPath);

	//--------------------BASIC OPERATION---------------------
	void				mFunction_AddToRenderList_GraphicObj(NoiseGraphicObject* pGraphicObj, std::vector<NoiseGraphicObject*>* pList);

	void				mFunction_AddToRenderList_Text(Noise2DBasicTextInfo* pText, std::vector<Noise2DBasicTextInfo*>* pList);

	void				mFunction_SetRasterState(NOISE_FILLMODE iFillMode,NOISE_CULLMODE iCullMode);

	void				mFunction_SetBlendState(NOISE_BLENDMODE iBlendMode);

	void				mFunction_CameraMatrix_Update(NoiseCamera* const pCamera);


	//----------------MESHES-----------------------
	void				mFunction_RenderMeshInList_UpdateCbPerObject(NoiseMesh* const pMesh);

	void				mFunction_RenderMeshInList_UpdateCbPerFrame(NoiseCamera*const pCamera);

	void				mFunction_RenderMeshInList_UpdateCbPerSubset(NoiseMesh* const pMesh,UINT subsetID);//return subset primitive count

	void				mFunction_RenderMeshInList_UpdateCbRarely();


	//----------------GRAPHIC OBJECT-----------------------
	void				mFunction_GraphicObj_Update_RenderTextured2D(UINT TexID);

	void				mFunction_GraphicObj_RenderLine3DInList(NoiseCamera*const pCamera,std::vector<NoiseGraphicObject*>* pList);

	void				mFunction_GraphicObj_RenderPoint3DInList(NoiseCamera*const pCamera,std::vector<NoiseGraphicObject*>* pList);

	void				mFunction_GraphicObj_RenderLine2DInList(std::vector<NoiseGraphicObject*>* pList);

	void				mFunction_GraphicObj_RenderPoint2DInList(std::vector<NoiseGraphicObject*>* pList);

	void				mFunction_GraphicObj_RenderTriangle2DInList(std::vector<NoiseGraphicObject*>* pList);


	//----------------TEXT-----------------------
	void				mFunction_TextGraphicObj_Update_TextInfo(UINT texID,NoiseTextureManager* pTexMgr,N_CbDrawText2D& cbText);
	
	void				mFunction_TextGraphicObj_Render(std::vector<Noise2DBasicTextInfo*>* pList);


	//----------------ATMOSPHERE-----------------------
	void				mFunction_Atmosphere_Fog_Update(NoiseAtmosphere*const pAtmo);

	void				mFunction_Atmosphere_SkyDome_Update(NoiseAtmosphere*const pAtmo,UINT& outSkyDomeTexID);

	void				mFunction_Atmosphere_SkyBox_Update(NoiseAtmosphere*const pAtmo, UINT& outSkyBoxTexID);

	void				mFunction_Atmosphere_UpdateCbAtmosphere(NoiseAtmosphere*const pAtmo, UINT skyDomeTexID,UINT skyBoxTexID);

private:
	std::vector <NoiseMesh*>*				m_pRenderList_Mesh;
	std::vector	<NoiseGraphicObject*>* 	m_pRenderList_CommonGraphicObj;//for user-defined graphic obj rendering
	std::vector<NoiseGraphicObject*>*	m_pRenderList_GUIGraphicObj;//for GUI common object rendering
	std::vector<Noise2DBasicTextInfo*>*m_pRenderList_GUIText;//internal Text Object
	std::vector<Noise2DBasicTextInfo*>*m_pRenderList_TextDynamic;//for dynamic Text Rendering(including other info)
	std::vector<Noise2DBasicTextInfo*>*m_pRenderList_TextStatic;//for static Text Rendering(including other info)
	std::vector	<NoiseAtmosphere*>*		m_pRenderList_Atmosphere;

	//Raster State
	ID3D11RasterizerState*					m_pRasterState_Solid_CullNone;
	ID3D11RasterizerState*					m_pRasterState_Solid_CullBack;
	ID3D11RasterizerState*					m_pRasterState_Solid_CullFront;
	ID3D11RasterizerState*					m_pRasterState_WireFrame_CullFront;
	ID3D11RasterizerState*					m_pRasterState_WireFrame_CullNone;
	ID3D11RasterizerState*					m_pRasterState_WireFrame_CullBack;
	ID3D11BlendState*						m_pBlendState_Opaque;
	ID3D11BlendState*						m_pBlendState_AlphaTransparency;
	ID3D11BlendState*						m_pBlendState_ColorAdd;
	ID3D11BlendState*						m_pBlendState_ColorMultiply;
	ID3D11DepthStencilState*				m_pDepthStencilState_EnableDepthTest;
	ID3D11DepthStencilState*				m_pDepthStencilState_DisableDepthTest;
	ID3D11SamplerState*					m_pSamplerState_FilterAnis;

	NoiseScene*									m_pFatherScene ;
	NOISE_FILLMODE							m_FillMode;//填充模式
	NOISE_CULLMODE						m_CullMode;//剔除模式
	NOISE_BLENDMODE						m_BlendMode;

	//在App中先定义好所有Struct再一次更新
	BOOL										mCanUpdateCbCameraMatrix;
	N_CbPerFrame							m_CbPerFrame;
	N_CbPerObject							m_CbPerObject;
	N_CbPerSubset							m_CbPerSubset;
	N_CbRarely								m_CbRarely;
	N_CbCameraMatrix					m_CbCameraMatrix;
	N_CbAtmosphere						m_CbAtmosphere;
	N_CbDrawText2D						m_CbDrawText2D;

	//用于从app更新到Gpu的接口
	ID3DX11Effect*							m_pFX;
	ID3DX11EffectTechnique*			m_pFX_Tech_Default;
	ID3DX11EffectTechnique*			m_pFX_Tech_Solid3D;
	ID3DX11EffectTechnique*			m_pFX_Tech_Solid2D;
	ID3DX11EffectTechnique*			m_pFX_Tech_Textured2D;
	ID3DX11EffectTechnique*			m_pFX_Tech_DrawText2D;
	ID3DX11EffectTechnique*			m_pFX_Tech_DrawSky;
	ID3DX11EffectConstantBuffer* m_pFX_CbPerObject;
	ID3DX11EffectConstantBuffer*	m_pFX_CbPerFrame;
	ID3DX11EffectConstantBuffer*	m_pFX_CbPerSubset;
	ID3DX11EffectConstantBuffer*	m_pFX_CbRarely;
	ID3DX11EffectConstantBuffer*	m_pFX_CbSolid3D;
	ID3DX11EffectConstantBuffer*	m_pFX_CbAtmosphere;
	ID3DX11EffectConstantBuffer* m_pFX_CbDrawText2D;
	ID3DX11EffectShaderResourceVariable* m_pFX_Texture_Diffuse;
	ID3DX11EffectShaderResourceVariable* m_pFX_Texture_Normal;
	ID3DX11EffectShaderResourceVariable* m_pFX_Texture_Specular;
	ID3DX11EffectShaderResourceVariable* m_pFX_Texture_CubeMap;
	ID3DX11EffectShaderResourceVariable* m_pFX2D_Texture_Diffuse;
	ID3DX11EffectSamplerVariable*	m_pFX_SamplerState_Default;
};

