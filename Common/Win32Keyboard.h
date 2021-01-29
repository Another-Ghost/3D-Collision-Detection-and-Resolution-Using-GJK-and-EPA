/******************************************************************************
Class:Win32Keyboard
Implements:Keyboard
Author:Rich Davison
Description:TODO

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#pragma once
#ifdef _WIN32
#include "Keyboard.h"
#include "Win32Window.h"

namespace NCL {
	namespace Win32Code {
		class Win32Keyboard : public Keyboard {
		public:
			friend class Win32Window;

		protected:
			Win32Keyboard(HWND &hwnd);
			virtual ~Win32Keyboard(void) {
			}

			virtual void UpdateRAW(RAWINPUT* raw);
			RAWINPUTDEVICE	rid;			//Windows OS hook 
		};
	}
}
#endif //_WIN32