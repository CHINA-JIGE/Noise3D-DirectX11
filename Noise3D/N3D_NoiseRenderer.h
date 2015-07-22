
/***********************************************************************

                           h：NoiseRenderer
					主要功能 ：负责渲染

************************************************************************/

#pragma once


public class _declspec(dllexport) NoiseRenderer
{
	//scene要跟它管的类都互粉=v=
	friend class NoiseScene;
	friend class NoiseMesh;
	friend class NoiseLightManager;

public:
	//构造函数
	NoiseRenderer();
	~NoiseRenderer();

	void			RenderMeshInList();

	void			ClearViews();

	void			RenderToScreen();

	void			SetFillMode(NOISE_FILLMODE iMode);


private:
	BOOL			m_Function_Init();

	BOOL			m_Function_Init_CreateEffectFromFile();

	BOOL			m_Function_Init_CreateEffectFromMemory();

	void				m_Function_SetRasterState(NOISE_FILLMODE iMode);

	void				m_Function_RenderMeshInList_UpdateCbPerObject();

	void				m_Function_RenderMeshInList_UpdateCbPerFrame();

	void				m_Function_RenderMeshInList_UpdateCbPerSubset();

	void				m_Function_RenderMeshInList_UpdateCbRarely();


	NoiseScene*								m_pFatherScene ;
	NOISE_FILLMODE						m_FillMode;//填充模式
	//在App中先定义好所有Struct再一次更新
	N_CbPerFrame							m_CbPerFrame;
	N_CbPerObject							m_CbPerObject;
	N_CbPerSubset							m_CbPerSubset;
	N_CbRarely								m_CbRarely;

	//用于从app更新到Gpu的接口
	ID3DX11Effect*							m_pFX;
	ID3DX11EffectTechnique*			m_pFX_Tech_Basic;
	ID3DX11EffectConstantBuffer* m_pFX_CbPerObject;
	ID3DX11EffectConstantBuffer* m_pFX_CbPerFrame;
	ID3DX11EffectConstantBuffer*	m_pFX_CbPerSubset;
	ID3DX11EffectConstantBuffer*	m_pFX_CbRarely;
};