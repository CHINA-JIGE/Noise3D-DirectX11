
/***********************************************************************

                           h£ºIRenderer
				desc: transfer data to Graphic memory
				and use gpu to render

************************************************************************/

#pragma once

#include "_BaseRenderModule.h"
#include "RenderInfrastructure.h"
#include "Renderer_Atmosphere.h"
#include "Renderer_GraphicObj.h"
#include "Renderer_Mesh.h"
#include "Renderer_Text.h"

namespace Noise3D
{

	class /*_declspec(dllexport)*/ IRenderer :
		private IFactory<IRenderInfrastructure>,
		protected IRenderModuleForAtmosphere,
		protected IRenderModuleForGraphicObject,
		protected IRenderModuleForMesh,
		protected IRenderModuleForText
	{
	public:

		void		AddToRenderQueue(IMesh* obj);

		void		AddToRenderQueue(IGraphicObject* obj);

		void		AddToRenderQueue(IDynamicText* obj);

		void		AddToRenderQueue(IStaticText* obj);

		void		SetActiveAtmosphere(IAtmosphere* obj);

		void		Render();//render object in a fixed order

		void		ClearBackground(const NVECTOR4& color = NVECTOR4(0, 0, 0, 0.0f));

		void		PresentToScreen();

		UINT	GetBackBufferWidth();

		UINT	GetBackBufferHeight();

		void		SetPostProcessingEnabled(bool isEnabled);

	private:

		//extern init by IScene
		bool	NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_Init(UINT bufferWidth, UINT bufferHeight, bool IsWindowed);

	private:

		friend IScene;//for external init

		friend IFactory<IRenderer>;

		IRenderer();

		~IRenderer();

		IRenderInfrastructure* m_pRenderInfrastructure;

	};
}