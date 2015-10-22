
/***********************************************************************

								¿‡£∫NOISE GUI manager

				Description : manage different GUI objects

************************************************************************/

#include "Noise3D.h"

NoiseGUIManager::NoiseGUIManager()
{
	m_pChildGraphicObject = new NoiseGraphicObject;
	m_pChildButtonList	= new std::vector<NoiseGUIButton*>;
	m_pChildPictureList		= new std::vector<NoiseGUIPicture*>;
};

void	NoiseGUIManager::AddToRenderList()
{
	if (!m_pFatherScene)return;

	m_pChildGraphicObject->AddToRenderList();
};

BOOL NoiseGUIManager::AddButton(NoiseGUIButton * pButton)
{
	//add button to child button list
	if (pButton)
	{
		m_pChildButtonList->push_back(pButton);
		pButton->m_pFatherGUIMgr = this;

		//create graphic object
		UINT rectID = 0;
		rectID=m_pChildGraphicObject->AddRectangle(NVECTOR2(0, 0), NVECTOR2(100.0f, 50.0f),
					NVECTOR4(0.3f, 0.3f, 1.0f, 1.0f),NOISE_MACRO_INVALID_TEXTURE_ID);

		//tell the button its rect ID 
		pButton->mGraphicObject_RectID = rectID;

	}
	else
	{
		DEBUG_MSG1("Noise GUI mgr : Add Button : Ptr invalid!");
		return TRUE;
	}
	return TRUE;
}

BOOL NoiseGUIManager::AddPicture(NoiseGUIPicture * pLabel)
{
	//add label ( pure text)
	if (pLabel)
	{
		m_pChildPictureList->push_back(pLabel);
		pLabel->m_pFatherGUIMgr = this;
	}
	else
	{
		DEBUG_MSG1("Noise GUI mgr : Add Label : Ptr invalid!");
		return TRUE;
	}

	return TRUE;
}

void	NoiseGUIManager::SetInputEngine(NoiseUtInputEngine * pInputE)
{
	if (!pInputE)
	{
		m_pInputEngine = pInputE;
	}
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
		mFunction_UpdatePictures();

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

	#pragma region UpdateButtonState

		//if mouse lies in the rect of button
		if (pCurrButton->IsPointInContainer(NVECTOR2(mouseX_BackBufferSpace, mouseY_BackBufferSpace)))
		{
			//message proc callback function
			tmpEventList.push_back(NOISE_GUI_EVENTS_BUTTON_MOUSEON);

			if (m_pInputEngine->IsMouseButtonPressed(NOISE_MOUSEBUTTON_LEFT))
			{
				//cursor is within the button and mouseLeft has been pressed down
				tmpEventList.push_back(NOISE_GUI_EVENTS_BUTTON_MOUSEDOWN);
				pCurrButton->mButtonState = NOISE_GUI_BUTTON_STATE_MOUSEBUTTONDOWN;
				pCurrButton->mButtonHasBeenPressedDown = TRUE;
			}
			else
			{
				//cursor is within the button and mouseLeft is up
				pCurrButton->mButtonState = NOISE_GUI_BUTTON_STATE_MOUSEON;
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
		else
		{
			//if cursor isn't within the button
			pCurrButton->mButtonHasBeenPressedDown = FALSE;
			pCurrButton->mButtonState = NOISE_GUI_BUTTON_STATE_COMMON;
		}

		//button was pressed down inside but is released outside , state must be updated
		if (!m_pInputEngine->IsMouseButtonPressed(NOISE_MOUSEBUTTON_LEFT))
		{
			pCurrButton->mButtonHasBeenPressedDown = FALSE;
		}

	#pragma endregion UpdateButtonState


	#pragma region CallMsgProcFunction

		//the function ptr cannot be NULL
		if (pCurrButton->m_pFunction_EventMessageProcess)
		{
			for (auto buttonEventMsg : tmpEventList)
			{
				//call back to main .exe using function ptr
				(*pCurrButton->m_pFunction_EventMessageProcess)(buttonEventMsg);
			}
		}



	#pragma region CallMsgProcFunction


	#pragma region UpdateGraphicObj

		NVECTOR2 v;
		switch (pCurrButton->mButtonState)
		{
		default:
		case NOISE_GUI_BUTTON_STATE_COMMON:
			v = pCurrButton->GetTopLeft();
			m_pChildGraphicObject->SetRectangle(
				pCurrButton->mGraphicObject_RectID,
				pCurrButton->GetTopLeft(),
				pCurrButton->GetBottomRight(),
				pCurrButton->GetBasicColor(),
				pCurrButton->mTextureID_MouseAway
				);
			break;

		case NOISE_GUI_BUTTON_STATE_MOUSEBUTTONDOWN:
			m_pChildGraphicObject->SetRectangle(
				pCurrButton->mGraphicObject_RectID,
				pCurrButton->GetTopLeft(),
				pCurrButton->GetBottomRight(),
				pCurrButton->GetBasicColor(),
				pCurrButton->mTextureID_MousePressedDown
				);
			break;

		case NOISE_GUI_BUTTON_STATE_MOUSEON:
			m_pChildGraphicObject->SetRectangle(
				pCurrButton->mGraphicObject_RectID,
				pCurrButton->GetTopLeft(),
				pCurrButton->GetBottomRight(),
				pCurrButton->GetBasicColor(),
				pCurrButton->mTextureID_MouseOn
				);
			break;
		}


		#pragma endregion UpdateGraphicObj


	}//end button traverse

}

void NoiseGUIManager::mFunction_UpdatePictures()
{

};




