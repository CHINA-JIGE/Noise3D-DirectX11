
/***********************************************************************

               h£ºrender module for special effect : Sweeping Trail

************************************************************************/

#pragma once

namespace Noise3D
{
	class IRenderer;

	class /*_declspec(dllexport)*/ IRenderModuleForSweepingTrailFX:
		IBaseRenderModule
	{
	public:

		void	AddToRenderQueue(ISweepingTrail* pTrail);

	protected:

		//"protected" : allow IRenderer to construct each render module
		// but simultaneously, not allow direct class construction by other user
		IRenderModuleForSweepingTrailFX();

		~IRenderModuleForSweepingTrailFX();

		void		RenderSweepingTrails();

		virtual void	ClearRenderList() override;

		//called by IRenderer
		virtual bool	Initialize(IRenderInfrastructure* pRI, IShaderVariableManager* pShaderVarMgr) override;

	private:

		void		mFunction_SweepingTrail_Update(ISweepingTrail*const pTrail);

		std::vector <ISweepingTrail*>	mRenderList_SweepingTrails; //list of object to be rendererd

		ID3DX11EffectTechnique*	m_pFX_DrawSolid3D;//(2018.7.24)Currently Sweeping trails don't need customized shader

		IRenderInfrastructure*			m_pRefRI;//common D3D operations/states

		IShaderVariableManager*	m_pRefShaderVarMgr;
	};
}
