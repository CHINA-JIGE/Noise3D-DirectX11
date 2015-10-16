
/***********************************************************************

							¿‡£∫NOISE GUI BUTTON

				Description : command buttom 

************************************************************************/

#include "Noise3D.h"


NoiseGUIButton::NoiseGUIButton()
{
	*m_pBasicColor = NVECTOR3(0.3f,0.3f,1.0f);
	mTextureID_MouseAway				= NOISE_MACRO_INVALID_TEXTURE_ID;
	mTextureID_MouseOn					= NOISE_MACRO_INVALID_TEXTURE_ID;
	mTextureID_MousePressedDown	= NOISE_MACRO_INVALID_TEXTURE_ID;
	mButtonState = NOISE_GUI_BUTTON_STATE_COMMON;
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




