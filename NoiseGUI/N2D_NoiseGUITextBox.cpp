
/***********************************************************************

								Àà£ºNOISE GUI TextBox

						Description : editable text box

************************************************************************/

#include "NoiseGUISystem.h"

NoiseGUITextBox::NoiseGUITextBox()
{
	mIsTextEditable = TRUE;
	mIsFocused = FALSE;
	m_pTextDynamic = new Noise2DTextDynamic;
	m_pGraphicObj = new NoiseGraphicObject;
	m_pTimer = new NoiseUtTimer(NOISE_TIMER_TIMEUNIT_MILLISECOND);
	mTextureID_BackGround = NOISE_MACRO_INVALID_TEXTURE_ID;
	mTextureID_Cursor = NOISE_MACRO_INVALID_TEXTURE_ID;
	mCursorPos = 0;
	mIsHoldingOnOneKey = FALSE;
	mKeyPressedDownLastFrame = NOISE_MACRO_INVALID_ID;
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
	//insert char - put a char at CursorPos (so clamp to .size() enable insertion at the back)
	//delete char- delete char at CursorPos (0 to size()-1 )
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

void NoiseGUITextBox::SetTextColor(NVECTOR4 color)
{
	m_pTextDynamic->SetTextColor(color);
}

NVECTOR4 NoiseGUITextBox::GetTextColor()
{
	return m_pTextDynamic->GetTextColor();
}

void NoiseGUITextBox::SetFont(UINT fontID)
{
	m_pTextDynamic->SetFont(fontID);
};

UINT NoiseGUITextBox::GetFontID()
{
	return m_pTextDynamic->GetFontID();
};

/************************************************************************
									P R I V A T E
************************************************************************/

void NoiseGUITextBox::Destroy()
{
	m_pTextDynamic->SelfDestruction();
	m_pGraphicObj->SelfDestruction();
}

void NoiseGUITextBox::mFunction_Update(BOOL isMouseInContainer, N_GUI_MOUSE_INFO& mouseInfo, BOOL isShiftPressed, BOOL isCapsLocked, std::vector<BOOL>& scanCodeStatusList)
{
	//validate 
	if (mIsEnabled == FALSE)return;
	
	m_pTimer->NextTick();

	//to store events message
	std::vector<UINT> tmpEventList;

	//it can judge whether to trigger TEXT_CHANGE events
	BOOL isStringModified = FALSE;

	//-----------------------Generate Common Events/Validate Focus--------------------
	mFunction_GenerateMouseEventsAndValidateFocus(tmpEventList, isMouseInContainer, mouseInfo);


	//----------------------------process keyboard input-------------------------
	//retrieve text content of the textbox
	std::string tmpTextBoxString;
	GetTextAscii(tmpTextBoxString);

	if (mIsFocused)
	{
		BOOL isNoKeyPressed = TRUE;

		//traverse every scan code 
		for (UINT currScanCode = 0;currScanCode < scanCodeStatusList.size();++currScanCode)
		{
			//encounter the first pressed key in List
			if (scanCodeStatusList.at(currScanCode) == TRUE)
			{
				isNoKeyPressed = FALSE;

				//first determine if this key can be input(prevent over-repeating)
				BOOL canSkipInput = mFunction_WhetherToSkipKeyInputProcess(currScanCode);
				if (canSkipInput)goto Label_FinishedCheckingKeyBoard;


				UINT scanCodeMapResult = 0;

				//cursor movement
				if (currScanCode == NOISE_KEY_LEFT)NoiseGUITextBox::SetCursorPos(mCursorPos - 1);
				if (currScanCode == NOISE_KEY_RIGHT)NoiseGUITextBox::SetCursorPos(mCursorPos + 1);

				//the following process the key that can edit the text content
				if (mIsTextEditable == FALSE)goto Label_FinishedCheckingKeyBoard;

				//keys of deletion
				if (currScanCode == NOISE_KEY_BACK)
				{
						isStringModified = TRUE;
						mFunction_DeleteChar(tmpTextBoxString, mCursorPos);
						goto Label_FinishedCheckingKeyBoard;//deal with one key once
				}


				if (currScanCode == NOISE_KEY_DELETE)
				{
						isStringModified = TRUE;
						mFunction_DeleteChar(tmpTextBoxString, mCursorPos + 1);
						goto Label_FinishedCheckingKeyBoard;//deal with one key once
				}

				//SHIFT will change the attribute of CASE (´óÐ¡Ð´)
				BOOL isCapital = abs(isCapsLocked - (isShiftPressed ? 1 : 0));

				//map scan code to ASCII char
				scanCodeMapResult = gFunction_MapDInputScanCodeToAscii(currScanCode, isCapital);

				//isprint() judge if the char has visible form (including space)
				if (isprint(scanCodeMapResult))
				{
					isStringModified = TRUE;
					mFunction_InsertChar(tmpTextBoxString, mCursorPos, scanCodeMapResult);
					goto Label_FinishedCheckingKeyBoard;//deal with one key once
				};


			}//isKeyPressed
		}//for every scan code

		//the repeating key bounces up,then last frame no key is pressed(related to repeating same key)
		if (isNoKeyPressed)mKeyPressedDownLastFrame = NOISE_MACRO_INVALID_ID;

	}//isFocused

	//-----------------------------------------------------------------------------
Label_FinishedCheckingKeyBoard:

	//update string
	if (isStringModified)
	{
		tmpEventList.push_back(NOISE_GUI_EVENTS_TEXTBOX_TEXTCHANGE);
	}



	//update string
	SetTextAscii(tmpTextBoxString);

	//update / clamp cursor pos according to new string
	//(chars might be deleted and cursor ran out of boundary)
	NoiseGUITextBox::SetCursorPos(mCursorPos);
	
	//align internal Text to match the size of TextBox
	m_pTextDynamic->SetCenterPos(GetCenterPos());
	m_pTextDynamic->SetWidth(GetWidth()-10.0f);//leave a little space near boundary
	m_pTextDynamic->SetHeight(GetHeight()-2.0f);
	m_pTextDynamic->SetEnabled(IsEnabled());

	//UPDATE EVENT LIST
	mFunction_CallMsgProcFunctionAndUpdateEventList(tmpEventList);
}

void  NoiseGUITextBox::mFunction_UpdateGraphicObject()
{
	const UINT rectID_textBoxBackground = 0;
	const UINT rectID_textBoxCursor = 1;

	//background
	m_pGraphicObj->SetRectangle(
		rectID_textBoxBackground,
		GetCenterPos(),
		GetWidth(),
		GetHeight(),
		GetBasicColor(),
		mTextureID_BackGround
		);

	// cursor picture(pos has been clamped)
	NVECTOR2 cursorTopLeft = GetTopLeft() + m_pTextDynamic->GetWordLocalPosOffset(mCursorPos);
	float cursorHeight = m_pTextDynamic->GetFontSize(GetFontID()).y;


	//must be focused to show the cursor
	if (mIsFocused == FALSE)
		m_pGraphicObj->SetRectangle(rectID_textBoxCursor, NVECTOR2(0, 0), NVECTOR2(0, 0), GetBasicColor(), mTextureID_Cursor);

	if (mIsFocused==TRUE &&  cursorTopLeft.y + cursorHeight < GetBottomRight().y)
	{
		m_pGraphicObj->SetRectangle(
			rectID_textBoxCursor,
			cursorTopLeft,
			cursorTopLeft + NVECTOR2(2.0f, m_pTextDynamic->GetFontSize(GetFontID()).y),
			GetBasicColor(),
			mTextureID_Cursor
			);
	}

	//---------------SET RECTANGLE DEPTH---------------
	//set the overlapping relation 
	//depth: CURSOR<TEXT<BACKGROUND
	m_pGraphicObj->SetRectangleDepth(rectID_textBoxCursor, mPosZ );
	m_pTextDynamic->SetPosZ(mPosZ + NOISE_GUI_CONST_LEVEL_DEPTH_INTERVAL / 5.0f);
	m_pGraphicObj->SetRectangleDepth(rectID_textBoxBackground, mPosZ + NOISE_GUI_CONST_LEVEL_DEPTH_INTERVAL / 3.0f);


	//internal TEXT will be updated before rendering
}

void NoiseGUITextBox::mFunction_ValidateFocus(BOOL isMouseLeftPressed, BOOL isMouseInContainer, std::vector<UINT>& eventList)
{
	//A component must be focused to interact

	if (isMouseLeftPressed)
	{
		//button down && cursor inside
		if (isMouseInContainer)
		{
			eventList.push_back(NOISE_GUI_EVENTS_COMMON_MOUSEDOWN);
			//get focus again
			if (mIsFocused == FALSE)eventList.push_back(NOISE_GUI_EVENTS_COMMON_GETFOCUS);

			//focus acquire
			mIsFocused = TRUE;
		}
		else
		{
			//lost focus (click outside)
			if (mIsFocused == TRUE)eventList.push_back(NOISE_GUI_EVENTS_COMMON_LOSTFOCUS);

			//pressed outside, lost focus
			mIsFocused = FALSE;
		}
	}//button down

}

void NoiseGUITextBox::mFunction_DeleteChar(std::string& targetString, UINT pos)
{
	//delete char- delete char at CursorPos (0 to size()-1 )
	//cursor Pos has been clamped to 0 to string.size()

	//0 ~ (string.size()-1)
	if (0<pos && pos <= targetString.size())
	{
		auto tmpIter = targetString.begin();
		targetString.erase((tmpIter += (pos-1)));//delete the char right before the cursor
		mCursorPos = pos-1;
	}

};

void NoiseGUITextBox::mFunction_InsertChar(std::string& targetString, UINT pos,char c)
{
	//insert char - put a char at CursorPos (so clamp to .size() enable insertion at the back)
	//cursor Pos has been clamped to 0 to string.size()

	if (0 <= pos && pos <= targetString.size())
	{
		auto tmpIter = targetString.begin();
		tmpIter += (pos);		//insert an element at the cursor
		targetString.insert(tmpIter, c);
		mCursorPos = pos+1;
	}
};

BOOL NoiseGUITextBox::mFunction_WhetherToSkipKeyInputProcess(UINT scanCode)
{
	//sometime we try to repeat inputing a char by holding on the keyboard.
	//but we should give enough threshold time for the slow typist not to repeat a key
	//which they actually don't want.(a finger might hold on to a key for about 50~300ms,
	//but maybe we just want 1 key input for this hit).

	//if you want you repeat a key by holding on, 700ms need to wait to confirm you actually want to repeat
	//rather than stay for long time by accident
	const double timeThreshold_TryingToRepeatOneKey = 700.0;
	const double timeThreshold_RepeatingOneKey = 40.0;

	double timeThreshold = 1000.0;

	if (mKeyPressedDownLastFrame == scanCode)
	{
		if (mIsHoldingOnOneKey)
			timeThreshold = timeThreshold_RepeatingOneKey;
		else
			timeThreshold = timeThreshold_TryingToRepeatOneKey;


		//prevent char entering from being too fast
		if (m_pTimer->GetTotalTime() <= timeThreshold)
		{
			return TRUE;//skip this key
		}
		else
		{
			mIsHoldingOnOneKey = TRUE;
			mKeyPressedDownLastFrame = scanCode;
			m_pTimer->ResetTotalTime();
			return FALSE;
		}
	}
	else
	{
		//different key is pressed
		mIsHoldingOnOneKey = FALSE;
		mKeyPressedDownLastFrame = scanCode;
		m_pTimer->ResetTotalTime();
		return FALSE;
	}

	return TRUE;
}
