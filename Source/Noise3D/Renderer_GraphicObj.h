
/***********************************************************************

                           h£ºrender module for graphic Object

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ IRenderModuleForGraphicObject:
		public IBaseRenderModule
	{
	public:
	
		void	AddToRenderQueue(GraphicObject* obj);

	protected:
		//"protected" : allow Renderer to construct each render module
		// but simultaneously, not allow direct class construction by other user
		IRenderModuleForGraphicObject();

		~IRenderModuleForGraphicObject();

		void		RenderGraphicObjects();

		virtual void	ClearRenderList() override;

		//called by Renderer
		virtual bool	Initialize(IRenderInfrastructure* pRI, IShaderVariableManager* pShaderVarMgr) override;

	private:

		void		mFunction_AddToRenderList_GraphicObj(GraphicObject* pGraphicObj, std::vector<GraphicObject*>* pList);

		void		mFunction_GraphicObj_Update_RenderTextured2D(N_UID texName);

		void		mFunction_GraphicObj_RenderLine3DInList(Camera*const pCamera, GraphicObject* pGObj);

		void		mFunction_GraphicObj_RenderPoint3DInList(Camera*const pCamera, GraphicObject* pGObj);

		void		mFunction_GraphicObj_RenderLine2D(GraphicObject* pGObj);

		void		mFunction_GraphicObj_RenderPoint2D(GraphicObject* pGObj);

		void		mFunction_GraphicObj_RenderTriangle2D(GraphicObject* pGObj);

		std::vector <GraphicObject*>	mRenderList_GO; //list of object to be rendererd

		ID3DX11EffectTechnique*		m_pFX_Tech_Solid3D;

		ID3DX11EffectTechnique*		m_pFX_Tech_Solid2D;

		ID3DX11EffectTechnique*		m_pFX_Tech_Textured2D;

		IRenderInfrastructure*				m_pRefRI;//common D3D operations/states

		IShaderVariableManager*		m_pRefShaderVarMgr;

	};
}
