#include "Win32Mouse.h"
#ifdef _WIN32

using namespace NCL;
using namespace Win32Code;

Win32Mouse::Win32Mouse(HWND &hwnd)	{
	rid.usUsagePage = HID_USAGE_PAGE_GENERIC; 
    rid.usUsage		= HID_USAGE_GENERIC_MOUSE; 
    rid.dwFlags		= RIDEV_INPUTSINK;   
    rid.hwndTarget	= hwnd;
    RegisterRawInputDevices(&rid, 1, sizeof(rid));

	setAbsolute = false;
}

void Win32Mouse::UpdateRAW(RAWINPUT* raw)	{
	if(isAwake)	{
		bool virtualDesktop = (raw->data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP) > 0;
		bool isAbsolute		= (raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) > 0;

		if (isAbsolute) {
			const int screenWidth  = GetSystemMetrics(virtualDesktop ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);
			const int screenHeight = GetSystemMetrics(virtualDesktop ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);

			Vector2 prevAbsolute = absolutePosition;
			absolutePosition.x = (raw->data.mouse.lLastX / (float)USHRT_MAX) * screenWidth;
			absolutePosition.y = (raw->data.mouse.lLastY / (float)USHRT_MAX) * screenHeight;

			if (setAbsolute) {
				relativePosition.x = (absolutePosition.x - prevAbsolute.x) * sensitivity;
				relativePosition.y = (absolutePosition.y - prevAbsolute.y) * sensitivity;
			}
			setAbsolute = true;
		}
		else {
			/*
			Update the absolute and relative mouse movements
			*/
			relativePosition.x += ((float)raw->data.mouse.lLastX) * sensitivity;
			relativePosition.y += ((float)raw->data.mouse.lLastY) * sensitivity;

			absolutePosition.x += (float)raw->data.mouse.lLastX;
			absolutePosition.y += (float)raw->data.mouse.lLastY;

			/*
			Bounds check the absolute position of the mouse, so it doesn't disappear off screen edges...
			*/
			absolutePosition.x = std::max(absolutePosition.x, 0.0f);
			absolutePosition.x = std::min(absolutePosition.x, absolutePositionBounds.x);

			absolutePosition.y = std::max(absolutePosition.y, 0.0f);
			absolutePosition.y = std::min(absolutePosition.y, absolutePositionBounds.y);
		}

		/*
		TODO: How framerate independent is this?
		*/
		if(raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL)	{		
			if(raw->data.mouse.usButtonData == 120) {
				frameWheel = 1;
			}
			else {
				frameWheel = -1;
			}
		}

		/*
		Oh, Microsoft...
		*/
		static int buttondowns [5] = {	RI_MOUSE_BUTTON_1_DOWN,
										RI_MOUSE_BUTTON_2_DOWN,
										RI_MOUSE_BUTTON_3_DOWN,
										RI_MOUSE_BUTTON_4_DOWN,
										RI_MOUSE_BUTTON_5_DOWN};

		static int buttonps [5] = {		RI_MOUSE_BUTTON_1_UP,
										RI_MOUSE_BUTTON_2_UP,
										RI_MOUSE_BUTTON_3_UP,
										RI_MOUSE_BUTTON_4_UP,
										RI_MOUSE_BUTTON_5_UP};
		
		for(int i = 0; i < 5; ++i) {
			if(raw->data.mouse.usButtonFlags & buttondowns[i])	{
				//The button was pressed!
				buttons[i] = true;

				/*
				If it wasn't too long ago since we last clicked, we trigger a double click!
				*/
				if(lastClickTime[i] > 0) {
					doubleClicks[i] = true;
				}

				/*
				No matter whether the mouse was double clicked or not, we reset the clicklimit
				*/
				lastClickTime[i] = clickLimit;
			}
			else if(raw->data.mouse.usButtonFlags & buttonps[i])	{
				//The button has been released!
				buttons[i]		= false;
				holdButtons[i]	= false;
			}
		}
	}
}
#endif //_WIN32