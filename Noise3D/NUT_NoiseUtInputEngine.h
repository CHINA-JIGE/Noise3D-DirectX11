/***********************************************************************

						h£ºNoiseInputEngine

************************************************************************/

#pragma once

#include "NUT_NoiseUtInputEngine_KeyDef.h"

public class _declspec(dllexport) NoiseUtInputEngine
{
public:

	//¹¹Ôìº¯Êý
	NoiseUtInputEngine();

	void			SelfDestruction();

	BOOL		Initialize(HINSTANCE hinstance, HWND hwnd);

	BOOL		Update();

	BOOL		IsKeyPressed(NOISE_KEY keyVal);

	BOOL		IsMouseButtonPressed(NOISE_MOUSEBUTTON mouseBtn);

	int			GetMouseDiffX();

	int			GetMouseDiffY();

	int			GetMouseScrollDiff();

	int			GetMouseScrPosX();

	int			GetMouseScrPosY();

private:

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