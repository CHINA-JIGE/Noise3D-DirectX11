
/***********************************************************************

                           h£ºrender module for atmosphere

************************************************************************/

#pragma once

namespace Noise3D
{
	class IRenderer;

	class /*_declspec(dllexport)*/ IRenderModuleForAtmosphere:
		IBaseRenderModule
	{
	public:

		void	SetActiveAtmosphere(IAtmosphere* pAtmo);

	protected:

		//"protected" : allow IRenderer to construct each render module
		// but simultaneously, not allow direct class construction by other user
		IRenderModuleForAtmosphere();

		~IRenderModuleForAtmosphere();

		void		RenderAtmosphere();

		virtual void	ClearRenderList() override;

		//called by IRenderer
		virtual bool	Initialize(IRenderInfrastructure* pRI, IShaderVariableManager* pShaderVarMgr) override;

	private:

		void		mFunction_Atmosphere_UpdateFogParameters(IAtmosphere*const pAtmo);

		void		mFunction_Atmosphere_UpdateSkyParameters(IAtmosphere*const pAtmo, bool& outEnabledSkybox, bool& outEnabledSkydome);


		std::vector <IAtmosphere*>	mRenderList_Atmosphere; //list of object to be rendererd

		ID3DX11EffectTechnique*	m_pFX_Tech_DrawSky;

		IRenderInfrastructure*			m_pRefRI;//common D3D operations/states

		IShaderVariableManager*	m_pRefShaderVarMgr;
	};
}
