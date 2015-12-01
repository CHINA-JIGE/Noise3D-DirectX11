
/***********************************************************************

				CLASS: GUI Events Common Operation

************************************************************************/

#pragma once

//a base class that provide general life cycle management
class _declspec(dllexport) NoiseGUIEventCommonOperation
{
public:

	NoiseGUIEventCommonOperation()
	{
		m_pFunction_EventMessageProcess = nullptr;
		m_pEventList = new std::vector<UINT>;
	}

	void	SetEventProcessCallbackFunction(UINT(*pFunc)(UINT NoiseGUIEvent)) //passive msg acquisition
	{
		m_pFunction_EventMessageProcess = pFunc;
	};

	void	GetEventList(std::vector<UINT>& outEventList) //active event acquisition
	{
		outEventList.assign(m_pEventList->begin(), m_pEventList->end());

		//clear all events (like win api GetMessage()???)
		m_pEventList->clear();
	};

protected:
	UINT(*m_pFunction_EventMessageProcess)(UINT NoiseGUIEvent);//a callback function pointer
	std::vector<UINT>*	m_pEventList;//Events message for this button
};

