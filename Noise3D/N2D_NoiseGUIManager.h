
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

	BOOL	AddText(NoiseGUIText* pText);

	void		SetInputEngine(NoiseUtInputEngine* pInputE);

	void		Update();//get info of child objects and update events list

	void		SetWindowHWND(HWND hWnd);


private:

	void		mFunction_UpdateButtons();//including graphic objs / events

	void		mFunction_UpdatePictures();

private:
	NoiseScene*					m_pFatherScene;
	NoiseUtInputEngine*	m_pInputEngine;
	NoiseGraphicObject*								m_pChildGraphicObject;
	std::vector<NoiseGUIButton*>*			m_pChildButtonList;
	std::vector<NoiseGUIText*>*				m_pChildTextList;
	HWND							mWindowHWND;
};


