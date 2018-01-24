
/***********************************************************************

                           h£ºIRenderer
				desc: transfer data to Graphic memory
				and use gpu to render

************************************************************************/
#pragma once

namespace Noise3D
{
	//some interfaces must be implemented (pure virtual is necessary, in case i forget- -)
	class IBaseRenderModule
	{
	public:

		//void addToList();

		//void Render();

	protected:

		virtual void ClearRenderList() = 0;

		virtual void Initialize(IRenderInfrastructure* pRI, IShaderVariableManager* pShaderVarMgr) = 0;
	};
}