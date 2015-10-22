
/***********************************************************************

							¿‡£∫NOISE GUI BUTTON

				Description : command button

************************************************************************/

#include "Noise3D.h"

NoiseGUIButton::NoiseGUIButton(UINT(*pFunc)(UINT NoiseGUIEvent))
{
	*m_pBasicColor = NVECTOR4(0.3f,0.3f,1.0f,1.0f);
	mTextureID_MouseAway				= NOISE_MACRO_INVALID_TEXTURE_ID;
	mTextureID_MouseOn					= NOISE_MACRO_INVALID_TEXTURE_ID;
	mTextureID_MousePressedDown	= NOISE_MACRO_INVALID_TEXTURE_ID;
	mGraphicObject_RectID				= NOISE_MACRO_INVALID_GRAPHICOBJ_ID;
	mButtonState = NOISE_GUI_BUTTON_STATE_COMMON;
	mButtonHasBeenPressedDown = FALSE;
	m_pFunction_EventMessageProcess = pFunc;//default:nullptr
}

void NoiseGUIButton::SetEventProcessCallbackFunction(UINT(*pFunc)(UINT NoiseGUIEvent))
{
	if(pFunc)m_pFunction_EventMessageProcess = pFunc;
}


/************************************************************************
                                     P R I V A T E          
************************************************************************/




