
/***********************************************************************

                           h£ºrender module for post-processing
				post-processing: render to a new

************************************************************************/

#pragma once

namespace Noise3D
{
	//Steps to Add one more post process effect
	//1.declare post process parameter description(struct N_PostProcessXXXXDesc)
	//2.declare 'AddToPostProcessList' public interface
	//3.declare ID3DX11EffectPass
	//4.add constructor&destructor of effect pass
	//5.add init name binding of effect pass
	//6.add corresponding private function to issue draw call
	//7.add a switch case to 'PostProcess()' to re-interpret a description
	//8.add ClearRenderList 

	struct N_PostProcessGreyScaleDesc
	{
		float factorR;
		float factorG;
		float factorB;
	};

	struct N_PostProcesQwertyDistortionDesc
	{
		ICamera* pCamera;
		IMesh* pScreenDescriptor;
	};


	//************************************************************************
	class /*_declspec(dllexport)*/ IRenderModuleForPostProcessing :
		IBaseRenderModule
	{
	public:

		void		AddToPostProcessList_GreyScale(const N_PostProcessGreyScaleDesc& param);

		//perspective-correct render : pass 2 of Qwerty 3D(another high level project of mine)
		void		AddToPostProcessList_QwertyDistortion(const N_PostProcesQwertyDistortionDesc& param);

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

		//This is a C-style implementation of an array of variants(post-process effect with different types)
		//re-interpret effect description according to post-process effect type
		//Some Thought:
		//Actually I have thought about how to store different types of description strucut
		//into one list, or just directly remove post-processing list.BUT:
		//An post process tasks queue is still necessary, or the RTVs can't be configured properly,
		//	It's not a good way to let user initiate each post-process immediately
		//	(1 back buffer A+ 2 off-screen render targets B C, A for displaying via swap chain,
		//	B, C for Render-To-Texture techniques, but B,C could serve as SRV or RTV in differnet pass)
		//but how to store different types of description struct in a queue?
		//		1.one vector for one desc type, + a list of Pair which store the mapping info to query target desc
		//		2.make use of polymorphism, a list of IBaseDesc*, dynamic_cast<> later
		//		3.C-style implementation, re-interpret according to a type info enum later(a little uncomfortable...)
		//		4.C-style implmentation, union of different desc, but unfortunately, a union can't have
		//			members with constructor/destructor
		//(2018.2.1)At last i choose 1. 
		//1 seems complicated...lazy. But compares to other, this is more acceptable..
		//2 is more c++ and 'elegant', but the IBaseDesc* should be an l-value that is NEWed, but a desc is 
		//	just a struct with a few params, i don't want to make a factory method for each of them..
		//3 C-style implementation of "Array of Variant".but the void* still points to memory that Noise3d 'news'.
		//		which is complex,dangerous,
		//4.not feasible
		struct N_PostProcessPass
		{
			NOISE_POST_PROCESS_EFFECT type;
			int index;//index in corresponding list
		};

		bool		mFunction_Init_VertexBufferOfQuad();//full-screen quad

		bool		mFunction_Init_CreateOffScreenRTV(UINT bufferWidth, UINT bufferHeight, UINT cMsaaSampleCount);

		bool		mFunction_Init_CreateOffScreenDSV(UINT bufferWidth, UINT bufferHeight, UINT cMsaaSampleCount);

		void		mFunction_SetInputShaderResource();

		void		mFunction_GreyScale(const N_PostProcessGreyScaleDesc& param);

		void		mFunction_QwertyDistortion(const N_PostProcesQwertyDistortionDesc& param);

		//post process pass info (post process effects can be ordered in different way)
		std::list<N_PostProcessPass>							mPostProcessEffectQueue;
		std::vector<N_PostProcessGreyScaleDesc>			mGreyScaleDescList;
		std::vector<N_PostProcesQwertyDistortionDesc> mQwertyDescList;

		//infrastructure
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