
/***********************************************************************

							class: NOISE GUI Scroll Bar

		Description :Oscilloscope , signal can be added to the queue,
		and the component will resample and show the curve on the screen

************************************************************************/

#include "NoiseGUISystem.h"

NoiseGUIOscilloscope::NoiseGUIOscilloscope(float maxAmplitude, float signalTimeInterval, UINT(*pFunc)(UINT NoiseGUIEvent))
{
	if(pFunc!=nullptr)m_pFunction_EventMessageProcess = pFunc;
	m_pGraphicObj_Grid = new NoiseGraphicObject;
	m_pGraphicObj_Wave = new NoiseGraphicObject;
	mMaxAmplitude = maxAmplitude;//max amplitude on screen
	mSignalTimeInterval = signalTimeInterval;
	mTriggerRecordingTime=20.0f;//how long time should be presented to screen after each trigger
	mIsTriggered=FALSE;
	mBackGroundTexID=NOISE_MACRO_INVALID_TEXTURE_ID;
	mScreenTimeRange_Min=0.0;//set Time Range which the User Interface can present
	mScreenTimeRange_Max=10.0; //say that there is a series of 20ms signal,but we just put 10ms on the screen
	m_pWaveColor = new NVECTOR4(0.3f,1.0f,0.3f,0.8f);
	m_pGridColor = new NVECTOR4(0.3f, 0.3f, 1.0f, 1.0f);
	m_pSignalList = new std::vector<float>;
	m_pSignalSequenceList=new 	std::vector<std::vector<float>>;
	mWaveGraphicTopoType = NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D;

}

void NoiseGUIOscilloscope::SetBackGroundTexture(UINT texID)
{
	mBackGroundTexID = texID;
}

void NoiseGUIOscilloscope::SetWaveGraphicType(BOOL isLineSegments)
{
	if (isLineSegments)
	{
		mWaveGraphicTopoType = NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D;
	}
	else
	{
		mWaveGraphicTopoType = NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D;

	}
}

void NoiseGUIOscilloscope::SetMaxAmplitude(float waveAmp)
{
	mMaxAmplitude = waveAmp > 0.0f ? waveAmp : 1.0f;
}

void NoiseGUIOscilloscope::SetGridColor(NVECTOR4 color)
{
	*m_pGridColor = color;
}

void NoiseGUIOscilloscope::SetWaveColor(NVECTOR4 color)
{
	*m_pGridColor = color;
}

void NoiseGUIOscilloscope::SetScreenTimeRange(float timeMin, float timeMax)
{
	mScreenTimeRange_Min =  timeMin;
	mScreenTimeRange_Max = timeMax;
}

void NoiseGUIOscilloscope::SetSignalTimeInterval(float time_ms)
{
	mSignalTimeInterval = time_ms > 0.0f ? time_ms : 0.0f;
}

void NoiseGUIOscilloscope::SetTriggerRecordingTime(float time_ms)
{
	//at least record one signal
	mTriggerRecordingTime = time_ms > mSignalTimeInterval ? time_ms : mSignalTimeInterval;
}

UINT NoiseGUIOscilloscope::GetBackGroundTexture()
{
	return mBackGroundTexID;
}

float NoiseGUIOscilloscope::GetMaxAmplitude()
{
	return mMaxAmplitude;
}

NVECTOR4 NoiseGUIOscilloscope::GetGridColor()
{
	return *m_pGridColor;
}

NVECTOR4 NoiseGUIOscilloscope::GetWaveColor()
{
	return *m_pWaveColor;
}

void NoiseGUIOscilloscope::PushSignal(float signalAmplitude)
{
	//--------------------------TRIGGER CONDITION-------------------------
	//we use fixed simple trigger condition for the time being, I must figure out 
	//how to let user select/define Trigger condition freely

	m_pSignalList->push_back(signalAmplitude);

	//check the Rising Edge
	const float signalThreshold = 0.5;

	if (m_pSignalList->size() >= 3)
	{
		UINT lastSignalIndex = m_pSignalList->size() - 1;

		//fulfill the condition to TRIGGER
		if (m_pSignalList->at(lastSignalIndex - 2) < signalThreshold
			&& m_pSignalList->at(lastSignalIndex) >= signalThreshold)
		{
			Trigger();
			m_pSignalList->clear();
			m_pSignalList->reserve(5000);
		}

	}

	UINT triggerSignalCount = UINT(mTriggerRecordingTime / mSignalTimeInterval);
	if (mIsTriggered == TRUE)
	{
		if (m_pSignalSequenceList->back().size() <= triggerSignalCount)
		{
			//add new signal to current series
			m_pSignalSequenceList->back().push_back(signalAmplitude);
		}
		else
		{
			mIsTriggered = FALSE;
		}
	}

}

void NoiseGUIOscilloscope::Trigger()
{
	if (mIsTriggered == FALSE)
	{
		//a new series is generated
		mIsTriggered = TRUE;
		std::vector<float> newTmpSeriesVector;
		m_pSignalSequenceList->push_back(newTmpSeriesVector);
	}
}

/***********************************************************************
										P R I V A T E
************************************************************************/


void NoiseGUIOscilloscope::Destroy()
{
}

void  NoiseGUIOscilloscope::mFunction_Update(BOOL isMouseInContainer, N_GUI_MOUSE_INFO & mouseInfo)
{
	//validate 
	if (mIsEnabled == FALSE)return;

	//to store events message
	std::vector<UINT> tmpEventList;

	//-----------------------Generate Common Events/Validate Focus--------------------
	mFunction_GenerateMouseEventsAndValidateFocus(tmpEventList, isMouseInContainer, mouseInfo);

	//UPDATE EVENT LIST
	mFunction_CallMsgProcFunctionAndUpdateEventList(tmpEventList);
}

void  NoiseGUIOscilloscope::mFunction_UpdateGraphicObject()
{
	NVECTOR2 topLeft = GetTopLeft();
	NVECTOR2 bottomRight = GetBottomRight();
	NVECTOR2 centerPos = GetCenterPos();

	//---------GRID-----------

	//whatever the order....6 lines are reserved for the grid
	m_pGraphicObj_Grid->SetLine2D(0,{ topLeft.x, centerPos.y },{ bottomRight.x, centerPos.y },*m_pGridColor,*m_pGridColor);
	m_pGraphicObj_Grid->SetLine2D(1,{ centerPos.x, topLeft.y },{ centerPos.x, bottomRight.y }, *m_pGridColor, *m_pGridColor);
	m_pGraphicObj_Grid->SetLine2D(2,topLeft,{ topLeft.x, bottomRight.y },*m_pGridColor,*m_pGridColor);
	m_pGraphicObj_Grid->SetLine2D(3,topLeft,{ bottomRight.x, topLeft.y },*m_pGridColor, *m_pGridColor);
	m_pGraphicObj_Grid->SetLine2D(4, bottomRight,{ topLeft.x, bottomRight.y }, *m_pGridColor, *m_pGridColor);
	m_pGraphicObj_Grid->SetLine2D(5,bottomRight,{ bottomRight.x, topLeft.y }, *m_pGridColor, *m_pGridColor);

	//---------RESERVE ENOUGH ELEMENT------------
	const UINT waveCount = 5;//how many waves will be drawn
	//there are many short sequences waiting to draw ,but we only draw some of them(the newest)
	UINT startSequenceID = m_pSignalSequenceList->size() > waveCount ? m_pSignalSequenceList->size()- waveCount : 0;
	
	//to avoid unnecessary element allocation,let's reserve enough space first
	UINT totalPointCount = 0;
	for (UINT i = startSequenceID;i < m_pSignalSequenceList->size();++i)totalPointCount += m_pSignalSequenceList->at(i).size();


	UINT count1 = 0;
	UINT count2 = 0;
	switch (mWaveGraphicTopoType)
	{
	//link adjacent points into line segments
	case NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D:
		m_pGraphicObj_Wave->AdjustElementCount(NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D, 0);
		m_pGraphicObj_Wave->AdjustElementCount(NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D, totalPointCount);
		break;

	case NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D:
		m_pGraphicObj_Wave->AdjustElementCount(NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D, totalPointCount);
		m_pGraphicObj_Wave->AdjustElementCount(NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D, 0);
		break;
	}


	//------------------RESAMPLE to present-----------------------------

	UINT graphicElementCount = 0;
	UINT currSequenceGraphicElementCount = 0;
	std::vector<NVECTOR2> resampledSequence;

	for (UINT i = startSequenceID;i < m_pSignalSequenceList->size();i++)
	{
		// filter/reconstruct/resample signals first
		resampledSequence.clear();
		mFunction_ResampleSignalToFitTheScreen(i, resampledSequence);

		//in resample sequence ,there are only points
		switch (mWaveGraphicTopoType)
		{
		//link adjacent points into line segments
		case NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D:

			//LINE
			currSequenceGraphicElementCount = resampledSequence.size()==0?0: resampledSequence.size()-1;

			//resampled signals can be now used
			for (UINT j = 0;j < currSequenceGraphicElementCount;j++)
			{
				m_pGraphicObj_Wave->SetLine2D(
					graphicElementCount + j,
					resampledSequence[j],
					resampledSequence[j + 1],
					*m_pWaveColor *float(1+i-startSequenceID)/float(waveCount),//the older wave fade out
					*m_pWaveColor* float(1+i- startSequenceID) / float(waveCount));
			}

			//an offset used by SetLine2D
			graphicElementCount += currSequenceGraphicElementCount;
			break;


		//present discrete points
		case NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D:

			currSequenceGraphicElementCount = resampledSequence.size();

			for (UINT j = 0;j < resampledSequence.size();j++)
			{
				m_pGraphicObj_Wave->SetPoint2D(
					graphicElementCount + j,
					resampledSequence.at(j),
					*m_pWaveColor* float(1+i - startSequenceID) / float(waveCount));//the older wave fade out
			}
			graphicElementCount += currSequenceGraphicElementCount;

			break;
		}

	}

	//------------------
	switch (mWaveGraphicTopoType)
	{
		//link adjacent points into line segments
	case NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D:
		m_pGraphicObj_Wave->AdjustElementCount(NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D, 0);
		m_pGraphicObj_Wave->AdjustElementCount(NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D, graphicElementCount);
		break;

	case NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D:
		m_pGraphicObj_Wave->AdjustElementCount(NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D, graphicElementCount);
		m_pGraphicObj_Wave->AdjustElementCount(NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D, 0);
		break;
	}

}

void NoiseGUIOscilloscope::mFunction_ResampleSignalToFitTheScreen(UINT sequenceID, std::vector<NVECTOR2>& outResampledSequence)
{
	//----now ,we gotta RESAMPLE signals to present the signals to screen
	//---Let's assume that one signal for one pixel X is the orginal size,
	//so maybe we should discuss the circumstances of Magnification/Minification
	std::vector<float>& currSequence = m_pSignalSequenceList->at(sequenceID);
	outResampledSequence.resize(currSequence.size());

	double screenTimeLength = (mScreenTimeRange_Max - mScreenTimeRange_Min);
	double sequenceTimeLength = currSequence.size()*mSignalTimeInterval;

	//IN MY ENGINE,2D GRAPHICS CAN BE DRAWN TO SUBPIXEL,
	//THUS MULTIPLE SIGNALS CROWDED IN ONE PIXEL IS ACCEPTABLE
	//ANTI-ALIASING WILL BE DONE.

	//horizontal distances between signals are greater than 1 pixel

		//  |____________SSSIIIIGGGG | NNNNAAAALLLL

		//point count depends on the scrranges
		for (UINT i = 0;i < currSequence.size();i++)
		{
			//first we will calculate the LERP ratio of the point in the direction of time axis (X),
			//intensity axis(Y) , then we can use the ratio to apply linear interpolation to points
			//to place them in correct place

			//sequence starts at 0.0ms ,adjacent signals have the same time interval 
			float signalScrRatioX = (i*float(mSignalTimeInterval - mScreenTimeRange_Min)) / screenTimeLength;
			float signalScrRatioY = currSequence.at(i)/ mMaxAmplitude;

			//this ratio is defined in Scr X Lerp Ratio Space
			float pointX = GetTopLeft().x + GetWidth()*gFunction_Clampf(signalScrRatioX, 0.0f, 1.0f);
			float pointY = GetCenterPos().y - GetHeight()/2.0f*gFunction_Clampf(signalScrRatioY, -1.0f, 1.0f);
			NVECTOR2 tmpPoint(pointX,pointY);
			outResampledSequence[i]=tmpPoint;
		}

	/*outResampledSequence.resize(GetWidth() / 5);
	for (UINT i = 0;i < GetWidth();i+=5)
		outResampledSequence.at(i/5)=(NVECTOR2(GetTopLeft().x + float(i) , GetCenterPos().y-10.0f));
		*/
}
