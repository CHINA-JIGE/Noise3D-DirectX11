
/***********************************************************************

									h£ºNoise GUI Text

************************************************************************/

#pragma once


public class _declspec(dllexport) NoiseGUIText :public Noise2DBasicContainerInfo
{
public:
	friend class NoiseGUIManager;
	friend class NoiseRenderer;

	NoiseGUIText();


private:
	NoiseGUIManager*		m_pFatherGUIMgr;
	NoiseGraphicObject*		m_pGraphicObj;
	BOOL			mIsTextEditable;
	UINT				mTextureID;

};