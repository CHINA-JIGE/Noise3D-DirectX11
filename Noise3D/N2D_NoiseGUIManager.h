
/***********************************************************************

									h£ºNoiseGUI

************************************************************************/
#pragma once

//the main GUI management class
public class _declspec(dllexport) NoiseGUIManager
{
public:
	friend NoiseScene;
	friend NoiseRenderer;

	NoiseGUIManager();

	void		AddToRenderList();

	BOOL	AddButton(NoiseGUIButton* pButton);

	BOOL	AddLabel(NoiseGUILabel* pLabel);

	void		Update();//get info of child objects and update events list

	void		GetMessage(std::vector<NOISE_GUI_EVENTS>& eventsList); // delete checked events

private:
	NoiseScene*						m_pFatherScene;
	NoiseUtInputEngine*		m_pInputEngine;



	NoiseGraphicObject*								m_pChildGraphicObject;
	std::vector<NoiseGUIButton*>*			m_pChildButtonList;
	std::vector<NoiseGUILabel*>*				m_pChildLabelList;
	std::vector<NOISE_GUI_EVENTS>*		m_pEventsList;
};

