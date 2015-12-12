
/***********************************************************************

								Àà£ºNOISE GUI Label

						Description : ineditable text box

************************************************************************/

#include "Noise3D.h"

NoiseGUITextBox::NoiseGUITextBox()
{
	mIsTextEditable = TRUE;
	mIsFocused = FALSE;
	m_pTextDynamic = new Noise2DTextDynamic;
	m_pGraphicObj = new NoiseGraphicObject;
	mTextureID_BackGround = NOISE_MACRO_INVALID_TEXTURE_ID;
	mTextureID_Cursor = NOISE_MACRO_INVALID_TEXTURE_ID;
}
void NoiseGUITextBox::SetTexture_BackGround(UINT texID)
{
	mTextureID_BackGround = texID;
};

void NoiseGUITextBox::SetTexture_Cursor(UINT texID)
{
	mTextureID_Cursor = texID;
}

void NoiseGUITextBox::SetEditable(BOOL isEditable)
{
	mIsTextEditable = isEditable;
};

BOOL NoiseGUITextBox::GetEditable()
{
	return mIsTextEditable;
}
void NoiseGUITextBox::SetCursorPos(UINT pos)
{
	std::string tmpText;
	m_pTextDynamic->GetTextAscii(tmpText);
	mCursorPos = gFunction_Clamp(pos, 0, tmpText.size());
};

UINT NoiseGUITextBox::GetCursorPos()
{
	return mCursorPos;
}
void NoiseGUITextBox::SetTextAscii(std::string text)
{
	m_pTextDynamic->SetTextAscii(text);
};

void NoiseGUITextBox::GetTextAscii(std::string & outText)
{
	m_pTextDynamic->GetTextAscii(outText);
}

void NoiseGUITextBox::SetFont(UINT fontID)
{
	m_pTextDynamic->SetFont(fontID);
};

UINT NoiseGUITextBox::GetFontID()
{
	return m_pTextDynamic->GetFontID();
};





void NoiseGUITextBox::Destroy()
{
	m_pTextDynamic->SelfDestruction();
	m_pGraphicObj->SelfDestruction();
}
