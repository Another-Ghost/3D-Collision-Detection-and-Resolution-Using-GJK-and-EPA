#include "Win32Keyboard.h"
#ifdef _WIN32

using namespace NCL;
using namespace Win32Code;

Win32Keyboard::Win32Keyboard(HWND &hwnd)	{
	//Tedious windows RAW input stuff
	rid.usUsagePage		= HID_USAGE_PAGE_GENERIC;		//The keyboard isn't anything fancy
    rid.usUsage			= HID_USAGE_GENERIC_KEYBOARD;	//but it's definitely a keyboard!
    rid.dwFlags			= RIDEV_INPUTSINK;				//Yes, we want to always receive RAW input...
    rid.hwndTarget		= hwnd;							//Windows OS window handle
    RegisterRawInputDevices(&rid, 1, sizeof(rid));		//We just want one keyboard, please!

	isAwake = true;
}

/*
Updates the keyboard state with data received from the OS.
*/
void Win32Keyboard::UpdateRAW(RAWINPUT* raw)	{
	if(isAwake)	{
		DWORD key = (DWORD)raw->data.keyboard.VKey;

		//We should do bounds checking!
		if(key < 0 || key >(int)KeyboardKeys::MAXVALUE)	{
			return;
		}

		//First bit of the flags tag determines whether the key is down or up
		keyStates[(int)key] = !(raw->data.keyboard.Flags & RI_KEY_BREAK);
	}
}

#endif //_WIN32