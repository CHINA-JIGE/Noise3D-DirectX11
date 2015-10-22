
/***********************************************************************

					类：NOISE input engine

					简述：键盘鼠标等控制系统


************************************************************************/

#include "Noise3D.h"

NoiseUtInputEngine::NoiseUtInputEngine()
{
	m_pDirectInput=nullptr;
	m_pDeviceKeyboard = nullptr;
	m_pDeviceMouse = nullptr;
	mMousePosX = 0;
	mMousePosY=0;
	mScreenWidth=0;
	mScreenHeight=0;
	mWindowWidth = 0;
	mWindowHeight = 0;

	mHasBeenInitialized=FALSE;
	mResponsiveHWND=(HWND)0;
};

void NoiseUtInputEngine::SelfDestruction()
{
	if (m_pDeviceKeyboard)m_pDeviceKeyboard->Unacquire();
	if (m_pDeviceMouse)m_pDeviceMouse->Unacquire();
	ReleaseCOM(m_pDirectInput);
	ReleaseCOM(m_pDeviceKeyboard);
	ReleaseCOM(m_pDeviceMouse);
	mHasBeenInitialized = FALSE;
};

BOOL NoiseUtInputEngine::Initialize(HINSTANCE hinstance, HWND hwnd)
{
	//input Engine has been initialized
	if (mHasBeenInitialized)return TRUE;

	//get screen size
	mScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	mScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	mResponsiveHWND = hwnd;
	
	//get cursor position at initial time
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	mMousePosX = (int)cursorPos.x;
	mMousePosY = (int)cursorPos.y;

	//create main DInput Interface
	HRESULT hr = S_OK;
	hr = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);
	HR_DEBUG(hr, "NoiseUtInputEngine :Create DirectInput8 : Failed");

	//then it's turn to Devices
	hr = m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pDeviceMouse, NULL);
	HR_DEBUG(hr, "NoiseUtInputEngine :Create DINPUT Device (Mouse) failed!!");
	hr = m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pDeviceKeyboard, NULL);
	HR_DEBUG(hr, "NoiseUtInputEngine :Create DINPUT Device (KeyBoard) failed!!");

	//set device data format ( well we can create our customized data format actually)
	hr = m_pDeviceMouse->SetDataFormat(&c_dfDIMouse);
	HR_DEBUG(hr, " NoiseUtInputEngine :Device (Mouse) Set Data Format failed!!");
	hr = m_pDeviceKeyboard->SetDataFormat(&c_dfDIKeyboard);
	HR_DEBUG(hr, " NoiseUtInputEngine :Device (Keyboard) Set Data Format failed!!");


	//set cooperative level ( how to share with other program)
	hr = m_pDeviceMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	HR_DEBUG(hr, " NoiseUtInputEngine :Device (Mouse)Set Cooperative Format failed!!");
	hr = m_pDeviceKeyboard->SetCooperativeLevel(hwnd,	DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	HR_DEBUG(hr, " NoiseUtInputEngine :Device (Keyboard)Set Cooperative Format failed!!");

	//acquire control (will be done in update)
	 m_pDeviceMouse->Acquire();
	//HR_DEBUG(hr, " NoiseUtInputEngine :Device (Mouse) Acquire failed");
	 m_pDeviceKeyboard->Acquire();
	//HR_DEBUG(hr, " NoiseUtInputEngine :Device (Keyboard) Acquire failed");*/

	mHasBeenInitialized = TRUE;
	return TRUE;
};

BOOL NoiseUtInputEngine::Update()
{
	//input engine must be initialized
	if (!mHasBeenInitialized)return FALSE;

	//..................
	HRESULT hr;
	//keyboard get device state (validate , and get buffered keyboard event data to an array)
	//m_pDeviceKeyboard->Poll();
	hr = m_pDeviceKeyboard->GetDeviceState(sizeof(mKeyboardState), (void*)&mKeyboardState);
	if (FAILED(hr))
	{
		// If the keyboard lost focus or was not acquired , then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			//acquire control
			m_pDeviceKeyboard->Acquire();
		}
		else
		{
			return FALSE;
		}
	}

	//mouse get device state(validate) read info like absolute movement of mouse
	//m_pDeviceMouse->Poll();
	hr = m_pDeviceMouse->GetDeviceState(sizeof(DIMOUSESTATE), (void*)&mMouseState);
	if (FAILED(hr))
	{
		//what if mouse device has been lost ,mouse state must have been invalid
		//so clear it
		ZeroMemory(&mMouseState, sizeof(mMouseState));

		// If the mouse lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pDeviceMouse->Acquire();

			//keep updating initial mouse pixel position
			POINT cursorPos;
			GetCursorPos(&cursorPos);
			mMousePosX = cursorPos.x;
			mMousePosY = cursorPos.y;
		}
		else
		{
			return FALSE;
		}
	}

#pragma region NewStateProcess

	//By default, mouse axes are reported as relative coordinates 
	//and joystick axes as absolute coordinates.
	//You can change the coordinate system for a device by setting a property.
	//For more information, see Device Properties.
	mMouseDiffX = mMouseState.lX;
	mMouseDiffY = mMouseState.lY;
	mMouseDiffScroll = mMouseState.lZ;

	//process newly got device state
	//and in relative mode ( default ), lX / lY are relative coordinates.
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	mMousePosX = cursorPos.x;
	mMousePosY = cursorPos.y;

	//boundary restriction
	if (mMousePosX < 0)mMousePosX = 0;
	if (mMousePosY < 0)mMousePosY = 0;
	if (mMousePosX > mScreenWidth)mMousePosX = mScreenWidth;
	if (mMousePosY > mScreenHeight)mMousePosY= mScreenHeight;

#pragma endregion

	return TRUE;
}

BOOL NoiseUtInputEngine::IsKeyPressed(NOISE_KEY keyVal)
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	//according to the doc , only low byte of the dwData  matters, so apply "and" with 0x80
	//in other word, the key was pressed if (val & 0x80) is non zero
	if ((mKeyboardState[keyVal]) & 0x80)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL NoiseUtInputEngine::IsMouseButtonPressed(NOISE_MOUSEBUTTON mouseBtn)
{
	//according to Doc :
	//The rgbButtons member is an array of bytes, one for each of four or eight buttons.
	//For a traditional mouse, the first element in the array is generally the left button, 
	//the second is the right button, and the third is the middle button.
	//The high bit is set if the button is down, and it is clear if the button is up or not present.

	switch(mouseBtn)
	{
	case NOISE_MOUSEBUTTON_LEFT:
		if (mMouseState.rgbButtons[0] & 0x80)return TRUE;
		break;
	case NOISE_MOUSEBUTTON_RIGHT:
		if (mMouseState.rgbButtons[1] & 0x80)return TRUE;
		break;
	case NOISE_MOUSEBUTTON_MIDDLE:
		if (mMouseState.rgbButtons[2] & 0x80)return TRUE;
		break;
	}

	return FALSE;
}

BOOL NoiseUtInputEngine::IsInitialized()
{
	return mHasBeenInitialized;
}

int NoiseUtInputEngine::GetMouseDiffX()
{
	return mMouseDiffX;
};

int NoiseUtInputEngine::GetMouseDiffY()
{
	return mMouseDiffY;
};

int NoiseUtInputEngine::GetMouseScrollDiff()
{
	return mMouseDiffScroll;
};

int NoiseUtInputEngine::GetMouseScrPosX()
{
	return mMousePosX;
}

int NoiseUtInputEngine::GetMouseScrPosY()
{
	return mMousePosY;
}
