
/***********************************************************************

                           h：NoiseRenderer3D
					主要功能 ：负责渲染

************************************************************************/

#pragma once


public class _declspec(dllexport) NoiseRenderer : private NoiseFileManager
{

	friend class NoiseScene;
	friend class NoiseMesh;
	friend class NoiseGraphicObject;

public:
	//构造函数
	NoiseRenderer();

	void			SelfDestruction();

	void			RenderMeshInList();

	void			RenderGraphicObjectInList();

	void			ClearBackground(NVECTOR4 color = NVECTOR4(0,0.3f,0.3f,1.0f));

	void			RenderToScreen();

	void			SetFillMode(NOISE_FILLMODE iMode);

	void			SetCullMode(NOISE_CULLMODE iMode);


private:
	BOOL			mFunction_Init();

	BOOL			mFunction_Init_CreateEffectFromFile(LPCWSTR fxPath);

	BOOL			mFunction_Init_CreateEffectFromMemory(char* compiledShaderPath);

	void				mFunction_SetRasterState(NOISE_FILLMODE iFillMode,NOISE_CULLMODE iCullMode);

	void				mFunction_RenderMeshInList_UpdatePerObject();

	void				mFunction_RenderMeshInList_UpdatePerFrame();

	void				mFunction_RenderMeshInList_UpdatePerSubset(UINT subsetID);

	void				mFunction_RenderMeshInList_UpdateRarely();

	//render graphic object
	void				mFunction_GraphicObj_Update_RenderSolid3D();

	void				mFunction_GraphicObj_Update_RenderTextured2D(UINT TexID);

	void				mFunction_GraphicObj_RenderLine3DInList();

	void				mFunction_GraphicObj_RenderPoint3DInList();

	void				mFunction_GraphicObj_RenderLine2DInList();

	void				mFunction_GraphicObj_RenderPoint2DInList();

	void				mFunction_GraphicObj_RenderTriangle2DInList();


private:
	std::vector <NoiseMesh*>*				m_pRenderList_Mesh;
	std::vector<NoiseGraphicObject*>* 	m_pRenderList_GraphicObject;
	//光栅化设置
	ID3D11RasterizerState*					m_pRasterState_Solid_CullNone;
	ID3D11RasterizerState*					m_pRasterState_Solid_CullBack;
	ID3D11RasterizerState*					m_pRasterState_Solid_CullFront;
	ID3D11RasterizerState*					m_pRasterState_WireFrame_CullFront;
	ID3D11RasterizerState*					m_pRasterState_WireFrame_CullNone;
	ID3D11RasterizerState*					m_pRasterState_WireFrame_CullBack;

	NoiseScene*									m_pFatherScene ;
	NOISE_FILLMODE							m_FillMode;//填充模式
	NOISE_CULLMODE						m_CullMode;//剔除模式

	//在App中先定义好所有Struct再一次更新
	N_CbPerFrame							m_CbPerFrame;
	N_CbPerObject							m_CbPerObject;
	N_CbPerSubset							m_CbPerSubset;
	N_CbRarely								m_CbRarely;
	N_CbSolid3D								m_CbSolid3D;

	//用于从app更新到Gpu的接口
	ID3DX11Effect*							m_pFX;

	ID3DX11EffectTechnique*			m_pFX_Tech_Default;
	ID3DX11EffectTechnique*			m_pFX_Tech_Solid3D;
	ID3DX11EffectTechnique*			m_pFX_Tech_Solid2D;
	ID3DX11EffectTechnique*			m_pFX_Tech_Textured2D;

	ID3DX11EffectConstantBuffer* m_pFX_CbPerObject;
	ID3DX11EffectConstantBuffer*	m_pFX_CbPerFrame;
	ID3DX11EffectConstantBuffer*	m_pFX_CbPerSubset;
	ID3DX11EffectConstantBuffer*	m_pFX_CbRarely;
	ID3DX11EffectConstantBuffer*	m_pFX_CbSolid3D;

	ID3DX11EffectShaderResourceVariable* m_pFX_Texture_Diffuse;
	ID3DX11EffectShaderResourceVariable* m_pFX_Texture_Normal;
	ID3DX11EffectShaderResourceVariable* m_pFX_Texture_Specular;

	ID3DX11EffectShaderResourceVariable* m_pFX2D_Texture_Diffuse;
};