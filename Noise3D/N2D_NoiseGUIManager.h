
/***********************************************************************

									h£ºNoiseGUI

************************************************************************/
#pragma once

class NoiseUtInputEngine;

//the main GUI management class
class _declspec(dllexport) NoiseGUIManager:public NoiseClassLifeCycle
{
public:
	friend class NoiseScene;
	friend class NoiseRenderer;

	NoiseGUIManager();

	//cancel AddChildObjectToRenderList and let user decide the render order
	//(and avoid repetitve render,like internal button)

	BOOL	CreateButton(NoiseGUIButton& refObject);

	BOOL	CreateTextBox(NoiseGUITextBox& refObject);

	BOOL	CreateScrollBar(NoiseGUIScrollBar& refObject);

	void		SetInputEngine(NoiseUtInputEngine& refInputE);

	void		Update();//get info of child objects and update events list

	void		SetWindowHWND(HWND hWnd);


private:

	void	Destroy();

	void		mFunction_UpdateButtons();//including graphic objs / events

	void		mFunction_UpdateScrollBars();//embed buttons

	void		mFunction_UpdateAllInternalGraphicObjs();//update graphic objs in every component

private:
	NoiseScene*					m_pFatherScene;
	NoiseUtInputEngine*	m_pInputEngine;
	std::vector<NoiseGUIButton*>*			m_pChildButtonList;
	std::vector<NoiseGUITextBox*>*				m_pChildTextList;
	std::vector<NoiseGUIScrollBar*>*			m_pChildScrollBarList;
	HWND							mWindowHWND;
};


