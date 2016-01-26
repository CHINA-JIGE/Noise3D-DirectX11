
/***********************************************************************

							h:Noise GUI Oscilloscope
					(digital wave curve visualization)
									"Ê¾²¨Æ÷"

************************************************************************/

#pragma once

//A button ; can be embbed into other GUI object
class _declspec(dllexport) NoiseGUIOscilloscope :
	public	NoiseClassLifeCycle,
	public	NoiseGUIComponentBase
{
public:
	friend class NoiseGUIManager;
	friend class NoiseRenderer;
	
	NoiseGUIOscilloscope(float maxAmplitude,float signalTimeInterval, UINT(*pFunc)(UINT NoiseGUIEvent) = nullptr);

	void	SetBackGroundTexture(UINT texID);

	void	SetWaveGraphicType(BOOL isLineSegments);

	void	SetMaxAmplitude(float waveAmp);

	void	SetGridColor(NVECTOR4 color);

	void	SetWaveColor(NVECTOR4 color);

	void	SetScreenTimeRange(float timeMin,float timeMax);

	void	SetSignalTimeInterval(float time_ms);

	void	SetTriggerRecordingTime(float time_ms);

	UINT GetBackGroundTexture();

	float	GetMaxAmplitude();

	NVECTOR4	GetGridColor();

	NVECTOR4	GetWaveColor();

	void	PushSignal(float signalAmplitude);

	void	Trigger();

private:

	void	Destroy();

	void NOISE_MACRO_FUNCTION_EXTERN_CALL	mFunction_Update(BOOL isMouseInContainer, N_GUI_MOUSE_INFO& mouseInfo);

	void NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_UpdateGraphicObject();

	void	mFunction_ResampleSignalToFitTheScreen(UINT sequenceID,std::vector<NVECTOR2>& outResampledSequence);

	NoiseGUIManager*		m_pFatherGUIMgr;
	NoiseGraphicObject*		m_pGraphicObj_Grid;
	NoiseGraphicObject*		m_pGraphicObj_Wave;

	//NoiseUtTimer*		m_pTimer;
	NOISE_GRAPHIC_OBJECT_TYPE mWaveGraphicTopoType;//	POINT/LINE
	double		mSignalTimeInterval;
	double		mTriggerRecordingTime;//how long time should be presented to screen after each trigger
	BOOL	mIsTriggered;
	UINT		mBackGroundTexID;
	float		mMaxAmplitude;//max amplitude on screen
	double	mScreenTimeRange_Min;//set Time Range which the User Interface can present
	double	mScreenTimeRange_Max;//say that there is a series of 50¦Ìs signal,but we just put 20¦Ìs on the screen
	NVECTOR4*		m_pWaveColor;
	NVECTOR4*		m_pGridColor;

	//general signal list that holds up all signal
	std::vector<float>*						m_pSignalList;
	//record a series of signals which will be presented to screen after "trigger"
	std::vector<std::vector<float>>* 	m_pSignalSequenceList;

};