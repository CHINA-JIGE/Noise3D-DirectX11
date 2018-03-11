
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
#include "Renderer_PostProcessing.h"

namespace Noise3D
{
	class /*_declspec(dllexport)*/ IRenderer :
		private IFactory<IRenderInfrastructure>,
		public IRenderModuleForAtmosphere,
		public IRenderModuleForGraphicObject,
		public IRenderModuleForMesh,
		public IRenderModuleForText,
		public IRenderModuleForPostProcessing
	{
	public:

		//explicitly overload 'AddToRenderQueue' 
		//prevent functions with same names are HIDDEN

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

		HWND		GetRenderWindowHWND();

		uint32_t	GetRenderWindowWidth();

		uint32_t	GetRenderWindowHeight();

		void		SwitchToFullScreenMode();

		void		SwitchToWindowedMode();

		//void	BakeLightMapForMesh(IMesh* pMesh);//bake light map for static scenes,automatic uv flatten is required.

	private:

		//extern init by IScene
		bool	NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_Init(UINT bufferWidth, UINT bufferHeight, HWND renderWindowHandle);

	private:

		friend IScene;//for external init

		friend IFactory<IRenderer>;

		IRenderer();

		~IRenderer();

		IRenderInfrastructure* m_pRenderInfrastructure;

	};
}