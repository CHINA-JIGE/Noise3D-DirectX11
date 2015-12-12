
/***********************************************************************

									h£ºNoiseGUI

************************************************************************/
#pragma once

class NoiseUtTimer;
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

	BOOL	Initialize(NoiseUtInputEngine& refInputE,NoiseFontManager& refFontMgr, HWND hwnd);

	BOOL	InitButton(NoiseGUIButton& refObject);

	BOOL	InitTextBox(NoiseGUITextBox& refObject,UINT fontID);

	BOOL	InitScrollBar(NoiseGUIScrollBar& refObject);

	void		SetInputEngine(NoiseUtInputEngine& refInputE);

	void		SetFontManager(NoiseFontManager& refFontMgr);

	void		Update();//get info of child objects and update events list

	void		SetWindowHWND(HWND hWnd);


private:

	void	Destroy();

	void		mFunction_CallMsgProcFunctionAndUpdateEventList(std::vector<UINT>& eventList,NoiseGUIEventCommonOperation* eventOpClass);

	void		mFunction_UpdateButtons();//including graphic objs / events

	void		mFunction_UpdateScrollBars();//embed buttons

	void		mFunction_UpdateTextBoxs();

	void		mFunction_UpdateAllInternalGraphicObjs();//update graphic objs in every component

private:
	NoiseUtInputEngine*	m_pInputEngine;
	NoiseFontManager*		m_pFontMgr;//need to be set
	NoiseUtTimer*				m_pTimer;//TextBox or
	std::vector<NoiseGUIButton*>*			m_pChildButtonList;
	std::vector<NoiseGUITextBox*>*			m_pChildTextList;
	std::vector<NoiseGUIScrollBar*>*			m_pChildScrollBarList;
	HWND							mWindowHWND;
};


