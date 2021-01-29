#include "Win32Window.h"
#ifdef _WIN32
#include "Windowsx.h"

using namespace NCL;
using namespace Win32Code;

Win32Window::Win32Window(const std::string& title, int sizeX, int sizeY, bool fullScreen, int offsetX, int offsetY)	{
	forceQuit		= false;
	init			= false;
	mouseLeftWindow	= false;
	lockMouse		= false;
	showMouse		= true;
	active			= true;

	this->fullScreen = fullScreen;

	size = Vector2((float)sizeX, (float)sizeY);

	defaultSize = size;

	fullScreen ? position.x = 0.0f : position.x = (float)offsetX;
	fullScreen ? position.y = 0.0f : position.y = (float)offsetY;

	windowInstance = GetModuleHandle(NULL);

	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	if (!GetClassInfoEx(windowInstance, WINDOWCLASS, &windowClass))	{
		windowClass.cbSize		= sizeof(WNDCLASSEX);
	    windowClass.style		= CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc	= (WNDPROC)WindowProc;
		windowClass.hInstance	= windowInstance;
		windowClass.hCursor		= LoadCursor(NULL, IDC_ARROW);
		windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
		windowClass.lpszClassName = WINDOWCLASS;

		if(!RegisterClassEx(&windowClass)) {
			std::cout << "Window::Window(): Failed to register class!" << std::endl;
			return;
		}
	}

	if(fullScreen) {
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared

		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth		= sizeX;			// Selected Screen Width
		dmScreenSettings.dmPelsHeight		= sizeY;			// Selected Screen Height
		dmScreenSettings.dmBitsPerPel		= 32;				// Selected Bits Per Pixel
		dmScreenSettings.dmDisplayFrequency = 60;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

		if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)	{
			std::cout << "Window::Window(): Failed to switch to fullscreen!" << std::endl;
			return;
		}
	}

	windowHandle = CreateWindowEx(fullScreen ? WS_EX_TOPMOST : NULL,
	WINDOWCLASS,    // name of the window class
	title.c_str(),   // title of the window
	fullScreen ? WS_POPUP|WS_VISIBLE : WS_OVERLAPPEDWINDOW|WS_POPUP|WS_VISIBLE|WS_SYSMENU|WS_MAXIMIZEBOX|WS_MINIMIZEBOX,    // window style
						(int)position.x,	// x-position of the window
                        (int)position.y,	// y-position of the window
                        (int)size.x,		// width of the window
                        (int)size.y,		// height of the window
                        NULL,				// No parent window!
                        NULL,				// No Menus!
						windowInstance,		// application handle
                        NULL);				// No multiple windows!

 	if(!windowHandle) {
		std::cout << "Window::Window(): Failed to create window!" << std::endl;
		return;
	}

	winMouse	= new Win32Mouse(windowHandle);
	winKeyboard = new Win32Keyboard(windowHandle);

	keyboard	= winKeyboard;
	mouse		= winMouse;
	winMouse->SetAbsolutePositionBounds(size);

	winMouse->Wake();
	winKeyboard->Wake();

	LockMouseToWindow(lockMouse);
	ShowOSPointer(showMouse);

	SetConsolePosition(1500, 200);

	windowTitle = title;

	init		= true;
	maximised	= false;
}

Win32Window::~Win32Window(void)	{
	init = false;
}

bool	Win32Window::InternalUpdate() {
	MSG		msg;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(windowHandle, &pt);
	winMouse->SetAbsolutePosition(Vector2((float)pt.x, (float)pt.y));

	while(PeekMessage(&msg,windowHandle,0,0,PM_REMOVE)) {
		CheckMessages(msg); 
	}

	return !forceQuit;
}

void	Win32Window::UpdateTitle()	{
	SetWindowText(windowHandle, windowTitle.c_str());
}

void	Win32Window::SetFullScreen(bool fullScreen) {
	if (fullScreen) {
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared

		DEVMODEA settings;
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &settings);

		size.x = (float)settings.dmPelsWidth;
		size.y = (float)settings.dmPelsHeight;

		dmScreenSettings.dmSize				= sizeof(dmScreenSettings);			// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth		= (DWORD)size.x;		// Selected Screen Width
		dmScreenSettings.dmPelsHeight		= (DWORD)size.y;		// Selected Screen Height
		dmScreenSettings.dmBitsPerPel		= 32;								// Selected Bits Per Pixel
		dmScreenSettings.dmDisplayFrequency = (DWORD)settings.dmDisplayFrequency;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			std::cout << __FUNCTION__ << " Failed to switch to fullscreen!" << std::endl;
		}
		else {
			ResizeRenderer();
		}
	}
	else {
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared

		size = defaultSize;

		dmScreenSettings.dmSize = sizeof(dmScreenSettings);	// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth  = (DWORD)size.x;		// Selected Screen Width
		dmScreenSettings.dmPelsHeight = (DWORD)size.y;		// Selected Screen Height
		dmScreenSettings.dmPosition.x = (DWORD)position.x;
		dmScreenSettings.dmPosition.y = (DWORD)position.y;
		dmScreenSettings.dmBitsPerPel = 32;					// Selected Bits Per Pixel
		dmScreenSettings.dmDisplayFrequency = 60;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY | DM_POSITION;

		if (ChangeDisplaySettings(&dmScreenSettings, 0) != DISP_CHANGE_SUCCESSFUL) {
			std::cout << __FUNCTION__ << " Failed to switch out of fullscreen!" << std::endl;
		}
	}
}

void Win32Window::CheckMessages(MSG &msg)	{
	Win32Window* thisWindow = (Win32Window*)window;
	switch (msg.message)	{				// Is There A Message Waiting?
		case (WM_QUIT):
		case (WM_CLOSE): {					// Have We Received A Quit Message?
			thisWindow->ShowOSPointer(true);
			thisWindow->LockMouseToWindow(false);
			forceQuit = true;
		}break;
		case (WM_INPUT): {
			UINT dwSize;
			GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, NULL, &dwSize,sizeof(RAWINPUTHEADER));

			BYTE* lpb = new BYTE[dwSize];
	
			GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, lpb, &dwSize,sizeof(RAWINPUTHEADER));
			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (keyboard && raw->header.dwType == RIM_TYPEKEYBOARD && active) {
				thisWindow->winKeyboard->UpdateRAW(raw);
			}

			if (mouse && raw->header.dwType == RIM_TYPEMOUSE && active) {			
				thisWindow->winMouse->UpdateRAW(raw);
			}

			delete lpb;
		}break;

		default: {								// If Not, Deal With Window Messages
			TranslateMessage(&msg);				// Translate The Message
			DispatchMessage(&msg);				// Dispatch The Message
		}
	}
}

LRESULT CALLBACK Win32Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)	{
	Win32Window* thisWindow = (Win32Window*)window;

	bool applyResize = false;

    switch(message)	 {
        case(WM_DESTROY):	{
			thisWindow->ShowOSPointer(true);
			thisWindow->LockMouseToWindow(false);

			PostQuitMessage(0);
			thisWindow->forceQuit = true;
		} break;
		case (WM_ACTIVATE): {
			if(LOWORD(wParam) == WA_INACTIVE)	{
				thisWindow->active = false;
				ReleaseCapture();
				ClipCursor(NULL);
				if (thisWindow->init && mouse && keyboard) {
					thisWindow->winMouse->Sleep();
					thisWindow->winKeyboard->Sleep();
				}
			}
			else{
				thisWindow->active = true;
				if(thisWindow->init) {
					thisWindow->winMouse->Wake();
					thisWindow->winKeyboard->Wake();

					if(thisWindow->lockMouse) {
						thisWindow->LockMouseToWindow(true);
					}
				}
			}
			return 0;
		}break;
		case (WM_LBUTTONDOWN): {
			if(thisWindow->init && thisWindow->lockMouse) {
				thisWindow->LockMouseToWindow(true);
			}
		}break;
		case (WM_MOUSEMOVE): {
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = thisWindow->windowHandle;
			TrackMouseEvent(&tme);

			if (mouse) {
				Win32Mouse*realMouse = (Win32Mouse*)mouse;
				realMouse->UpdateWindowPosition(
					Vector2((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam))
				);
			}

			if(thisWindow->mouseLeftWindow) {
				thisWindow->mouseLeftWindow = false;
				if (thisWindow->init) {
					thisWindow->winMouse->Wake();
					thisWindow->winKeyboard->Wake();
				}
			}
		}break;
		case(WM_MOUSELEAVE):{
			thisWindow->mouseLeftWindow = true;
			if (thisWindow->init) {
				thisWindow->winMouse->Sleep();
				thisWindow->winKeyboard->Sleep();
			}
		}break;
		case(WM_SIZE): {
			float newX = (float)LOWORD(lParam);
			float newY = (float)HIWORD(lParam);
			if (newX > 0 && newY > 0 && (newX != thisWindow->size.x || newY != thisWindow->size.y)) {
				thisWindow->size.x = (float)LOWORD(lParam);
				thisWindow->size.y = (float)HIWORD(lParam);
			}
			if (wParam == SIZE_MAXIMIZED) {
				thisWindow->maximised = true;
				applyResize = true;
			}
			else if (wParam == SIZE_RESTORED && thisWindow->maximised) {
				thisWindow->maximised = false;
				applyResize = true;
			}
		}break;
		case(WM_ENTERSIZEMOVE): {
		}break;
		case(WM_EXITSIZEMOVE): {
			applyResize = true;
		}break;
    }

	if (applyResize) {
		thisWindow->ResizeRenderer();
		if (thisWindow->init) {
			thisWindow->winMouse->SetAbsolutePositionBounds(thisWindow->size);
			thisWindow->LockMouseToWindow(thisWindow->lockMouse);
		}
	}

    return DefWindowProc (hWnd, message, wParam, lParam);
}

void	Win32Window::LockMouseToWindow(bool lock)	{
	lockMouse = lock;
	if(lock) {
		RECT		windowRect;
		GetWindowRect (windowHandle, &windowRect);

		//windowRect.top -= 1; //adjust until something happens?!

		SetCapture(windowHandle);
		ClipCursor(&windowRect);
	}
	else{
		ReleaseCapture();
		ClipCursor(NULL);
	}
}

void	Win32Window::ShowOSPointer(bool show)	{
	if(show == showMouse) {
		return;	//ShowCursor does weird things, due to being a counter internally...
	}

	showMouse = show;
	if(show) {
		ShowCursor(1);
	}
	else{
		ShowCursor(0);
	}
}

void	Win32Window::SetConsolePosition(int x, int y)	{
	HWND consoleWindow = GetConsoleWindow();

	SetWindowPos(consoleWindow, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	SetActiveWindow(windowHandle);
}

void	Win32Window::SetWindowPosition(int x, int y) {
	SetWindowPos(windowHandle, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	SetActiveWindow(windowHandle);
}

void	Win32Window::ShowConsole(bool state)				{
	HWND consoleWindow = GetConsoleWindow();

	ShowWindow(consoleWindow, state ? SW_RESTORE : SW_HIDE);

	SetActiveWindow(windowHandle);
}

#endif //_WIN32