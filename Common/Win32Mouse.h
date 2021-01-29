/******************************************************************************
Class:Win32Mouse
Implements:Mouse
Author:Rich Davison
Description:TODO

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#pragma once
#ifdef _WIN32
#include "Mouse.h"
#include "Win32Window.h"

namespace NCL {
	namespace Win32Code {
		class Win32Mouse : public NCL::Mouse {
		public:
			friend class Win32Window;

		protected:
			Win32Mouse(HWND &hwnd);
			virtual ~Win32Mouse(void) {}

			void UpdateWindowPosition(const Vector2& newPos) {
				windowPosition = newPos;
			}

			virtual void	UpdateRAW(RAWINPUT* raw);
			RAWINPUTDEVICE	rid;			//Windows OS hook 

			bool		setAbsolute;
		};
	}
}
#endif //_WIN32