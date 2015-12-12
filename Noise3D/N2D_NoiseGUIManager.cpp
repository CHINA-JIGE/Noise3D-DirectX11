
/***********************************************************************

								¿‡£∫NOISE GUI manager

				Description : manage different GUI objects

************************************************************************/

#include "Noise3D.h"

static UINT	static_mouseScrPosX=0;
static UINT	static_mouseScrPosY = 0;
static float	static_windowSizescaleX = 1.0f;
static float	static_windowSizeScaleY = 1.0f;
static float	static_mouseX_BackBufferSpace = 0.0;
static float	static_mouseY_BackBufferSpace = 0.0f;

NoiseGUIManager::NoiseGUIManager()
{
	m_pInputEngine = nullptr;
	m_pFontMgr = nullptr;
	m_pChildButtonList		= new std::vector<NoiseGUIButton*>;
	m_pChildTextList			= new std::vector<NoiseGUITextBox*>;
	m_pChildScrollBarList	= new std::vector<NoiseGUIScrollBar*>;
	m_pTimer						= new NoiseUtTimer(NOISE_TIMER_TIMEUNIT_MILLISECOND);
}


BOOL NoiseGUIManager::Initialize(NoiseUtInputEngine& refInputE, NoiseFontManager& refFontMgr, HWND hwnd)
{
	if (IsInitialized())
	{
		DEBUG_MSG1("Object Has Been Initialized!");
		return FALSE;
	}

	m_pInputEngine = &refInputE;
	m_pFontMgr = &refFontMgr;

	//hwnd is needed for some win API invokation
	if (hwnd)
	{
		mWindowHWND = hwnd;
	}
	else
	{
		DEBUG_MSG1("Create GUI : hwnd Invaid!!");
		return FALSE;
	}

	SetStatusToBeInitialized();
	return TRUE;
}

BOOL NoiseGUIManager::InitButton(NoiseGUIButton& refObject)
{
	if (refObject.IsInitialized())
	{
		DEBUG_MSG1("Object Has Been Initialized!");
		return FALSE;
	}

	//add button to child button list
	m_pChildButtonList->push_back(&refObject);
	refObject.m_pFatherGUIMgr = this;

	//create graphic object
	 refObject.m_pGraphicObj->AddRectangle(NVECTOR2(0, 0), NVECTOR2(100.0f, 50.0f),
		NVECTOR4(0.3f, 0.3f, 1.0f, 1.0f), NOISE_MACRO_INVALID_TEXTURE_ID);

	refObject.SetStatusToBeInitialized();

	return TRUE;
}

BOOL NoiseGUIManager::InitTextBox(NoiseGUITextBox& refObject, UINT fontID)
{
	if (refObject.IsInitialized())
	{
		DEBUG_MSG1("TextBox Has Been Initialized!");
		return FALSE;
	}

	if (m_pFontMgr == nullptr)
	{
		DEBUG_MSG1("Font Manager Hasn't been set!");
		return FALSE;
	}

	//backGround
	refObject.m_pGraphicObj->AddRectangle(NVECTOR2(0, 0), NVECTOR2(100.0f, 50.0f), NVECTOR4(0.3f, 0.3f, 1.0f, 1.0f), NOISE_MACRO_INVALID_TEXTURE_ID);
	//cursor
	refObject.m_pGraphicObj->AddRectangle(NVECTOR2(0, 0), NVECTOR2(20.0f, 3.0f), NVECTOR4(0.3f, 1.0f, 0.3f, 1.0f), NOISE_MACRO_INVALID_TEXTURE_ID);

	m_pFontMgr->InitDynamicTextA(fontID, "", 100, 50, NVECTOR4(0.3f, 1.0f, 0.3f, 1.0f), 3, 0, *refObject.m_pTextDynamic);

	//add label ( pure text)
	m_pChildTextList->push_back(&refObject);
	refObject.m_pFatherGUIMgr = this;

	refObject.SetStatusToBeInitialized();

	return TRUE;
}

BOOL NoiseGUIManager::InitScrollBar(NoiseGUIScrollBar & refObject)
{
	if (refObject.IsInitialized())
	{
		DEBUG_MSG1("Object Has Been Created!");
		return FALSE;
	}

	//initialized internal GUI button
	InitButton(*refObject.m_pButtonScrolling);
	refObject.m_pGraphicObj_Groove->AddRectangle(NVECTOR2(0,0),NVECTOR2(200.0f,30.0f),NVECTOR4(1.0f,1.0f,1.0f,1.0f));

	m_pChildScrollBarList->push_back(&refObject);
	refObject.m_pFatherGUIMgr = this;

	refObject.SetStatusToBeInitialized();

	return TRUE;
}

void	NoiseGUIManager::SetInputEngine(NoiseUtInputEngine& refInputE)
{
	m_pInputEngine = &refInputE;
};

void NoiseGUIManager::SetFontManager(NoiseFontManager & refFontMgr)
{
	m_pFontMgr = &refFontMgr;
};


void	NoiseGUIManager::Update()
{
	//input Engine must be set
	if (!m_pInputEngine)return;

	//timer continue ticking
	m_pTimer->NextTick();

	//if internal input engine was inoperative, then nth will be retrieved
	if (m_pInputEngine->IsInitialized())
	{
		m_pInputEngine->Update();


		//Get info from Input Engine
#pragma region UpdateInputEngineInfo

		static_mouseScrPosX = m_pInputEngine->GetMouseScrPosX();
		static_mouseScrPosY = m_pInputEngine->GetMouseScrPosY();

		//get the Rect info of render window
		RECT windowRect;
		NOISE_MACRO_FUNCTION_WINAPI GetWindowRect(mWindowHWND, &windowRect);

		//we should check window style in case it's a window with FRAMES or CAPTION
		long windowLONG =NOISE_MACRO_FUNCTION_WINAPI GetWindowLongW(mWindowHWND, GWL_STYLE);
		UINT windowFrameWidth = NOISE_MACRO_FUNCTION_WINAPI  GetSystemMetrics(SM_CXFRAME);
		UINT windowFrameHeight = NOISE_MACRO_FUNCTION_WINAPI  GetSystemMetrics(SM_CYFRAME);
		UINT windowCaptionHeight = NOISE_MACRO_FUNCTION_WINAPI  GetSystemMetrics(SM_CYCAPTION);

		//this is a mode without window Caption
		if (windowLONG & WS_POPUP)
		{
			windowFrameWidth = 0;
			windowFrameHeight = 0;
			windowCaptionHeight = 0;
		}

		//remember , render window might enlarge or shrink , (didnt match the back buffer)
		//so mouse coord must be scaled to fit 
		//also note that actual render area don't include the window FRAMES & CAPTION
		static_windowSizescaleX = (float)(windowRect.right - windowRect.left - windowFrameWidth * 2) / gMainBufferPixelWidth;
		static_windowSizeScaleY = (float)(windowRect.bottom - windowRect.top - windowFrameHeight * 2 - windowCaptionHeight) / gMainBufferPixelHeight;

		//relative cursor Pos (window scaling is taken into account)
		static_mouseX_BackBufferSpace = (static_mouseScrPosX - windowRect.left - windowFrameWidth) / static_windowSizescaleX;
		static_mouseY_BackBufferSpace = (static_mouseScrPosY - windowRect.top - windowFrameHeight - windowCaptionHeight) / static_windowSizeScaleY;

#pragma endregion UpdateInputEngineInfo


		//traverse various kinds of objects and send message
		mFunction_UpdateButtons();

		mFunction_UpdateScrollBars();

		if (m_pFontMgr != nullptr)mFunction_UpdateTextBoxs();

		mFunction_UpdateAllInternalGraphicObjs();
	}
	else
	{
		DEBUG_MSG1("GUI: Input Engine is not initialized.");
	}

}

void NoiseGUIManager::SetWindowHWND(HWND hWnd)
{
	mWindowHWND = hWnd;
};

/***********************************************************************
										P R I V A T E
************************************************************************/
void NoiseGUIManager::Destroy()
{
	for (auto obj : *m_pChildButtonList)
	{
		if (obj)obj->SelfDestruction();
	}

	for (auto obj : *m_pChildTextList)
	{
		if (obj)obj->SelfDestruction();
	}

	for (auto obj : *m_pChildScrollBarList)
	{
		if (obj)obj->SelfDestruction();
	}
}

void NoiseGUIManager::mFunction_CallMsgProcFunctionAndUpdateEventList(std::vector<UINT>& eventList, NoiseGUIEventCommonOperation* eventOpClass)
{

	eventOpClass->m_pEventList->clear();
	for (UINT currEvent : eventList)
	{
		//the function ptr cannot be NULL
		if (eventOpClass->m_pFunction_EventMessageProcess)
		{
			//call back to main .exe using function ptr
			(eventOpClass->m_pFunction_EventMessageProcess)(currEvent);
		}

		//add to its message list
		eventOpClass->m_pEventList->push_back(currEvent);
	}
};

void NoiseGUIManager::mFunction_UpdateButtons()
{

	//!!!!!!!watch out for the multi-level of 'if'  embbeding
	for (auto pCurrButton : *m_pChildButtonList)
	{
		//to store events message
		std::vector<UINT> tmpEventList;

		if (pCurrButton->IsEnabled() == FALSE)continue;

#pragma region UpdateButtonState

		//if mouse lies in the rect of button
		if (pCurrButton->IsPointInContainer(NVECTOR2(static_mouseX_BackBufferSpace, static_mouseY_BackBufferSpace)))
		{
			//message proc callback function
			tmpEventList.push_back(NOISE_GUI_EVENTS_BUTTON_MOUSEON);

			if (m_pInputEngine->IsMouseButtonPressed(NOISE_MOUSEBUTTON_LEFT))
			{
				if (!pCurrButton->mButtonHasBeenPressedDown)
				{
					//the first time pressed down, record the relative pos of cursor to btn center
					pCurrButton->m_pMouseDown_OffsetFromCenter->x = pCurrButton->GetCenterPos().x- static_mouseX_BackBufferSpace ;
					pCurrButton->m_pMouseDown_OffsetFromCenter->y = pCurrButton->GetCenterPos().y- static_mouseY_BackBufferSpace;
				}

				//cursor is within the button and mouseLeft has been pressed down
				tmpEventList.push_back(NOISE_GUI_EVENTS_BUTTON_MOUSEDOWN);
				pCurrButton->mButtonState = NOISE_GUI_BUTTON_STATE_MOUSEBUTTONDOWN;
				pCurrButton->mButtonHasBeenPressedDown = TRUE;


				//now mouse is down , let's see if the mouse is dragging the button
				//apply relative movemwnt along with mouse
				//use position difference will lead to some aliasing (because of delay)
				//and an offset from cursor to button center should be accumulated
				if (pCurrButton->mIsDragableX)
				{
					pCurrButton->SetCenterPos(
						NVECTOR2(static_mouseX_BackBufferSpace, pCurrButton->GetCenterPos().y)
						+NVECTOR2(pCurrButton->m_pMouseDown_OffsetFromCenter->x,0));
				}
					
				if (pCurrButton->mIsDragableY)
				{
					pCurrButton->SetCenterPos(
						NVECTOR2(pCurrButton->GetCenterPos().x, static_mouseY_BackBufferSpace)
						+ NVECTOR2(0,pCurrButton->m_pMouseDown_OffsetFromCenter->y));
				}

			}//move pressed
			else
			{
				//cursor is within the button and mouseLeft is up
				pCurrButton->mButtonState = NOISE_GUI_BUTTON_STATE_MOUSEON;

				//now mouse is down , let's see if the mouse is dragging the button
				if ((m_pInputEngine->GetMouseDiffX()!=0) || (m_pInputEngine->GetMouseDiffY()!=0))
				{
					tmpEventList.push_back(NOISE_GUI_EVENTS_BUTTON_MOUSEMOVE);
				}
			}

			//mouse button up
			if (!m_pInputEngine->IsMouseButtonPressed(NOISE_MOUSEBUTTON_LEFT)
				&& pCurrButton->mButtonHasBeenPressedDown)
			{
				//a button down and up makes a "press"
				tmpEventList.push_back(NOISE_GUI_EVENTS_BUTTON_MOUSEPRESSED);
				pCurrButton->mButtonHasBeenPressedDown = FALSE;
			}
		}
		else//if cursor isn't within the button
		{
			pCurrButton->mButtonHasBeenPressedDown = FALSE;
			pCurrButton->mButtonState = NOISE_GUI_BUTTON_STATE_COMMON;
		}

		//button was pressed down inside but is released outside , state must be updated
		if (!m_pInputEngine->IsMouseButtonPressed(NOISE_MOUSEBUTTON_LEFT))
		{
			pCurrButton->mButtonHasBeenPressedDown = FALSE;
		}

#pragma endregion UpdateButtonState

	//UPDATE EVENTS LIST
	mFunction_CallMsgProcFunctionAndUpdateEventList(tmpEventList, pCurrButton);

	}//end button traverse

}

void NoiseGUIManager::mFunction_UpdateScrollBars()
{

	//mouse scrolling wheel movement
	float mouseScrollDiff = -float(m_pInputEngine->GetMouseScrollDiff())/30.0f;

	//internal button has been updated,so DRAG operation has been done
	for (auto pBar : *m_pChildScrollBarList)
	{
		//to store events message
		std::vector<UINT> tmpEventList;

		if (pBar->IsEnabled() == FALSE)continue;

		NVECTOR2 btnPos = pBar->m_pButtonScrolling->GetCenterPos();
		NVECTOR2 btnPosTopLeft = pBar->m_pButtonScrolling->GetTopLeft();
		NVECTOR2 btnPosBottomRight = pBar->m_pButtonScrolling->GetBottomRight();
		NVECTOR2 barPos = pBar->GetCenterPos();
		NVECTOR2 barPosTopLeft = pBar->GetTopLeft();
		NVECTOR2 barPosBottomRight = pBar->GetBottomRight();
		float btnWidth = pBar->m_pButtonScrolling->GetWidth();
		float btnHeight= pBar->m_pButtonScrolling->GetHeight();
		float	barWidth = pBar->GetWidth();
		float barHeight = pBar->GetHeight();

	#pragma region UpdatePositions(Bind button the groove)
		if (pBar->mIsHorizontal)
		{
			//move button :  bind scroll button to the scrolling groove
			pBar->m_pButtonScrolling->SetCenterPos(gFunction_Clampf(btnPos.x + float(mouseScrollDiff), barPosTopLeft.x+btnWidth/2.0f, barPosBottomRight.x- btnWidth / 2.0f), barPos.y);
			pBar->m_pButtonScrolling->SetWidth(pBar->mScrollButtonLength);
			pBar->m_pButtonScrolling->SetHeight(pBar->GetHeight());

			//derived by the position of scroll button
			float valueRatio =(btnPosTopLeft.x- barPosTopLeft.x)/(barWidth-btnWidth);
			
			//linear interpolation
			float newValue= gFunction_Lerp(pBar->mRangeMin, pBar->mRangeMax, valueRatio);

			//generate "SCROLL" event
			if (newValue != pBar->mCurrentValue)
			{
				tmpEventList.push_back(NOISE_GUI_EVENTS_SCROLLBAR_SCROLL);
			}
			pBar->mCurrentValue = gFunction_Clampf(newValue, pBar->mRangeMin, pBar->mRangeMax);
		}
		else
		{
			pBar->m_pButtonScrolling->SetCenterPos(barPos.x,gFunction_Clampf(btnPos.y + float(mouseScrollDiff), barPosTopLeft.y + btnHeight / 2.0f, barPosBottomRight.y - btnHeight / 2.0f));
			pBar->m_pButtonScrolling->SetWidth(pBar->GetWidth());
			pBar->m_pButtonScrolling->SetHeight(pBar->mScrollButtonLength);

			float valueRatio = (btnPosTopLeft.y  - barPosTopLeft.y) / (barHeight - btnHeight);

			//linear interpolation
			float newValue = gFunction_Lerp(pBar->mRangeMin, pBar->mRangeMax, valueRatio);

			//generate "SCROLL" event
			if (newValue != pBar->mCurrentValue)
			{
				tmpEventList.push_back(NOISE_GUI_EVENTS_SCROLLBAR_SCROLL);
			}
			pBar->mCurrentValue = gFunction_Clampf(newValue, pBar->mRangeMin, pBar->mRangeMax);

		}

	#pragma endregion UpdatePositions(Bind button the groove)


		//actively get events from child button
		std::vector<UINT> childButtonEventList;
		pBar->m_pButtonScrolling->GetEventList(childButtonEventList);
		for (auto eventID : childButtonEventList)
		{
			if (eventID == NOISE_GUI_EVENTS_BUTTON_MOUSEDOWN)
			{
				tmpEventList.push_back(NOISE_GUI_EVENTS_SCROLLBAR_BUTTONDOWN);
			}
		}

		//UPDATE EVENTS LIST
		mFunction_CallMsgProcFunctionAndUpdateEventList(tmpEventList, pBar);
	}//next Scroll Bar

}

void NoiseGUIManager::mFunction_UpdateTextBoxs()
{
	//Update internal TextDynamic ,respond to keyboard input....
	//and textbox have the attribute of "focus"

	//used to validate focus
	BOOL isMouseLeftButtonDown = m_pInputEngine->IsMouseButtonPressed(NOISE_MOUSEBUTTON_LEFT);

	for (auto pTextBox : *m_pChildTextList)
	{
		//validate 
		if (pTextBox->mIsEnabled == FALSE)continue;

		//to store events message
		std::vector<UINT> tmpEventList;

		//it can judge whether to trigger TEXT_CHANGE events
		BOOL isStringModified = FALSE;

		//see if this textbox get focus
		if (isMouseLeftButtonDown)
		{
			BOOL isCursorWithinTextBox =
				gFunction_IsPointInRect2D(
					NVECTOR2(static_mouseX_BackBufferSpace, static_mouseY_BackBufferSpace),
					pTextBox->GetTopLeft(),
					pTextBox->GetBottomRight()
					);

			//button down && cursor inside
			if (isCursorWithinTextBox)
			{
				tmpEventList.push_back(NOISE_GUI_EVENTS_TEXTBOX_MOUSEDOWN);
				//get focus again
				if (pTextBox->mIsFocused == FALSE)tmpEventList.push_back(NOISE_GUI_EVENTS_TEXTBOX_GETFOCUS);
				
				//focus acquire
				pTextBox->mIsFocused = TRUE;
			}
			else
			{
				//lost focus (click outside)
				if (pTextBox->mIsFocused == TRUE)tmpEventList.push_back(NOISE_GUI_EVENTS_TEXTBOX_LOSTFOCUS);

				//pressed outside, lost focus
				pTextBox->mIsFocused = FALSE;
			}
		}//button down

		//retrieve text content of the textbox
		std::string tmpTextBoxString;
		pTextBox->GetTextAscii(tmpTextBoxString);

		//process keyboard input
		if (pTextBox->mIsFocused)
		{
			for (UINT i = 0;i < 0xED;++i)
			{
				if (m_pInputEngine->IsKeyPressed(i)==TRUE)
				{
					UINT scanCodeMapResult = 0;
					
					//map scan code to ASCII char
					scanCodeMapResult = gFunction_MapDInputScanCodeToAscii(i);

					//....
					if (scanCodeMapResult != 0)
					{
						//isprint() judge if the char has visible form (including space)
						if (isprint(scanCodeMapResult))
						{
							tmpTextBoxString.push_back(scanCodeMapResult);
							isStringModified = TRUE;
							break;
						};
					}
					
					if (m_pInputEngine->IsKeyPressed(NOISE_KEY_BACK)==TRUE)
					{
						if (tmpTextBoxString.size() > 0)		
						{
							isStringModified = TRUE;
							tmpTextBoxString.pop_back();
							break;
						}
					}

				}//isKeyPressed
			}//for every virtual key
		}//isFocused


		//update string
		if (isStringModified)
		{
			tmpEventList.push_back(NOISE_GUI_EVENTS_TEXTBOX_TEXTCHANGE);
		}
		pTextBox->SetTextAscii(tmpTextBoxString);
	
		//align internal Text to match the size of TextBox
		pTextBox->m_pTextDynamic->SetCenterPos(pTextBox->GetCenterPos());
		pTextBox->m_pTextDynamic->SetWidth(pTextBox->GetWidth());
		pTextBox->m_pTextDynamic->SetHeight(pTextBox->GetHeight());
		pTextBox->m_pTextDynamic->SetEnabled(pTextBox->IsEnabled());

		//UPDATE EVENT LIST
		mFunction_CallMsgProcFunctionAndUpdateEventList(tmpEventList, pTextBox);

	}//TextBox List
};

void NoiseGUIManager::mFunction_UpdateAllInternalGraphicObjs()
{
	//only update graphic objects, other internal component (e.g. button inside scrollBar)
	//will not be directly updated here...the reason that updating graphic objs after updating 
	//the infos of components is to wait for all container info are finally determined.NoiseGraphicObject is 
	//the real ,basic, graphic container

	//update buttons
	for (auto pCurrButton : *m_pChildButtonList)
	{
		if (pCurrButton->IsEnabled() == FALSE)continue;
		switch (pCurrButton->mButtonState)
		{
		default:
		case NOISE_GUI_BUTTON_STATE_COMMON:
			pCurrButton->m_pGraphicObj->SetRectangle(
				0,
				pCurrButton->GetTopLeft(),
				pCurrButton->GetBottomRight(),
				pCurrButton->GetBasicColor(),
				pCurrButton->mTextureID_MouseAway
				);
			break;

		case NOISE_GUI_BUTTON_STATE_MOUSEBUTTONDOWN:
			pCurrButton->m_pGraphicObj->SetRectangle(
				0,
				pCurrButton->GetTopLeft(),
				pCurrButton->GetBottomRight(),
				pCurrButton->GetBasicColor(),
				pCurrButton->mTextureID_MousePressedDown
				);
			break;

		case NOISE_GUI_BUTTON_STATE_MOUSEON:
			pCurrButton->m_pGraphicObj->SetRectangle(
				0,
				pCurrButton->GetTopLeft(),
				pCurrButton->GetBottomRight(),
				pCurrButton->GetBasicColor(),
				pCurrButton->mTextureID_MouseOn
				);
			break;
		}
	}

	for (auto pBar : *m_pChildScrollBarList)
	{
		if (pBar->IsEnabled() == FALSE)continue;
		//only 1 rect for the groove texture
		pBar->m_pGraphicObj_Groove->SetRectangle(
			0,
			pBar->GetCenterPos(),
			pBar->GetWidth(),
			pBar->GetHeight(),
			pBar->GetBasicColor(),
			pBar->mTextureID_Groove
			);
	}

	const UINT rectID_textBoxBackground = 0;
	const UINT rectID_textBoxCursor = 1;

	for (auto pTextBox : *m_pChildTextList)
	{
			if (pTextBox->IsEnabled() == FALSE)continue;

			//background
			pTextBox->m_pGraphicObj->SetRectangle(
				rectID_textBoxBackground,
				pTextBox->GetCenterPos(),
				pTextBox->GetWidth(),
				pTextBox->GetHeight(),
				pTextBox->GetBasicColor(),
				pTextBox->mTextureID_BackGround
				);
			// cursor
			pTextBox->m_pGraphicObj->SetRectangle(
				rectID_textBoxCursor,
				pTextBox->GetCenterPos(),
				pTextBox->GetWidth(),
				pTextBox->GetHeight(),
				pTextBox->GetBasicColor(),
				pTextBox->mTextureID_Cursor
				);
			//internal TEXT will be updated before rendering
	}

}

