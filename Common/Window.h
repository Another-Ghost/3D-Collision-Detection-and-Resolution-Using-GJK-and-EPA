/******************************************************************************
Class:Window
Implements:
Author:Rich Davison
Description:TODO

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "GameTimer.h"

#include "Vector2.h"

#include <string>

namespace NCL {
	namespace Rendering {
		class RendererBase;
	};
	using namespace Rendering;
	
	class Window {
	public:
		static Window* CreateGameWindow(std::string title = "NCLGL!", int sizeX = 800, int sizeY = 600, bool fullScreen = false, int offsetX = 100, int offsetY = 100);

		static void DestroyGameWindow() {
			delete window;
			window = nullptr;
		}

		bool		UpdateWindow();
		void		SetRenderer(RendererBase* r);

		bool		HasInitialised()	const { return init; }

		float		GetScreenAspect()	const {
			return size.x / size.y;
		}

		Vector2		GetScreenSize()		const { return size; }
		Vector2		GetScreenPosition()	const { return position; }

		const std::string&  GetTitle()   const { return windowTitle; }
		void				SetTitle(const std::string& title) {
			windowTitle = title;
			UpdateTitle();
		};

		virtual void	LockMouseToWindow(bool lock) = 0;
		virtual void	ShowOSPointer(bool show) = 0;

		virtual void	SetWindowPosition(int x, int y) {};
		virtual void	SetFullScreen(bool state) {};
		virtual void	SetConsolePosition(int x, int y) {};
		virtual void	ShowConsole(bool state) {};

		static const Keyboard*	 GetKeyboard() { return keyboard; }
		static const Mouse*		 GetMouse() { return mouse; }
		static const GameTimer*	 GetTimer() { return timer; }

		static Window*	const GetWindow() { return window; }
	protected:
		Window();
		virtual ~Window();

		virtual void UpdateTitle() {}

		virtual bool InternalUpdate() = 0;

		void ResizeRenderer();

		RendererBase*	renderer;

		bool				init;
		Vector2				position;
		Vector2				size;
		Vector2				defaultSize;

		std::string			windowTitle;

		static Window*		window;
		static Keyboard*	keyboard;
		static Mouse*		mouse;
		static GameTimer*	timer;
	};
}
