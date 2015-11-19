
/***********************************************************************

                           h：NoiseRenderer3D
					主要功能 ：负责渲染

************************************************************************/

#pragma once


public class _declspec(dllexport) NoiseRenderer : private NoiseFileManager
{

	friend class NoiseScene;
	/*friend class NoiseMesh;
	friend class NoiseGraphicObject;
	friend class NoiseGUIManager;
	friend class NoiseAtmosphere;
	friend class Noise2DTextDynamic;
	friend class Noise2DTextStatic;*/

public:
	//构造函数
	NoiseRenderer();

	void			SelfDestruction();

	void			RenderMeshInList();

	void			RenderGraphicObjectInList();

	void			RenderAtmosphereInList();

	void			RenderText2DInList();

	void			AddOjectToRenderList(NoiseMesh& obj);
	void			AddOjectToRenderList(NoiseGraphicObject& obj);
	void			AddOjectToRenderList(NoiseAtmosphere& obj);
	void			AddOjectToRenderList(NoiseGUIButton& obj);
	void			AddOjectToRenderList(NoiseGUIScrollBar& obj);
	void			AddOjectToRenderList(NoiseGUIText& obj);
	void			AddOjectToRenderList(Noise2DTextDynamic& obj);
	void			AddOjectToRenderList(Noise2DTextStatic& obj);

	void			ClearBackground(NVECTOR4 color = NVECTOR4(0,0,0,0.0f));

	void			RenderToScreen();

	void			SetFillMode(NOISE_FILLMODE iMode);

	void			SetCullMode(NOISE_CULLMODE iMode);

	void			SetBlendingMode(NOISE_BLENDMODE iMode);

private:
	BOOL			mFunction_Init();

	BOOL			mFunction_Init_CreateBlendState();

	BOOL			mFunction_Init_CreateRasterState();

	BOOL			mFunction_Init_CreateSamplerState();

	BOOL			mFunction_Init_CreateDepthStencilState();

	BOOL			mFunction_Init_CreateEffectFromFile(LPCWSTR fxPath);

	BOOL			mFunction_Init_CreateEffectFromMemory(char* compiledShaderPath);

	//..........
	void				mFunction_SetRasterState(NOISE_FILLMODE iFillMode,NOISE_CULLMODE iCullMode);

	void				mFunction_SetBlendState(NOISE_BLENDMODE iBlendMode);

	void				mFunction_CameraMatrix_Update();

	void				mFunction_RenderMeshInList_UpdateCbPerObject();

	void				mFunction_RenderMeshInList_UpdateCbPerFrame();

	void				mFunction_RenderMeshInList_UpdateCbPerSubset(UINT subsetID);

	void				mFunction_RenderMeshInList_UpdateCbRarely();

	//render graphic object
	void				mFunction_CommonGraphicObj_Update_RenderTextured2D(UINT TexID);

	void				mFunction_CommonGraphicObj_RenderLine3DInList();

	void				mFunction_CommonGraphicObj_RenderPoint3DInList();

	void				mFunction_CommonGraphicObj_RenderLine2DInList();

	void				mFunction_CommonGraphicObj_RenderPoint2DInList();

	void				mFunction_CommonGraphicObj_RenderTriangle2DInList();

	void				mFunction_TextGraphicObj_Update_TextInfo(UINT texID,NoiseTextureManager* pTexMgr,N_CbDrawText2D& cbText);
	
	void				mFunction_TextGraphicObj_RenderTriangles();

	void				mFunction_Atmosphere_Fog_Update();

	void				mFunction_Atmosphere_SkyDome_Update();

	void				mFunction_Atmosphere_SkyBox_Update();

	void				mFunction_Atmosphere_UpdateCbAtmosphere();

	//validate Mat/Tex ID in case of 'out of range' error
	UINT				mFunction_ValidateTextureID_UsingTexMgr(UINT texID, NOISE_TEXTURE_TYPE texType=NOISE_TEXTURE_TYPE_COMMON);

	UINT				mFunction_ValidateMaterialID_UsingMatMgr(UINT matID);



private:
	std::vector <NoiseMesh*>*				m_pRenderList_Mesh;
	std::vector	<NoiseGraphicObject*>* 	m_pRenderList_CommonGraphicObject;
	std::vector<Noise2DTextDynamic*>*	m_pRenderList_TextDynamic;//specially set for text rendering
	std::vector<Noise2DTextStatic*>*		m_pRenderList_TextStatic;//specially set for text rendering
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