
/***********************************************************************

									h£ºNoise GUI Text

************************************************************************/

#pragma once

class _declspec(dllexport) NoiseGUITextBox :
	public NoiseClassLifeCycle,
	public NoiseGUIComponentBase
{
public:
	friend class NoiseGUIManager;
	friend class NoiseRenderer;

	NoiseGUITextBox();

	void		SetTexture_BackGround(UINT texID);

	void		SetTexture_Cursor(UINT texID);

	void		SetEditable(BOOL isEditable);

	BOOL	GetEditable();

	//insert char - put a char at CursorPos (so clamp to .size() enable insertion at the back)
	//delete char- delete char at CursorPos (0 to size()-1 )
	void		SetCursorPos(UINT pos);

	UINT		GetCursorPos();
		
	void		SetMaxWordCount(UINT count= NOISE_GUI_CONST_TEXTBOX_DEFAULT_MAX_WORD_COUNT);

	UINT		GetMaxWordCount();

	void		SetTextAscii(std::string text);

	void		GetTextAscii(std::string& outText);

	void		SetTextColor(NVECTOR4 color);

	NVECTOR4 GetTextColor();

	void		SetFont(UINT fontID);

	UINT		GetFontID();


private:

	void	Destroy();
	
	BOOL mFunction_WhetherToSkipKeyInputProcess(UINT scanCode);

	void	mFunction_ValidateFocus(BOOL isMouseLeftPressed, BOOL isMouseInContainer, std::vector<UINT>& eventList);

	void	NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_Update(BOOL isMouseInContainer, N_GUI_MOUSE_INFO& mouseInfo,BOOL isShiftPressed,BOOL isCapsLocked,std::vector<BOOL>& scanCodeStatusList);//by mgr

	void NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_UpdateGraphicObject();//by mgr

	void	mFunction_DeleteChar(std::string& targetString,UINT pos);

	void	mFunction_InsertChar(std::string& targetString, UINT pos, char c);

	NoiseGUIManager*		m_pFatherGUIMgr;
	Noise2DTextDynamic*	m_pTextDynamic;
	NoiseGraphicObject*		m_pGraphicObj;//cursor & background
	NoiseUtTimer*				m_pTimer;//prevent char entering from being too fast

	BOOL			mIsTextEditable;
	UINT				mCursorPos;
	UINT				mTextureID_BackGround;
	UINT				mTextureID_Cursor;
	UINT				mKeyPressedDownLastFrame;
	UINT				mIsHoldingOnOneKey;
	UINT				mMaxWordCount;//the max count  of character which can be set 
};