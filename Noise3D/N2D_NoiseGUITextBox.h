
/***********************************************************************

									h£ºNoise GUI Text

************************************************************************/

#pragma once


class _declspec(dllexport) NoiseGUITextBox :public Noise2DBasicContainerInfo,public NoiseClassLifeCycle
{
public:
	friend class NoiseGUIManager;
	friend class NoiseRenderer;

	NoiseGUITextBox();

	void		SetEnabled(BOOL isEnabled);

	BOOL	IsEnabled();

private:

	void	Destroy();

	BOOL			mIsEnabled;
	NoiseGUIManager*		m_pFatherGUIMgr;
	NoiseGraphicObject*		m_pGraphicObj;
	BOOL			mIsTextEditable;
	UINT				mTextureID;

};