
/***********************************************************************

                           h£ºrender module for Shadow Map (pass 1)

************************************************************************/

#pragma once

namespace Noise3D
{
	class Renderer;

	class /*_declspec(dllexport)*/ IRenderModuleForShadowMap :
		public IBaseRenderModule
	{
	public:

	protected:

		virtual void	ClearRenderList() override;

		virtual bool	Initialize(IRenderInfrastructure* pRI, IShaderVariableManager* pShaderVarMgr) override;

	private:

		std::shared_ptr<IRenderInfrastructure*>	m_pRefRI;//common D3D operations/states

		IShaderVariableManager*	m_pRefShaderVarMgr;
	};

}
