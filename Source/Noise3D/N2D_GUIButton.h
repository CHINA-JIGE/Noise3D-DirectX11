
/***********************************************************************

								h:Noise GUI Button

************************************************************************/

#pragma once

namespace Noise3D
{
	namespace GUI
	{
		//A button ; can be embbed into other GUI object
		class _declspec(dllexport) IButton :
			public	IComponentBase
		{
		public:
			friend class IComponentManager;
			friend class IRenderer;

			IButton(UINT(*pFunc)(UINT NoiseGUIEvent) = nullptr);


			void	SetDragableX(BOOL dragableX);

			void	SetDragableY(BOOL dragableY);

			void	SetTexture(NOISE_GUI_BUTTON_STATE btnState, UINT texID);

		private:

			void	Destroy();

			void NOISE_MACRO_FUNCTION_EXTERN_CALL	mFunction_Update(BOOL isMouseInContainer, N_GUI_MOUSE_INFO& mouseInfo);

			void NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_UpdateGraphicObject();

			IComponentManager*		m_pFatherGUIMgr;
			IGraphicObject*		m_pGraphicObj;

			NOISE_GUI_BUTTON_STATE mButtonState;
			NVECTOR2* m_pMouseDown_OffsetFromCenter;//used when dragable , to keep the relative position unchanged to cursor

			//textures for 3 different state
			UINT		mTextureID_MouseAway;
			UINT		mTextureID_MouseOn;
			UINT		mTextureID_MousePressedDown;

			BOOL	mIsDragableX;//decide if it can be dragged to move in X dir
			BOOL	mIsDragableY;//decide if it can be dragged to move in Y dir

		};
	}
}