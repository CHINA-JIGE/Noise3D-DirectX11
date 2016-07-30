
/***********************************************************************

						h£ºNoise GUI Scroll Bar

************************************************************************/

#pragma once

namespace Noise3D
{
	namespace GUI
	{
		class /*_declspec(dllexport)*/ IScrollBar :
			public IComponentBase
		{
		public:
			friend class IComponentManager;
			friend class IRenderer;

			IScrollBar(BOOL isHorizontal = FALSE, float maxValue = 1.0f, float minValue = 0.0f);

			void		SetAlignment(BOOL isHorizontal);

			void		SetTexture_ScrollGroove(UINT texID);

			void		SetTexture_ScrollButton(NOISE_GUI_BUTTON_STATE btnState, UINT texID);

			void		SetValue(float val);

			float		GetValue();

			void		SetValueRange(float minValue, float maxValue);//minimum range 0.001f

			void		SetScrollButtonLength(float btnPixelLength);

			void		SetScrollWheelSpeed(float fSpeed = 1.0f);//default :1.0f

			void		SetEnabled(BOOL isEnabled);//override (internal btn also need to set)

		private:

			void	Destroy();

			void NOISE_MACRO_FUNCTION_EXTERN_CALL	mFunction_Update(BOOL isMouseInContainer, N_GUI_MOUSE_INFO& mouseInfo);

			void NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_UpdateGraphicObject();

			IComponentManager*		m_pFatherGUIMgr;
			IGraphicObject*		m_pGraphicObj_Groove;//used to store the Groove picture for the time being
			UINT								mTextureID_Groove;
			IButton*			m_pButtonScrolling;
			float								mScrollButtonLength;
			float								mScrollWheelSpeed;
			BOOL							mIsMouseWheelScrollingEnabled;
			BOOL							mIsHorizontal;

			float								mCurrentValue;
			float								mRangeMin;
			float								mRangeMax;
		};
	}
}