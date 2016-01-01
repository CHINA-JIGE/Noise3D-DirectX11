
/***********************************************************************

					class: NOISE GUI Scroll Bar

				Description : scrolling bar (mostly used for adjusting value)

************************************************************************/

#include "NoiseGUISystem.h"


NoiseGUIScrollBar::NoiseGUIScrollBar(BOOL isHorizontal, float maxValue , float minValue)
{
	m_pEventList					= new std::vector<UINT>;
	m_pGraphicObj_Groove				= new NoiseGraphicObject;
	m_pButtonScrolling		= new NoiseGUIButton;
	mScrollWheelSpeed		= 1.0f;
	mIsMouseWheelScrollingEnabled =FALSE;
	SetAlignment(isHorizontal);
	SetWidth(300.0f);
	SetHeight(30.0f);
	mScrollButtonLength =isHorizontal? GetWidth()/4:GetHeight()/4;

	mCurrentValue=0.5f;
	SetValueRange(minValue, maxValue);
}

void NoiseGUIScrollBar::SetAlignment(BOOL isHorizontal)
{
	mIsHorizontal = isHorizontal;
	if (mIsHorizontal)
	{
		m_pButtonScrolling->SetDragableY(FALSE);
		m_pButtonScrolling->SetDragableX(TRUE);
	}
	else
	{
		m_pButtonScrolling->SetDragableY (TRUE);
		m_pButtonScrolling->SetDragableX (FALSE);
	}
}

void NoiseGUIScrollBar::SetTexture_ScrollGroove(UINT texID)
{
	mTextureID_Groove = texID;
}

void NoiseGUIScrollBar::SetTexture_ScrollButton(NOISE_GUI_BUTTON_STATE btnState, UINT texID)
{
	m_pButtonScrolling->SetTexture(btnState, texID);
}

void NoiseGUIScrollBar::SetValue(float val)
{
	mCurrentValue = gFunction_Clampf(val, mRangeMin, mRangeMax);
}

float NoiseGUIScrollBar::GetValue()
{
	return mCurrentValue;
};

void NoiseGUIScrollBar::SetValueRange(float minValue,float maxValue)
{
	//adjust range if it's invalid,new interval is 0.001f
	if (maxValue <= minValue)maxValue = minValue + 0.001f;
	mRangeMin = minValue;
	mRangeMax = maxValue;
	SetValue(mCurrentValue);
}

void NoiseGUIScrollBar::SetScrollButtonLength(float btnPixelLength)
{
	//restrict minimum length
	mScrollButtonLength =( btnPixelLength > 10.0f ? btnPixelLength : 10.0f);
	
	//no longer than the groove 
	if (mIsHorizontal)
	{
		if (mScrollButtonLength > GetWidth())mScrollButtonLength = GetWidth();
	}
	else
	{
		if (mScrollButtonLength > GetHeight())mScrollButtonLength = GetHeight();
	}
}

void NoiseGUIScrollBar::SetScrollWheelSpeed(float fSpeed)
{
	mScrollWheelSpeed = fSpeed;
}

void NoiseGUIScrollBar::SetEnabled(BOOL isEnabled)
{
	m_pButtonScrolling->SetEnabled(isEnabled);
	mIsEnabled =isEnabled;
}


/***********************************************************************
									P R I V A T E
************************************************************************/

void NoiseGUIScrollBar::Destroy()
{
	m_pButtonScrolling->SelfDestruction();
	m_pGraphicObj_Groove->SelfDestruction();

}

void NoiseGUIScrollBar::mFunction_Update(BOOL isMouseInContainer, N_GUI_MOUSE_INFO& mouseInfo)
{
	//to store events message
	std::vector<UINT> tmpEventList;

	if (IsEnabled() == FALSE)return;

	//-----------------------Generate Common Events/Validate Focus--------------------
	mFunction_GenerateMouseEventsAndValidateFocus(tmpEventList, isMouseInContainer, mouseInfo);

	//---------------------UpdatePositions(Bind button the groove)-------------------------
	NVECTOR2 btnPos = m_pButtonScrolling->GetCenterPos();
	NVECTOR2 btnPosTopLeft = m_pButtonScrolling->GetTopLeft();
	NVECTOR2 btnPosBottomRight = m_pButtonScrolling->GetBottomRight();
	NVECTOR2 barPos =GetCenterPos();
	NVECTOR2 barPosTopLeft = GetTopLeft();
	NVECTOR2 barPosBottomRight = GetBottomRight();
	float btnWidth		= m_pButtonScrolling->GetWidth();
	float btnHeight		= m_pButtonScrolling->GetHeight();
	float	barWidth		= GetWidth();
	float barHeight		=GetHeight();

	//scroll by wheel will be available when focused
	if (mIsFocused == FALSE)goto label_UpdateEventList;
	
	if (mIsHorizontal)
	{
		//move button :  bind scroll button to the scrolling groove
		m_pButtonScrolling->SetCenterPos(gFunction_Clampf(btnPos.x + float(mouseInfo.mouseScrollDiff), barPosTopLeft.x + btnWidth / 2.0f, barPosBottomRight.x - btnWidth / 2.0f), barPos.y);
		m_pButtonScrolling->SetWidth(mScrollButtonLength);
		m_pButtonScrolling->SetHeight(GetHeight());

		//derived by the position of scroll button
		float valueRatio = (btnPosTopLeft.x - barPosTopLeft.x) / (barWidth - btnWidth);

		//linear interpolation
		float newValue = gFunction_Lerp(mRangeMin,mRangeMax, valueRatio);

		//generate "SCROLL" event
		if (newValue != mCurrentValue)
		{
			tmpEventList.push_back(NOISE_GUI_EVENTS_SCROLLBAR_SCROLL);
		}
		mCurrentValue = gFunction_Clampf(newValue, mRangeMin,mRangeMax);
	}
	else
	{
		//move button :  bind scroll button to the scrolling groove
		m_pButtonScrolling->SetCenterPos(barPos.x, gFunction_Clampf(btnPos.y + float(mouseInfo.mouseScrollDiff), barPosTopLeft.y + btnHeight / 2.0f, barPosBottomRight.y - btnHeight / 2.0f));
		m_pButtonScrolling->SetWidth(barWidth);
		m_pButtonScrolling->SetHeight(mScrollButtonLength);

		float valueRatio = (btnPosTopLeft.y - barPosTopLeft.y) / (barHeight - btnHeight);

		//linear interpolation
		float newValue = gFunction_Lerp(mRangeMin, mRangeMax, valueRatio);

		//generate "SCROLL" event
		if (newValue !=mCurrentValue)
		{
			tmpEventList.push_back(NOISE_GUI_EVENTS_SCROLLBAR_SCROLL);
		}
		mCurrentValue = gFunction_Clampf(newValue, mRangeMin,mRangeMax);

	}

//------------------------------------------------------------------------------

	label_UpdateEventList:
	//UPDATE EVENTS LIST
	mFunction_CallMsgProcFunctionAndUpdateEventList(tmpEventList);
}

void  NoiseGUIScrollBar::mFunction_UpdateGraphicObject()
{
	//another internal graphic obj will be updated in 

	m_pGraphicObj_Groove->SetRectangle(
		0,
		GetCenterPos(),
		GetWidth(),
		GetHeight(),
		GetBasicColor(),
		mTextureID_Groove
		);

	//scroll groove must be deeper
	m_pGraphicObj_Groove->SetRectangleDepth(0, mPosZ+ NOISE_GUI_CONST_LEVEL_DEPTH_INTERVAL/5.0f);
	//scroll Bar
	m_pButtonScrolling->SetPosZ(mPosZ);
}
