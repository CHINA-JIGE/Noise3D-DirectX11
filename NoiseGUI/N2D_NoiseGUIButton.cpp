
/***********************************************************************

							¿‡£∫NOISE GUI BUTTON

				Description : command button

************************************************************************/

#include "NoiseGUISystem.h"

NoiseGUIButton::NoiseGUIButton(UINT(*pFunc)(UINT NoiseGUIEvent))
{
	*m_pBasicColor = NVECTOR4(0.3f,0.3f,1.0f,1.0f);
	m_pMouseDown_OffsetFromCenter = new NVECTOR2(0, 0);
	m_pGraphicObj = new NoiseGraphicObject;
	m_pEventList = new std::vector<UINT>;
	mTextureID_MouseAway				= NOISE_MACRO_INVALID_TEXTURE_ID;
	mTextureID_MouseOn					= NOISE_MACRO_INVALID_TEXTURE_ID;
	mTextureID_MousePressedDown	= NOISE_MACRO_INVALID_TEXTURE_ID;
	mButtonState = NOISE_GUI_BUTTON_STATE_COMMON;
	mIsDragableX = FALSE;
	mIsDragableY = FALSE;
	m_pFunction_EventMessageProcess = pFunc;//default:nullptr
}

void NoiseGUIButton::SetDragableX(BOOL dragableX)
{
	mIsDragableX = dragableX;
}

void NoiseGUIButton::SetDragableY(BOOL dragableY)
{
	mIsDragableY = dragableY;
}

void NoiseGUIButton::SetTexture(NOISE_GUI_BUTTON_STATE btnState, UINT texID)
{
	switch (btnState)
	{
	case NOISE_GUI_BUTTON_STATE_COMMON:
		mTextureID_MouseAway = texID;
		break;
	case NOISE_GUI_BUTTON_STATE_MOUSEON:
		mTextureID_MouseOn = texID;
		break;
	case NOISE_GUI_BUTTON_STATE_MOUSEBUTTONDOWN:
		mTextureID_MousePressedDown = texID;
		break;
	default:
		break;
	}
}


/************************************************************************
                                     P R I V A T E          
************************************************************************/

void NoiseGUIButton::Destroy()
{
	m_pFunction_EventMessageProcess = nullptr;
	m_pGraphicObj->SelfDestruction();
}

/*void	NoiseGUIButton::mFunction_Update(BOOL isMouseInContainer, BOOL isMouseLeftPressedDown, BOOL isMouseMoving, float mousePosX_backBufferSpace, float mousePosY_backBufferSpace)
{

	//to store events
	std::vector<UINT> tmpEventList;

	if (IsEnabled() == FALSE)return;

	//---------------------------UPDATE BUTTON STATE-----------------------


	//if mouse lies in the rect of button
	if (isMouseInContainer)
	{
		//message proc callback function
		tmpEventList.push_back(NOISE_GUI_EVENTS_COMMON_MOUSEON);

		if (isMouseLeftPressedDown)
		{
			if (!mButtonHasBeenPressedDown)
			{
				//the first time pressed down, record the relative pos of cursor to btn center
				m_pMouseDown_OffsetFromCenter->x = GetCenterPos().x - mousePosX_backBufferSpace;
				m_pMouseDown_OffsetFromCenter->y = GetCenterPos().y - mousePosY_backBufferSpace;
			}

			//cursor is within the button and mouseLeft has been pressed down
			tmpEventList.push_back(NOISE_GUI_EVENTS_COMMON_MOUSEDOWN);
			mButtonState = NOISE_GUI_BUTTON_STATE_MOUSEBUTTONDOWN;
			mButtonHasBeenPressedDown = TRUE;

			//now mouse is down , let's see if the mouse is dragging the button
			//apply relative movemwnt along with mouse
			//use position difference will lead to some aliasing (because of delay)
			//and an offset from cursor to button center should be accumulated
			if (mIsDragableX)
			{
				SetCenterPos(
					NVECTOR2(mousePosX_backBufferSpace, GetCenterPos().y)
					+ NVECTOR2(m_pMouseDown_OffsetFromCenter->x, 0));
			}

			if (mIsDragableY)
			{
				SetCenterPos(
					NVECTOR2(GetCenterPos().x, mousePosY_backBufferSpace)
					+ NVECTOR2(0, m_pMouseDown_OffsetFromCenter->y));
			}

		}//move pressed
		else
		{
			//cursor is within the button and mouseLeft is up
			mButtonState = NOISE_GUI_BUTTON_STATE_MOUSEON;

			//now mouse is down , let's see if the mouse is dragging the button
			if (isMouseMoving)
			{
				tmpEventList.push_back(NOISE_GUI_EVENTS_COMMON_MOUSEMOVE);
			}
		}

		//mouse button up
		if ( (!isMouseLeftPressedDown) && mButtonHasBeenPressedDown)
		{
			//a button down and up makes a "press"
			tmpEventList.push_back(NOISE_GUI_EVENTS_COMMON_MOUSEPRESSED);
			mButtonHasBeenPressedDown = FALSE;
		}
	}
	else//if cursor isn't within the button
	if(isMouseInContainer==FALSE)
	{
		mButtonHasBeenPressedDown = FALSE;
		mButtonState = NOISE_GUI_BUTTON_STATE_COMMON;
	}

	//button was pressed down inside but is released outside , state must be updated
	if (!isMouseLeftPressedDown)
	{
		mButtonHasBeenPressedDown = FALSE;
	}


	//--------------------------------------------------

	//UPDATE EVENTS LIST
	mFunction_CallMsgProcFunctionAndUpdateEventList(tmpEventList);
}
*/

void	NoiseGUIButton::mFunction_Update(BOOL isMouseInContainer, N_GUI_MOUSE_INFO& mouseInfo)
{

	//to store events
	std::vector<UINT> tmpEventList;

	if (IsEnabled() == FALSE)return;

	//-----------------------Generate Common Events/Validate Focus--------------------
	mFunction_GenerateMouseEventsAndValidateFocus(tmpEventList,  isMouseInContainer, mouseInfo);

	//---------------------------UPDATE BUTTON STATE-----------------------------------
	for (auto ev : tmpEventList)
	{
		switch (ev)
		{
		case NOISE_GUI_EVENTS_COMMON_MOUSEDOWN:
			if (mHasBeenPressedDown==FALSE)
			{
				//the first time pressed down, record the relative pos of cursor to btn center
				m_pMouseDown_OffsetFromCenter->x = GetCenterPos().x - mouseInfo.mouseX_backBufferSpace;
				m_pMouseDown_OffsetFromCenter->y = GetCenterPos().y - mouseInfo.mouseY_backBufferSpace;
			}

			//cursor is within the button and mouseLeft has been pressed down
			mButtonState = NOISE_GUI_BUTTON_STATE_MOUSEBUTTONDOWN;
			mHasBeenPressedDown = TRUE;

			//now mouse is down , let's see if the mouse is dragging the button
			//apply relative movemwnt along with mouse
			//use position difference will lead to some aliasing (because of delay)
			//and an offset from cursor to button center should be accumulated
			if (mIsDragableX)
			{
				SetCenterPos(
					NVECTOR2(mouseInfo.mouseX_backBufferSpace, GetCenterPos().y)
					+ NVECTOR2(m_pMouseDown_OffsetFromCenter->x, 0));
			}

			if (mIsDragableY)
			{
				SetCenterPos(
					NVECTOR2(GetCenterPos().x, mouseInfo.mouseY_backBufferSpace)
					+ NVECTOR2(0, m_pMouseDown_OffsetFromCenter->y));
			}
			break;


		case NOISE_GUI_EVENTS_COMMON_MOUSEON:
			//cursor is within the button and mouseLeft is up
			mButtonState = NOISE_GUI_BUTTON_STATE_MOUSEON;

		default:
			break;

		}
	}

	if (isMouseInContainer == FALSE)
	{
		mButtonState = NOISE_GUI_BUTTON_STATE_COMMON;
	}


	//----------------------CALLBACK & UPDATE EVENTS LIST----------------------
	mFunction_CallMsgProcFunctionAndUpdateEventList(tmpEventList);
}

void  NoiseGUIButton::mFunction_UpdateGraphicObject()
{
	switch (mButtonState)
	{
	default:
	case NOISE_GUI_BUTTON_STATE_COMMON:
		m_pGraphicObj->SetRectangle(
			0,
			GetTopLeft(),
			GetBottomRight(),
			GetBasicColor(),
			mTextureID_MouseAway
			);
		break;

	case NOISE_GUI_BUTTON_STATE_MOUSEBUTTONDOWN:
		m_pGraphicObj->SetRectangle(
			0,
			GetTopLeft(),
			GetBottomRight(),
			GetBasicColor(),
			mTextureID_MousePressedDown
			);
		break;

	case NOISE_GUI_BUTTON_STATE_MOUSEON:
		m_pGraphicObj->SetRectangle(
			0,
			GetTopLeft(),
			GetBottomRight(),
			GetBasicColor(),
			mTextureID_MouseOn
			);
		break;
	}

	//map posZ to (0 , 1) for 2D rendering
	m_pGraphicObj->SetRectangleDepth(0, mPosZ);

}
