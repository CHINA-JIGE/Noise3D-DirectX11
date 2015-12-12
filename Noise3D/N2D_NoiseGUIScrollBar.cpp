
/***********************************************************************

					class: NOISE GUI Scroll Bar

				Description : scrolling bar (mostly used for adjusting value)

************************************************************************/

#include "Noise3D.h"

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

void NoiseGUIScrollBar::Destroy()
{
	m_pButtonScrolling->SelfDestruction();
	m_pGraphicObj_Groove->SelfDestruction();

}
