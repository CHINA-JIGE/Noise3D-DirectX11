
/***********************************************************************

					类：NOISE input engine

					简述：键盘鼠标等控制系统


************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::Ut;

UINT Noise3D::Ut::MapDInputScanCodeToAscii(UINT scanCode, bool isCapital)
{
#define returnChar(UCase,LCase) if(isCapital){return UCase;}else {return LCase;} break;

	switch (scanCode)
	{
	case NOISE_KEY_0:	returnChar(')', '0');
	case NOISE_KEY_1:	returnChar('!', '1');
	case NOISE_KEY_2:	returnChar('@', '2');
	case NOISE_KEY_3:	returnChar('#', '3');
	case NOISE_KEY_4:	returnChar('$', '4');
	case NOISE_KEY_5:	returnChar('%', '5');
	case NOISE_KEY_6:	 returnChar('^', '6');
	case NOISE_KEY_7:	returnChar('&', '7');
	case NOISE_KEY_8:	returnChar('*', '8');
	case NOISE_KEY_9:	returnChar('(', '9');
	case NOISE_KEY_A:	returnChar('A', 'a');
	case NOISE_KEY_B:	returnChar('B', 'b');
	case NOISE_KEY_C:	returnChar('C', 'c');
	case NOISE_KEY_D:	returnChar('D', 'd');
	case NOISE_KEY_E:	returnChar('E', 'e');
	case NOISE_KEY_F:	returnChar('F', 'f');
	case NOISE_KEY_G:	returnChar('G', 'g');
	case NOISE_KEY_H:	returnChar('H', 'h');
	case NOISE_KEY_I:	returnChar('I', 'i');
	case NOISE_KEY_J:	returnChar('J', 'j');
	case NOISE_KEY_K:	returnChar('K', 'k');
	case NOISE_KEY_L:	returnChar('L', 'l');
	case NOISE_KEY_M:	returnChar('M', 'm');
	case NOISE_KEY_N:	returnChar('N', 'n');
	case NOISE_KEY_O:	returnChar('O', 'o');
	case NOISE_KEY_P:	returnChar('P', 'p');
	case NOISE_KEY_Q:	returnChar('Q', 'q');
	case NOISE_KEY_R:	returnChar('R', 'r');
	case NOISE_KEY_S:	returnChar('S', 's');
	case NOISE_KEY_T:	returnChar('T', 't');
	case NOISE_KEY_U:	returnChar('U', 'u');
	case NOISE_KEY_V:	returnChar('V', 'v');
	case NOISE_KEY_W:	returnChar('W', 'w');
	case NOISE_KEY_X:	returnChar('X', 'x');
	case NOISE_KEY_Y:	returnChar('Y', 'y');
	case NOISE_KEY_Z:	returnChar('Z', 'z');
	case NOISE_KEY_COMMA:	returnChar('<', ',');
	case NOISE_KEY_PERIOD:		returnChar('>', '.');
	case NOISE_KEY_LBRACKET:	returnChar('{', '[');
	case NOISE_KEY_RBRACKET:	returnChar('}', ']');
	case NOISE_KEY_MINUS:		returnChar('_', '-');
	case NOISE_KEY_EQUALS:	returnChar('+', '=');
	case NOISE_KEY_SEMICOLON: returnChar(':', ';');
	case NOISE_KEY_SLASH: returnChar('?', '/');
	case NOISE_KEY_BACKSLASH: returnChar('|', '\\');
	case NOISE_KEY_APOSTROPHE: returnChar('"', '\'');
	case NOISE_KEY_SPACE: return ' ';break;
	default:return 0;break;
	}
};

IInputEngine::IInputEngine()
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

	mHasBeenInitialized= false;
	mResponsiveHWND=(HWND)0;
};

void IInputEngine::Destroy()
{
	if (m_pDeviceKeyboard)m_pDeviceKeyboard->Unacquire();
	if (m_pDeviceMouse)m_pDeviceMouse->Unacquire();
	ReleaseCOM(m_pDirectInput);
	ReleaseCOM(m_pDeviceKeyboard);
	ReleaseCOM(m_pDeviceMouse);
	mHasBeenInitialized = false;
};

bool IInputEngine::Initialize(HINSTANCE hinstance, HWND hwnd)
{
	//input Engine has been initialized
	if (mHasBeenInitialized)return true;

	//get screen size
	mScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	mScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	mResponsiveHWND = hwnd;
	
	//get cursor position at initial time
	POINT cursorPos;
	NOISE_MACRO_FUNCTION_WINAPI GetCursorPos(&cursorPos);
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
	HR_DEBUG(hr, " NoiseUtInputEngine :Device (Mouse)Set Cooperative Level failed!!");
	hr = m_pDeviceKeyboard->SetCooperativeLevel(hwnd,	DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	HR_DEBUG(hr, " NoiseUtInputEngine :Device (Keyboard)Set Cooperative Level failed!!");

	//acquire control (will be done in update)
	 m_pDeviceMouse->Acquire();
	//HR_DEBUG(hr, " NoiseUtInputEngine :Device (Mouse) Acquire failed");
	 m_pDeviceKeyboard->Acquire();
	//HR_DEBUG(hr, " NoiseUtInputEngine :Device (Keyboard) Acquire failed");*/

	mHasBeenInitialized = true;
	return true;
};

bool IInputEngine::Update()
{
	//input engine must be initialized
	if (!mHasBeenInitialized)return false;

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
			return false;
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
			NOISE_MACRO_FUNCTION_WINAPI GetCursorPos(&cursorPos);
			mMousePosX = cursorPos.x;
			mMousePosY = cursorPos.y;
		}
		else
		{
			return false;
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
	NOISE_MACRO_FUNCTION_WINAPI GetCursorPos(&cursorPos);
	mMousePosX = cursorPos.x;
	mMousePosY = cursorPos.y;

	//boundary restriction
	if (mMousePosX < 0)mMousePosX = 0;
	if (mMousePosY < 0)mMousePosY = 0;
	if (mMousePosX > mScreenWidth)mMousePosX = mScreenWidth;
	if (mMousePosY > mScreenHeight)mMousePosY= mScreenHeight;

#pragma endregion

	return true;
}

bool IInputEngine::IsKeyPressed(NOISE_KEY keyVal)
{
	return IsKeyPressed(UINT(keyVal));
}

bool IInputEngine::IsKeyPressed(UINT keyVal)
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	//according to the doc , only low byte of the dwData  matters, so apply "and" with 0x80
	//in other word, the key was pressed if (val & 0x80) is non zero
	if ((mKeyboardState[keyVal]) & 0x80)
	{
		return true;
	}

	return false;
}

bool IInputEngine::IsMouseButtonPressed(NOISE_MOUSEBUTTON mouseBtn)
{
	//according to Doc :
	//The rgbButtons member is an array of bytes, one for each of four or eight buttons.
	//For a traditional mouse, the first element in the array is generally the left button, 
	//the second is the right button, and the third is the middle button.
	//The high bit is set if the button is down, and it is clear if the button is up or not present.

	switch(mouseBtn)
	{
	case NOISE_MOUSEBUTTON_LEFT:
		if (mMouseState.rgbButtons[0] & 0x80)return true;
		break;
	case NOISE_MOUSEBUTTON_RIGHT:
		if (mMouseState.rgbButtons[1] & 0x80)return true;
		break;
	case NOISE_MOUSEBUTTON_MIDDLE:
		if (mMouseState.rgbButtons[2] & 0x80)return true;
		break;
	}

	return false;
}

bool IInputEngine::IsInitialized()
{
	return mHasBeenInitialized;
}

int IInputEngine::GetMouseDiffX()
{
	return mMouseDiffX;
};

int IInputEngine::GetMouseDiffY()
{
	return mMouseDiffY;
};

int IInputEngine::GetMouseScrollDiff()
{
	return mMouseDiffScroll;
};

int IInputEngine::GetMouseScrPosX()
{
	return mMousePosX;
}

int IInputEngine::GetMouseScrPosY()
{
	return mMousePosY;
}
