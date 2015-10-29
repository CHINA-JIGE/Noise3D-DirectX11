
/***********************************************************************

						h£ºNoise GUI Scroll Bar

************************************************************************/

#pragma once


public class _declspec(dllexport) NoiseGUIScrollBar :public NoiseGUIBasicContainerInfo
{
public:
	friend class NoiseGUIManager;

	NoiseGUIScrollBar();



private:
	NoiseGUIManager*		m_pFatherGUIMgr;

	NoiseGUIButton*			m_pScrollButton;
	UINT								mTextureID_Groove;
	BOOL			mIsMouseWheelScrollingEnabled;

};
