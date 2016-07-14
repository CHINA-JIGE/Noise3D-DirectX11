
/***********************************************************************

									h£ºNoiseGUI

************************************************************************/
#pragma once

namespace Noise3D
{
	namespace GUI
	{
		//the main GUI management class
		class /*_declspec(dllexport)*/ IComponentManager
		{
		public:

			IComponentManager();
			~IComponentManager();

			//cancel AddChildObjectToRenderList and let user decide the render order
			//(and avoid repetitve render,like internal button)

			BOOL	Initialize(UINT backBufferWidth, UINT backBufferHeight, IRenderer& refRenderer, Ut::IInputEngine& refInputE, IFontManager& refFontMgr, HWND hwnd);

			BOOL	InitButton(IButton& refObject);

			BOOL	InitTextBox(ITextBox& refObject, UINT fontID);

			BOOL	InitScrollBar(IScrollBar& refObject);

			BOOL	InitOscilloscope(IOscilloscope& refObject);

			void		SetInputEngine(Ut::IInputEngine& refInputE);

			void		SetFontManager(IFontManager& refFontMgr);

			void		SetRenderer(IRenderer& refRenderer);

			void		Update();//get info of child objects and update events list

			void		SetWindowHWND(HWND hWnd);



			void		AddObjectToRenderList(IScrollBar& obj);

			void		AddObjectToRenderList(IButton & obj);

			void		AddObjectToRenderList(ITextBox& obj);

			void		AddObjectToRenderList(IOscilloscope& obj);
			//void		AddObjectsToRenderList();

		private:

			void		Destroy();

			void		mFunction_UpdateInputEngineInfo();

			void		mFunction_ComputeMouseDepth();//to implement overlapping

			void		mFunction_UpdateButtons();//including graphic objs / events

			void		mFunction_UpdateScrollBars();//embed buttons

			void		mFunction_UpdateTextBoxs();

			void		mFunction_UpdateOscilloscope();

			void		mFunction_UpdateAllInternalGraphicObjs();//update graphic objs in every component


		private:
			Ut::IInputEngine*		m_pInputEngine;
			IFontManager*				m_pFontMgr;//need to be set
			Noise3D::IRenderer*				m_pRenderer;

			std::vector<IButton*>*			m_pChildButtonList;
			std::vector<ITextBox*>*			m_pChildTextList;
			std::vector<IScrollBar*>*			m_pChildScrollBarList;
			std::vector<IOscilloscope*>*	m_pChildOscilloscope;
			UINT			mCurrentTimeStamp;
			HWND		mWindowHWND;
			UINT			mBackBufferWidth;
			UINT			mBackBufferHeight;
		};
	}
}