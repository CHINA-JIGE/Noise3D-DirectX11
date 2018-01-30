
/***********************************************************************

                           h£ºrender module for post-processing
				post-processing: render to a new

************************************************************************/

#pragma once

namespace Noise3D
{
	//Steps to Add one more post process effect
	//1.declare post process parameter description
	//2.declare 'Enqueue' public interface
	//3.declare ID3DX11EffectPass
	//4.add constructor&destructor of effect pass
	//5.add init name binding of effect pass
	//6.add corresponding private function to issue draw call
	//7.add a case to 'PostProcess()' to re-interpret a description

	struct N_PostProcessGreyScaleDesc
	{
		float factorR;
		float factorG;
		float factorB;
	};

	struct N_PostProcesQwertyDistortionDesc
	{
		NVECTOR3 camPos;
		IMesh* pScreenDescriptor;
	};


	//************************************************************************
	class /*_declspec(dllexport)*/ IRenderModuleForPostProcessing :
		IBaseRenderModule
	{
	public:

		void		EnqueuePostProcessEffect_GreyScale(const N_PostProcessGreyScaleDesc& param);

		//perspective-correct render : pass 2 of Qwerty 3D(another high level project of mine)
		void		EnqueuePostProcessEffect_QwertyDistortion(const N_PostProcesQwertyDistortionDesc& param);

	protected:

		//"protected" : allow IRenderer to construct each render module
		// but simultaneously, not allow direct class construction by other user
		IRenderModuleForPostProcessing();

		~IRenderModuleForPostProcessing();

		void			PostProcess();

		uint32_t	GetPostProcessPassCount();

		virtual void	ClearRenderList() override;

		virtual bool	Initialize(IRenderInfrastructure* pRI, IShaderVariableManager* pShaderVarMgr) override;

	private:

		enum NOISE_POST_PROCESS_EFFECT
		{
			GreyScale,
			QwertyDistortion
		};

		//re-interpret effect description according to post-process effect type
		struct N_PostProcessPass
		{
			N_PostProcessPass(){}

			NOISE_POST_PROCESS_EFFECT type;
			union
			{
				N_PostProcessGreyScaleDesc greyScale;
				N_PostProcesQwertyDistortionDesc qwerty;
			} desc;
		};

		bool		mFunction_Init_VertexBufferOfQuad();

		bool		mFunction_Init_CreateRenderToTextureViews(UINT bufferWidth, UINT bufferHeight, UINT cMsaaSampleCount);

		void		mFunction_SetInputShaderResource();

		void		mFunction_GreyScale(const N_PostProcessGreyScaleDesc& param);

		void		mFunction_QwertyDistortion(const N_PostProcesQwertyDistortionDesc& param);

		//post process pass info (different effect can be ordered in different way)
		std::vector<N_PostProcessPass>	mPostProcessEffectQueue;

		IRenderInfrastructure*			m_pRefRI;//common D3D operations/states

		IShaderVariableManager*	m_pRefShaderVarMgr;

		ID3DX11EffectTechnique*	m_pFX_Tech_PostProcessing;

		ID3DX11EffectPass*		m_pFX_Pass_GreyScale;
		ID3DX11EffectPass*		m_pFX_Pass_Qwerty;

		//because one texture can't be bound to pipeline simultaneously,
		//multiple post processing effect must use 2 off-screen texture to 
		//bounce back and forth (bind one as shader input while another as render output)
		//and finally, bind Back Buffer of swap chain as the final render output
		ID3D11RenderTargetView*			m_pOffScreenRenderTargetView_A;
		ID3D11ShaderResourceView*		m_pOffScreenShaderResourceView_A;
		ID3D11DepthStencilView*			m_pOffScreenDepthStencilView_A;
		
		ID3D11RenderTargetView*			m_pOffScreenRenderTargetView_B;
		ID3D11ShaderResourceView*		m_pOffScreenShaderResourceView_B;
		ID3D11DepthStencilView*			m_pOffScreenDepthStencilView_B;

		//a vertex that have 6 vertices to draw a 2D full-screen Quad/Rectangle
		//so that post-process can be initiated by one draw call.
		static const uint32_t	cQuadVertexCount = 6;
		ID3D11Buffer*				m_pVB_Quad;
	};
}