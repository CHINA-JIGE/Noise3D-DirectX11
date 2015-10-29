
/***********************************************************************

									h£ºNoise GUI Text

************************************************************************/

#pragma once


public class _declspec(dllexport) NoiseGUIText :public NoiseGUIBasicContainerInfo
{
public:
	friend class NoiseGUIManager;

	NoiseGUIText();



private:
	NoiseGUIManager*		m_pFatherGUIMgr;
	
	BOOL			mIsTextEditable;
	UINT				mTextureID;

};