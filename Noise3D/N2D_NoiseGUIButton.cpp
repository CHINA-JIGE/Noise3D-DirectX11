
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
	mIsDragableX = FALSE;
	mIsDragableY = FALSE;
	m_pFunction_EventMessageProcess = pFunc;//default:nullptr
}

void NoiseGUIButton::SetEventProcessCallbackFunction(UINT(*pFunc)(UINT NoiseGUIEvent))
{
	if(pFunc)m_pFunction_EventMessageProcess = pFunc;
}

void NoiseGUIButton::SetDragableX(BOOL dragableX)
{
	mIsDragableX = dragableX;
}

void NoiseGUIButton::SetDragableY(BOOL dragableY)
{
	mIsDragableY = dragableY;
}

void NoiseGUIButton::SetTexture_MouseAway(UINT texID)
{
	//texID will be validated when rendering graphic object
	mTextureID_MouseAway = texID;
};

void NoiseGUIButton::SetTexture_MouseOn(UINT texID)
{
	mTextureID_MouseOn = texID;
};

void NoiseGUIButton::SetTexture_MousePressedDown(UINT texID)
{
	mTextureID_MousePressedDown = texID;
};

/************************************************************************
                                     P R I V A T E          
************************************************************************/




