
/***********************************************************************

									h£ºNoise GUI Label

************************************************************************/

#pragma once


//A button ; can be embbed into other GUI object
public class _declspec(dllexport) NoiseGUIPicture :public NoiseGUIBasicContainerInfo
{
public:
	friend class NoiseGUIManager;

	NoiseGUIPicture();



private:
	NoiseGUIManager*		m_pFatherGUIMgr;
	
	UINT				mTextureID;

};