
/***********************************************************************

									h£ºNoiseGUI

************************************************************************/
#pragma once

//the main GUI management class
class _declspec(dllexport) NoiseGUIManager:public NoiseClassLifeCycle
{
public:

	NoiseGUIManager();
	~NoiseGUIManager();

	//cancel AddChildObjectToRenderList and let user decide the render order
	//(and avoid repetitve render,like internal button)

	BOOL	Initialize(UINT backBufferWidth,UINT backBufferHeight,NoiseRenderer& refRenderer,NoiseUtInputEngine& refInputE,NoiseFontManager& refFontMgr, HWND hwnd);

	BOOL	InitButton(NoiseGUIButton& refObject);

	BOOL	InitTextBox(NoiseGUITextBox& refObject,UINT fontID);

	BOOL	InitScrollBar(NoiseGUIScrollBar& refObject);

	void		SetInputEngine(NoiseUtInputEngine& refInputE);

	void		SetFontManager(NoiseFontManager& refFontMgr);

	void		SetRenderer(NoiseRenderer& refRenderer);

	void		Update();//get info of child objects and update events list

	void		SetWindowHWND(HWND hWnd);



	void		AddObjectToRenderList(NoiseGUIScrollBar& obj);

	void		AddObjectToRenderList(NoiseGUIButton & obj);

	void		AddObjectToRenderList(NoiseGUITextBox& obj);


	//void		AddObjectsToRenderList();

private:

	void		Destroy();

	void		mFunction_UpdateInputEngineInfo();

	void		mFunction_ComputeMouseDepth();//to implement overlapping

	void		mFunction_UpdateButtons();//including graphic objs / events

	void		mFunction_UpdateScrollBars();//embed buttons

	void		mFunction_UpdateTextBoxs();

	void		mFunction_UpdateAllInternalGraphicObjs();//update graphic objs in every component


private:
	NoiseUtInputEngine*	m_pInputEngine;
	NoiseFontManager*		m_pFontMgr;//need to be set
	NoiseRenderer*				m_pRenderer;

	std::vector<NoiseGUIButton*>*			m_pChildButtonList;
	std::vector<NoiseGUITextBox*>*			m_pChildTextList;
	std::vector<NoiseGUIScrollBar*>*			m_pChildScrollBarList;
	UINT			mCurrentTimeStamp;
	HWND		mWindowHWND;
	UINT			mBackBufferWidth;
	UINT			mBackBufferHeight;
};


