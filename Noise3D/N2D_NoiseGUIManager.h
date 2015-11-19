
/***********************************************************************

									h£ºNoiseGUI

************************************************************************/
#pragma once

class NoiseUtInputEngine;

//the main GUI management class
public class _declspec(dllexport) NoiseGUIManager
{
public:
	friend class NoiseScene;
	friend class NoiseRenderer;

	NoiseGUIManager();

	void		SelfDestruction();

	BOOL	AddChildObjectToRenderList();//Renderer need to be valid

	BOOL	AddButton(NoiseGUIButton& refButton);

	BOOL	AddText(NoiseGUIText& refText);

	void		SetInputEngine(NoiseUtInputEngine& refInputE);

	void		Update();//get info of child objects and update events list

	void		SetWindowHWND(HWND hWnd);


private:

	void		mFunction_UpdateButtons();//including graphic objs / events

private:
	NoiseScene*					m_pFatherScene;
	NoiseUtInputEngine*	m_pInputEngine;
	std::vector<NoiseGUIButton*>*			m_pChildButtonList;
	std::vector<NoiseGUIText*>*				m_pChildTextList;
	std::vector<NoiseGUIScrollBar*>*			m_pChildScrollBarList;
	HWND							mWindowHWND;
};


