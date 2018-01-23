
/***********************************************************************

                           h£ºIRenderer
				desc: transfer data to Graphic memory
				and use gpu to render

************************************************************************/

#pragma once

#include "RenderInfrastructure.h"
#include "Renderer_Atmosphere.h"
#include "Renderer_GraphicObj.h"
#include "Renderer_Mesh.h"
#include "Renderer_Text.h"

namespace Noise3D
{
	/*class IRenderModule
	{
	public:

		void addToList();
		void Render();

	protected:

		void clear();
	private:
	};*/

	class /*_declspec(dllexport)*/ IRenderer :
		private IFactory<IRenderInfrastructure>,
		public IRenderModuleForAtmosphere
	{
	public:
		
		/*void			RenderMeshes();

		void			RenderGraphicObjects();

		//void			RenderAtmosphere();

		void			RenderTexts();

		void			AddObjectToRenderList(IMesh* obj);

		void			AddObjectToRenderList(IGraphicObject* obj);

		void			AddObjectToRenderList(IAtmosphere* obj);

		void			AddObjectToRenderList(IDynamicText* obj);

		void			AddObjectToRenderList(IStaticText* obj);*/

		void			ClearBackground(const NVECTOR4& color = NVECTOR4(0, 0, 0, 0.0f));

		void			PresentToScreen();

		UINT		GetBackBufferWidth();

		UINT		GetBackBufferHeight();

		void			SetDepthTestEnabled(bool isEnabled);

		void			SetPostProcessingEnabled(bool isEnabled);

	private:

		//extern init by IScene
		bool	NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_Init(UINT bufferWidth, UINT bufferHeight, bool IsWindowed);

		void	mFunction_AddToRenderList_GraphicObj(IGraphicObject* pGraphicObj, std::vector<IGraphicObject*>* pList);

		void	mFunction_AddToRenderList_Text(IBasicTextInfo* pText, std::vector<IBasicTextInfo*>* pList);


		//----------------MESHES-----------------------
		void			mFunction_RenderMeshInList_UpdatePerObject(IMesh* const pMesh);

		void			mFunction_RenderMeshInList_UpdatePerFrame(ICamera*const pCamera);

		ID3DX11EffectPass*		mFunction_RenderMeshInList_UpdatePerSubset(IMesh* const pMesh, UINT subsetID);//return subset primitive count

		void			mFunction_RenderMeshInList_UpdateRarely();


		//----------------GRAPHIC OBJECT-----------------------
		void			mFunction_GraphicObj_Update_RenderTextured2D(N_UID texName);

		void			mFunction_GraphicObj_RenderLine3DInList(ICamera*const pCamera, std::vector<IGraphicObject*>* pList);

		void			mFunction_GraphicObj_RenderPoint3DInList(ICamera*const pCamera, std::vector<IGraphicObject*>* pList);

		void			mFunction_GraphicObj_RenderLine2DInList(std::vector<IGraphicObject*>* pList);

		void			mFunction_GraphicObj_RenderPoint2DInList(std::vector<IGraphicObject*>* pList);

		void			mFunction_GraphicObj_RenderTriangle2DInList(std::vector<IGraphicObject*>* pList);

		//----------------TEXT-----------------------
		void			mFunction_TextGraphicObj_Update_TextInfo(N_UID uid, ITextureManager* pTexMgr, IBasicTextInfo* pText);

		void			mFunction_TextGraphicObj_Render(std::vector<IBasicTextInfo*>* pList);

	private:

		friend IScene;//for external init

		friend IFactory<IRenderer>;

		IRenderer();

		~IRenderer();

		IRenderInfrastructure* m_pRenderInfrastructure;

		/*IShaderVariableManager* m_pRefShaderVarMgr;

		//each category of renderable object should have:
		//1.object list to be rendered
		//2.specific render preparation and draw call
		//3.clear render list after Present()
		std::vector <IMesh*>					mRenderList_Mesh;
		std::vector <IGraphicObject*> 	mRenderList_CommonGraphicObj;//for user-defined graphic obj rendering
		std::vector <IBasicTextInfo*>		mRenderList_TextDynamic;//for dynamic Text Rendering(including other info)
		std::vector <IBasicTextInfo*>		mRenderList_TextStatic;//for static Text Rendering(including other info)
		std::vector <IAtmosphere*>		mRenderList_Atmosphere;*/

		/*IDXGISwapChain*						m_pSwapChain;
		ID3D11RenderTargetView*			m_pRenderTargetViewForPresent;//RTV for back buffer
		ID3D11DepthStencilView*			m_pDepthStencilView;//DSV for back buffer*/
		/*ID3D11RenderTargetView*			m_pRTTRenderTargetView;//RTV for render-to-texture
		ID3D11ShaderResourceView*		m_pRTTShaderResourceView;//SRV for render-To-Texture
		ID3D11DepthStencilView*			m_pRTTDepthStencilView;//DSV for render-To-Texture
		ID3D11RasterizerState*				m_pRasterState_Solid_CullNone;
		ID3D11RasterizerState*				m_pRasterState_Solid_CullBack;
		ID3D11RasterizerState*				m_pRasterState_Solid_CullFront;
		ID3D11RasterizerState*				m_pRasterState_WireFrame_CullFront;
		ID3D11RasterizerState*				m_pRasterState_WireFrame_CullNone;
		ID3D11RasterizerState*				m_pRasterState_WireFrame_CullBack;
		ID3D11BlendState*					m_pBlendState_Opaque;
		ID3D11BlendState*					m_pBlendState_AlphaTransparency;
		ID3D11BlendState*					m_pBlendState_ColorAdd;
		ID3D11BlendState*					m_pBlendState_ColorMultiply;
		ID3D11DepthStencilState*		m_pDepthStencilState_EnableDepthTest;
		ID3D11DepthStencilState*		m_pDepthStencilState_DisableDepthTest;
		ID3D11SamplerState*				m_pSamplerState_FilterLinear;*/

		//EffectTechnique interfaces in compiled internal shaders
		/*ID3DX11EffectTechnique*			m_pFX_Tech_DrawMesh;
		ID3DX11EffectTechnique*			m_pFX_Tech_Solid3D;
		ID3DX11EffectTechnique*			m_pFX_Tech_Solid2D;
		ID3DX11EffectTechnique*			m_pFX_Tech_Textured2D;
		ID3DX11EffectTechnique*			m_pFX_Tech_DrawText2D;
		ID3DX11EffectTechnique*			m_pFX_Tech_DrawSky;*/

	};
}
