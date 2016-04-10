
/***********************************************************************

							类：NOISE GUI Component Base

				Description : Some Common Attribute/Events generation/operation
				is encapsuled in this class

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::GUI;

IComponentBase::IComponentBase()
{
	mIsFocused = FALSE;
	mDepthLevel = 0;
	mHasBeenPressedDown = FALSE;
	m_pFunction_EventMessageProcess = nullptr;
	m_pEventList = new std::vector<UINT>;
}

//passive msg acquisition
void	IComponentBase::SetEventProcessCallbackFunction(UINT(*pFunc)(UINT NoiseGUIEvent))
{
	if (pFunc != nullptr)m_pFunction_EventMessageProcess = pFunc;
};

//active event acquisition
void	IComponentBase::GetEventList(std::vector<UINT>& outEventList)
{
	outEventList.assign(m_pEventList->begin(), m_pEventList->end());

	//clear all events (like win api GetMessage()???)
	m_pEventList->clear();
};

//是否已获取焦点
BOOL IComponentBase::IsFocused()
{
	return mIsFocused; 
}

void IComponentBase::SetDepthLevel(UINT depth)
{
	//a posZ can't be too big ,in graphic object we will map posZ to unit space (0,1) for 2d rendering
	mDepthLevel = depth>NOISE_GUI_CONST_MAX_POSITION_Z? NOISE_GUI_CONST_MAX_POSITION_Z: depth;
	mPosZ = float(depth) / NOISE_GUI_CONST_MAX_POSITION_Z;
}

UINT IComponentBase::GetDepthLevel()
{
	return mDepthLevel;
}



/***********************************************************************
							P R I V A T E / P R O T E C T E D 
************************************************************************/

 void IComponentBase::mFunction_GenerateMouseEventsAndValidateFocus(std::vector<UINT>& outEventList, BOOL isMouseInContainer, N_GUI_MOUSE_INFO& mouseInfo)
{
	//1.validate focus
	//2.mouse event:button down/up/move...
	//the POS Z is used to implement component overlapping


	//-------------------VALIDATE FOCUS---------------
	//..."focus" ....ehhh,means the component got user's attention and the ability to be directly handled
	if (mouseInfo.isMouseLeftPressedDown)
	{
		//button down && cursor inside && not overlapped by other(mouse is deep enough)
		if (isMouseInContainer==TRUE && mouseInfo.mouseDepthLevel>=mDepthLevel )
		{
			outEventList.push_back(NOISE_GUI_EVENTS_COMMON_MOUSEDOWN);
			//get focus again
			if (mIsFocused == FALSE)outEventList.push_back(NOISE_GUI_EVENTS_COMMON_GETFOCUS);

			//focus acquire
			mIsFocused = TRUE;
		}
		else
		{
			//lost focus (click outside)
			if (mIsFocused == TRUE)outEventList.push_back(NOISE_GUI_EVENTS_COMMON_LOSTFOCUS);

			//pressed outside, lost focus
			mIsFocused = FALSE;
		}
	}//button down


	//--------------------------MOUSE EVENT--------------------------
	//if mouse lies in the rect of button
	if (isMouseInContainer==TRUE && mouseInfo.mouseDepthLevel >= mDepthLevel)
	{
		//message proc callback function
		outEventList.push_back(NOISE_GUI_EVENTS_COMMON_MOUSEON);

		if (mouseInfo.isMouseLeftPressedDown)
		{
			//cursor is within the button and mouseLeft has been pressed down
			outEventList.push_back(NOISE_GUI_EVENTS_COMMON_MOUSEDOWN);

		}//move pressed

		//now mouse is down , let's see if the mouse is dragging the button
		if (mouseInfo.isMouseMoving)
		{
			outEventList.push_back(NOISE_GUI_EVENTS_COMMON_MOUSEMOVE);
		}

		//mouse button up
		if ((mouseInfo.isMouseLeftPressedDown==FALSE) && mHasBeenPressedDown==TRUE)
		{
			//a button down and up makes a "press"
			outEventList.push_back(NOISE_GUI_EVENTS_COMMON_MOUSEUP);
			mHasBeenPressedDown = FALSE;
		}

	}//...
	else//if cursor isn't within the button
	{
		//button was pressed down inside but is released outside , state must be updated
			mHasBeenPressedDown = FALSE;
	}


}

void	IComponentBase::mFunction_CallMsgProcFunctionAndUpdateEventList(std::vector<UINT>& eventList)
{

	m_pEventList->clear();
	for (UINT currEvent : eventList)
	{
		//the function ptr cannot be NULL
		if (m_pFunction_EventMessageProcess)
		{
			//call back to main .exe using function ptr
			(m_pFunction_EventMessageProcess)(currEvent);
		}

		//add to its message list
		m_pEventList->push_back(currEvent);
	}
};