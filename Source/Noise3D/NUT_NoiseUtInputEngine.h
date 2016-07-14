/***********************************************************************

						h£ºNoiseInputEngine

************************************************************************/

#pragma once

#include "NUT_NoiseUtInputEngine_KeyDef.h"

namespace Noise3D
{
	namespace Ut
	{
		class /*_declspec(dllexport)*/ IInputEngine 
		{
		public:

			//¹¹Ôìº¯Êý
			IInputEngine();

			BOOL		Initialize(HINSTANCE hinstance, HWND hwnd);

			BOOL		Update();

			BOOL		IsKeyPressed(NOISE_KEY keyVal);

			BOOL		IsKeyPressed(UINT keyVal);

			BOOL		IsMouseButtonPressed(NOISE_MOUSEBUTTON mouseBtn);

			BOOL		IsInitialized();

			int			GetMouseDiffX();

			int			GetMouseDiffY();

			int			GetMouseScrollDiff();

			int			GetMouseScrPosX();

			int			GetMouseScrPosY();

		private:

			void			Destroy();

			IDirectInput8*				m_pDirectInput;
			IDirectInputDevice8*		m_pDeviceKeyboard;
			IDirectInputDevice8*		m_pDeviceMouse;

			BOOL							mHasBeenInitialized;
			HWND							mResponsiveHWND;

			unsigned char mKeyboardState[256];
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