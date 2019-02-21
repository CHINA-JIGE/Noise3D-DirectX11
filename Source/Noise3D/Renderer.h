
/***********************************************************************

                           h£ºRenderer
				desc: transfer data to Graphic memory
				and use gpu to render

		//procedure to integrate one more render module
		//1.define a render module class that inherit 'IRenderInfrastructure'
		//2. implement some of the interfaces(Initialize, ClearRenderList, RenderXXXX, AddToRenderQueue...)
				note that a ref-RI and ShaderVarMgr should be passed in. Technique should be initialized here too.)
		//3. write specific render setting and draw call in the render module ( make full use of RI and ShaderVarMgr)
		//3. Renderer inherit from IRenderModuleXXX
		//4. invoke "IRenderModuleXXX::initialize" in Renderer's initalizer
		//5. invoke "IRenderModuleXXX::ClearRenderList" in Renderer's "PresentToScreen"

************************************************************************/

#pragma once

#include "_BaseRenderModule.h"
#include "_RenderPassInfo.h"
#include "RenderInfrastructure.h"
#include "Renderer_ShadowMap.h"
#include "Renderer_Atmosphere.h"
#include "Renderer_GraphicObj.h"
#include "Renderer_Mesh.h"
#include "Renderer_Text.h"
#include "Renderer_PostProcessing.h"
#include "Renderer_SweepingTrail.h"

namespace Noise3D
{
	class /*_declspec(dllexport)*/ Renderer :
		private IFactory<IRenderInfrastructure>,
		public IRenderModuleForAtmosphere,
		public IRenderModuleForGraphicObject,
		public IRenderModuleForMesh,
		public IRenderModuleForText,
		public IRenderModuleForSweepingTrailFX,
		public IRenderModuleForPostProcessing
	{
	public:

		//explicitly overload 'AddToRenderQueue' 
		//prevent functions with same names are HIDDEN
		void		AddToRenderQueue(Mesh* obj);

		void		AddToRenderQueue(GraphicObject* obj);

		void		AddToRenderQueue(DynamicText* obj);

		void		AddToRenderQueue(StaticText* obj);

		void		AddToRenderQueue(SweepingTrail* obj);

		void		SetActiveAtmosphere(Atmosphere* obj);

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

	private:

		//extern init by SceneManager
		bool	NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_Init(UINT bufferWidth, UINT bufferHeight, HWND renderWindowHandle);

	private:

		friend SceneManager;//for external init

		friend IFactory<Renderer>;

		Renderer();

		~Renderer();

		IRenderInfrastructure* m_pRenderInfrastructure;

	};
}