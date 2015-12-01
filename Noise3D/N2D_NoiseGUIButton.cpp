
/***********************************************************************

							¿‡£∫NOISE GUI BUTTON

				Description : command button

************************************************************************/

#include "Noise3D.h"

NoiseGUIButton::NoiseGUIButton(UINT(*pFunc)(UINT NoiseGUIEvent))
{
	mIsEnabled = TRUE;
	*m_pBasicColor = NVECTOR4(0.3f,0.3f,1.0f,1.0f);
	m_pMouseDown_OffsetFromCenter = new NVECTOR2(0, 0);
	m_pGraphicObj = new NoiseGraphicObject;
	m_pEventList = new std::vector<UINT>;
	mTextureID_MouseAway				= NOISE_MACRO_INVALID_TEXTURE_ID;
	mTextureID_MouseOn					= NOISE_MACRO_INVALID_TEXTURE_ID;
	mTextureID_MousePressedDown	= NOISE_MACRO_INVALID_TEXTURE_ID;
	mButtonState = NOISE_GUI_BUTTON_STATE_COMMON;
	mButtonHasBeenPressedDown = FALSE;
	mIsDragableX = FALSE;
	mIsDragableY = FALSE;
	m_pFunction_EventMessageProcess = pFunc;//default:nullptr
}

void NoiseGUIButton::Destroy()
{
	m_pFunction_EventMessageProcess = nullptr;
	m_pGraphicObj->SelfDestruction();
}

void NoiseGUIButton::SetDragableX(BOOL dragableX)
{
	mIsDragableX = dragableX;
}

void NoiseGUIButton::SetDragableY(BOOL dragableY)
{
	mIsDragableY = dragableY;
}

void NoiseGUIButton::SetTexture(NOISE_GUI_BUTTON_STATE btnState, UINT texID)
{
	switch (btnState)
	{
	case NOISE_GUI_BUTTON_STATE_COMMON:
		mTextureID_MouseAway = texID;
		break;
	case NOISE_GUI_BUTTON_STATE_MOUSEON:
		mTextureID_MouseOn = texID;
		break;
	case NOISE_GUI_BUTTON_STATE_MOUSEBUTTONDOWN:
		mTextureID_MousePressedDown = texID;
		break;
	default:
		break;
	}
}

void NoiseGUIButton::SetEnabled(BOOL isEnabled)
{
	mIsEnabled = isEnabled;
}

BOOL NoiseGUIButton::IsEnabled()
{
	return mIsEnabled;
}


/************************************************************************
                                     P R I V A T E          
************************************************************************/

