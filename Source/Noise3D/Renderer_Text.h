
/***********************************************************************

                           h£ºrender module for Text

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ IRenderModuleForText :
		IBaseRenderModule
	{
	public:

		void	AddToRenderQueue(IDynamicText* obj);

		void	AddToRenderQueue(IStaticText* obj);

	protected:

		//"protected" : allow IRenderer to construct each render module
		// but simultaneously, not allow direct class construction by other user
		IRenderModuleForText();

		~IRenderModuleForText();

		void		RenderTexts();

		virtual void	ClearRenderList() override;

		virtual bool	Initialize(IRenderInfrastructure* pRI, IShaderVariableManager* pShaderVarMgr) override;

	private:

		void		mFunction_AddToRenderList_Text(IBasicTextInfo* pText, std::vector<IBasicTextInfo*>* pList);

		void		mFunction_TextGraphicObj_Update_TextInfo(N_UID uid, ITextureManager* pTexMgr, IBasicTextInfo* pText);

		void		mFunction_TextGraphicObj_Render(std::vector<IBasicTextInfo*>* pList);

		std::vector <IBasicTextInfo*>		mRenderList_TextDynamic;//for dynamic Text Rendering(including other info)
		
		std::vector <IBasicTextInfo*>		mRenderList_TextStatic;//for static Text Rendering(including other info)
	
		ID3DX11EffectTechnique*	m_pFX_Tech_Solid2D;

		ID3DX11EffectTechnique*	m_pFX_Tech_DrawText2D;

		IRenderInfrastructure*			m_pRefRI;//common D3D operations/states

		IShaderVariableManager*	m_pRefShaderVarMgr;

		IRenderer*		m_pFatherRenderer;//re-use graphic object render
	};
}
