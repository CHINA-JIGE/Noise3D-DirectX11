
/***********************************************************************

                           h£ºrender module for mesh

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ IRenderModuleForMesh :
		IBaseRenderModule
	{
	public:

		void	AddToRenderQueue(IMesh* pMesh);

	protected:

		//"protected" : allow IRenderer to construct each render module
		// but simultaneously, not allow direct class construction by other user
		IRenderModuleForMesh();

		~IRenderModuleForMesh();

		void		RenderMeshes();

		virtual void	ClearRenderList() override;

		virtual bool	Initialize(IRenderInfrastructure* pRI, IShaderVariableManager* pShaderVarMgr) override;

	private:

		void		mFunction_RenderMeshInList_UpdatePerObject(IMesh* const pMesh);

		void		mFunction_RenderMeshInList_UpdatePerFrame();

		ID3DX11EffectPass*		mFunction_RenderMeshInList_UpdatePerSubset(IMesh* const pMesh, UINT subsetID);//return subset primitive count

		void		mFunction_RenderMeshInList_UpdateRarely();

		std::vector <IMesh*>			mRenderList_Mesh; //list of object to be rendererd

		ID3DX11EffectTechnique*	m_pFX_Tech_DrawMesh;

		IRenderInfrastructure*			m_pRefRI;//common D3D operations/states

		IShaderVariableManager*	m_pRefShaderVarMgr;

	};
}
