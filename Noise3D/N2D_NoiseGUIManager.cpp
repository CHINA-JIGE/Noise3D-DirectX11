
/***********************************************************************

								¿‡£∫NOISE GUI manager

				Description : manage different GUI objects

************************************************************************/

#include "Noise3D.h"

NoiseGUIManager::NoiseGUIManager()
{
	m_pChildButtonList	= new std::vector<NoiseGUIButton*>;
	m_pChildTextList		= new std::vector<NoiseGUITextBox*>;
	m_pChildScrollBarList = new std::vector<NoiseGUIScrollBar*>;
}

void NoiseGUIManager::Destroy()
{
	for (auto obj : *m_pChildButtonList)
	{
		if(obj)obj->SelfDestruction();
	}

	for (auto obj : *m_pChildTextList)
	{
		if (obj)obj->SelfDestruction();
	}

	for (auto obj : *m_pChildScrollBarList)
	{
		if (obj)obj->SelfDestruction();
	}
};

BOOL NoiseGUIManager::CreateButton(NoiseGUIButton& refObject)
{
	try
	{
		refObject.Initialize();
	}
		catch (std::runtime_error)
	{
		DEBUG_MSG1("Object Has Been Created!");
		return FALSE;
	}

	//add button to child button list
	m_pChildButtonList->push_back(&refObject);
	refObject.m_pFatherGUIMgr = this;

	//initialized here because GUI manager have access to Scene
	m_pFatherScene->CreateGraphicObject(*refObject.m_pGraphicObj);

	//create graphic object
	UINT rectID = 0;
	rectID = refObject.m_pGraphicObj->AddRectangle(NVECTOR2(0, 0), NVECTOR2(100.0f, 50.0f),
		NVECTOR4(0.3f, 0.3f, 1.0f, 1.0f), NOISE_MACRO_INVALID_TEXTURE_ID);


	return TRUE;
}

BOOL NoiseGUIManager::CreateTextBox(NoiseGUITextBox& refObject)
{
	try
	{
		refObject.Initialize();
	}
	catch (std::runtime_error)
	{
		DEBUG_MSG1("Object Has Been Created!");
		return FALSE;
	}

	//add label ( pure text)
	m_pChildTextList->push_back(&refObject);
	refObject.m_pFatherGUIMgr = this;

	return TRUE;
}

BOOL NoiseGUIManager::CreateScrollBar(NoiseGUIScrollBar & refObject)
{
	try
	{
		refObject.Initialize();
	}
	catch (std::runtime_error)
	{
		DEBUG_MSG1("Object Has Been Created!");
		return FALSE;
	}
	
	//initialized internal GUI button
	CreateButton(*refObject.m_pButtonScrolling);
	refObject.m_pGraphicObj->AddRectangle(NVECTOR2(0,0),NVECTOR2(200.0f,30.0f),NVECTOR4(1.0f,1.0f,1.0f,1.0f));

	m_pChildScrollBarList->push_back(&refObject);
	refObject.m_pFatherGUIMgr = this;

	return TRUE;
}

void	NoiseGUIManager::SetInputEngine(NoiseUtInputEngine& refInputE)
{
	m_pInputEngine = &refInputE;
};

void	NoiseGUIManager::Update()
{
	//input Engine must be set
	if (!m_pInputEngine)return;

	//if internal input engine was inoperative, then nth will be retrieved
	if (m_pInputEngine->IsInitialized())
	{

		//traverse various kinds of objects and send message

		mFunction_UpdateButtons();

		mFunction_UpdateScrollBars();

		mFunction_UpdateAllInternalGraphicObjs();
	}

}

void NoiseGUIManager::SetWindowHWND(HWND hWnd)
{
	mWindowHWND = hWnd;
};

/***********************************************************************
										P R I V A T E
************************************************************************/

void NoiseGUIManager::mFunction_UpdateButtons()
{

#pragma region MouseInfo

	UINT mouseScrPosX = m_pInputEngine->GetMouseScrPosX();
	UINT mouseScrPosY = m_pInputEngine->GetMouseScrPosY();

	//get the Rect info of render window
	RECT windowRect;
	GetWindowRect(mWindowHWND, &windowRect);

	//remember , render window might enlarge or shrink , (didnt match the back buffer)
	//so mouse coord must be scaled to fit 
	float scaleX = (float)(windowRect.right - windowRect.left) / gMainBufferPixelWidth;
	float scaleY = (float)(windowRect.bottom - windowRect.top) / gMainBufferPixelHeight;

	//relative cursor Pos (window scaling is taken into account)
	float mouseX_BackBufferSpace = (mouseScrPosX - windowRect.left) / scaleX;
	float mouseY_BackBufferSpace = (mouseScrPosY - windowRect.top) / scaleY;

#pragma endregion MouseInfo


	//to store events message
	std::vector<UINT> tmpEventList;

	//!!!!!!!watch out for the multi-level of 'if'  embbeding
	for (auto pCurrButton : *m_pChildButtonList)
	{

		if (pCurrButton->IsEnabled() == FALSE)continue;

#pragma region UpdateButtonState

		//if mouse lies in the rect of button
		if (pCurrButton->IsPointInContainer(NVECTOR2(mouseX_BackBufferSpace, mouseY_BackBufferSpace)))
		{
			//message proc callback function
			tmpEventList.push_back(NOISE_GUI_EVENTS_BUTTON_MOUSEON);

			if (m_pInputEngine->IsMouseButtonPressed(NOISE_MOUSEBUTTON_LEFT))
			{
				if (!pCurrButton->mButtonHasBeenPressedDown)
				{
					//the first time pressed down, record the relative pos of cursor to btn center
					pCurrButton->m_pMouseDown_OffsetFromCenter->x = pCurrButton->GetCenterPos().x- mouseX_BackBufferSpace ;
					pCurrButton->m_pMouseDown_OffsetFromCenter->y = pCurrButton->GetCenterPos().y- mouseY_BackBufferSpace;
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
						NVECTOR2(mouseX_BackBufferSpace, pCurrButton->GetCenterPos().y)
						+NVECTOR2(pCurrButton->m_pMouseDown_OffsetFromCenter->x,0));
				}
					
				if (pCurrButton->mIsDragableY)
				{
					pCurrButton->SetCenterPos(
						NVECTOR2(pCurrButton->GetCenterPos().x, mouseY_BackBufferSpace)
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


#pragma region CallMsgProcFunction / UpdateEventList

		pCurrButton->m_pEventList->clear();
		for (UINT buttonEventMsg : tmpEventList)
		{
			//the function ptr cannot be NULL
			if (pCurrButton->m_pFunction_EventMessageProcess)
			{
				//call back to main .exe using function ptr
				(pCurrButton->m_pFunction_EventMessageProcess)(buttonEventMsg);
			}

			//add to its message list
			pCurrButton->m_pEventList->push_back(buttonEventMsg);
		}
	

	#pragma endregion CallMsgProcFunction / UpdateEventList

	}//end button traverse

}

void NoiseGUIManager::mFunction_UpdateScrollBars()
{
	//to store events message
	std::vector<UINT> tmpEventList;
	
	//mouse scrolling wheel movement
	float mouseScrollDiff = -float(m_pInputEngine->GetMouseScrollDiff())/30.0f;

	//internal button has been updated,so DRAG operation has been done
	for (auto pBar : *m_pChildScrollBarList)
	{
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
			pBar->m_pButtonScrolling->SetCenterPos(gFunction_Clamp(btnPos.x + float(mouseScrollDiff), barPosTopLeft.x+btnWidth/2.0f, barPosBottomRight.x- btnWidth / 2.0f), barPos.y);
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
			pBar->mCurrentValue = gFunction_Clamp(newValue, pBar->mRangeMin, pBar->mRangeMax);
		}
		else
		{
			pBar->m_pButtonScrolling->SetCenterPos(barPos.x,gFunction_Clamp(btnPos.y + float(mouseScrollDiff), barPosTopLeft.y + btnHeight / 2.0f, barPosBottomRight.y - btnHeight / 2.0f));
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
			pBar->mCurrentValue = gFunction_Clamp(newValue, pBar->mRangeMin, pBar->mRangeMax);

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

	#pragma region CallMsgProcFunction / UpdateEventList
		pBar->m_pEventList->clear();
		for (auto barEvent : tmpEventList)
		{
			if (pBar->m_pFunction_EventMessageProcess != nullptr)
			{
				pBar->m_pFunction_EventMessageProcess(barEvent);
			}

			pBar->m_pEventList->push_back(barEvent);
		}

	#pragma endregion CallMsgProcFunction / UpdateEventList

	}//next Scroll Bar

}

void NoiseGUIManager::mFunction_UpdateAllInternalGraphicObjs()
{
	//update buttons
	for (auto pCurrButton : *m_pChildButtonList)
	{
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
		//only 1 rect for the groove texture
		pBar->m_pGraphicObj->SetRectangle(
			0,
			pBar->GetCenterPos(),
			pBar->GetWidth(),
			pBar->GetHeight(),
			pBar->GetBasicColor(),
			pBar->mTextureID_Groove
			);
	}


}

