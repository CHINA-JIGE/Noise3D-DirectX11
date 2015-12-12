
/***********************************************************************

									h£ºNoise GUI Text

************************************************************************/

#pragma once

class _declspec(dllexport) NoiseGUITextBox :
	public Noise2DBasicContainerInfo,
	public NoiseClassLifeCycle,
	public NoiseGUIEventCommonOperation
{
public:
	friend class NoiseGUIManager;
	friend class NoiseRenderer;

	NoiseGUITextBox();

	void		SetTexture_BackGround(UINT texID);

	void		SetTexture_Cursor(UINT texID);

	void		SetEditable(BOOL isEditable);

	BOOL	GetEditable();

	void		SetCursorPos(UINT pos);

	UINT		GetCursorPos();
		
	void		SetTextAscii(std::string text);

	void		GetTextAscii(std::string& outText);

	void		SetFont(UINT fontID);

	UINT		GetFontID();

private:

	void	Destroy();

	NoiseGUIManager*		m_pFatherGUIMgr;
	Noise2DTextDynamic*	m_pTextDynamic;
	NoiseGraphicObject*		m_pGraphicObj;//cursor & background
	BOOL			mIsFocused;
	BOOL			mIsTextEditable;
	UINT				mCursorPos;
	UINT				mTextureID_BackGround;
	UINT				mTextureID_Cursor;

};