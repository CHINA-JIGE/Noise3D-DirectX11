
/***********************************************************************

                           h£ºBaseRenderModule
				desc: some common interface that must be 
				implemented by each render module

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

		virtual bool Initialize(IRenderInfrastructure* pRI, IShaderVariableManager* pShaderVarMgr) = 0;
	};
}