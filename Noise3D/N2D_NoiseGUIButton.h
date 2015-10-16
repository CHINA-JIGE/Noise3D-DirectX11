
/***********************************************************************

								h:Noise GUI Button

************************************************************************/

#pragma once

//A button ; can be embbed into other GUI object
public class _declspec(dllexport) NoiseGUIButton :public NoiseGUIBasicContainerInfo
{
public:
	friend class NoiseGUIManager;

	NoiseGUIButton();

	void	SetTexture_MouseAway(UINT texID);

	void	SetTexture_MouseOn(UINT texID);

	void	SetTexture_MousePressedDown(UINT texID);


private:
	NoiseGUIManager*		m_pFatherGUIMgr;

	NOISE_GUI_BUTTON_STATE mButtonState;
	NVECTOR3*	m_pBasicColor;
	UINT				mTextureID_MouseAway;
	UINT				mTextureID_MouseOn;
	UINT				mTextureID_MousePressedDown;


};