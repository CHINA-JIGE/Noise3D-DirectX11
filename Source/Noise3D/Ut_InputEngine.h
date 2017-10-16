/***********************************************************************

						h£ºNoiseInputEngine

************************************************************************/

#pragma once

#include "Ut_InputEngine_KeyDef.h"

namespace Noise3D
{

	namespace Ut
	{
		class /*_declspec(dllexport)*/ IInputEngine
		{
		public:

			//¹¹Ôìº¯Êý
			IInputEngine();

			bool		Initialize(HINSTANCE hinstance, HWND hwnd);

			bool		Update();

			bool		IsKeyPressed(NOISE_KEY keyVal);

			bool		IsKeyPressed(UINT keyVal);

			bool		IsMouseButtonPressed(NOISE_MOUSEBUTTON mouseBtn);

			bool		IsInitialized();

			int			GetMouseDiffX();

			int			GetMouseDiffY();

			int			GetMouseScrollDiff();

			int			GetMouseScrPosX();

			int			GetMouseScrPosY();

		private:

			void			Destroy();

			IDirectInput8*				m_pDirectInput;

			IDirectInputDevice8*	m_pDeviceKeyboard;

			IDirectInputDevice8*	m_pDeviceMouse;

			bool							mHasBeenInitialized;

			HWND							mResponsiveHWND;

			unsigned char		mKeyboardState[256];

			DIMOUSESTATE mMouseState;

			int		mMousePosX;//with respect to screen

			int		mMousePosY;

			int		mMouseDiffX;//difference between this time and last time

			int		mMouseDiffY;

			int		mMouseDiffScroll;

			int		mScreenWidth;

			int		mScreenHeight;

			int		mWindowWidth;

			int		mWindowHeight;

		};
	}
};