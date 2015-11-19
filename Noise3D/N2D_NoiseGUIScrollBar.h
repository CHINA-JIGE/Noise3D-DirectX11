
/***********************************************************************

						h£ºNoise GUI Scroll Bar

************************************************************************/

#pragma once


public class _declspec(dllexport) NoiseGUIScrollBar :public Noise2DBasicContainerInfo
{
public:
	friend class NoiseGUIManager;
	friend class NoiseRenderer;

	NoiseGUIScrollBar();



private:
	NoiseGUIManager*		m_pFatherGUIMgr;
	NoiseGraphicObject*		m_pGraphicObj;

	NoiseGUIButton*			m_pScrollButton;
	UINT								mTextureID_Groove;
	BOOL			mIsMouseWheelScrollingEnabled;

};
