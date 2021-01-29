#include "Window.h"
#include <thread>

#ifdef _WIN32
#include "Win32Window.h"
#endif

#ifdef __ORBIS__
#include "../Plugins/PlayStation4/PS4Window.h"
#endif

#include "RendererBase.h"

using namespace NCL;
using namespace Rendering;

Window*		Window::window		= nullptr;
Keyboard*	Window::keyboard	= nullptr;
Mouse*		Window::mouse		= nullptr;
GameTimer*	Window::timer		= nullptr;

Window::Window()	{
	renderer	= nullptr;
	window		= this;
	timer		= new GameTimer();
}

Window::~Window()	{
	delete keyboard;keyboard= nullptr;
	delete mouse;	mouse	= nullptr;
	delete timer;	timer	= nullptr;
	window = nullptr;
	delete timer;
}

Window* Window::CreateGameWindow(std::string title, int sizeX, int sizeY, bool fullScreen, int offsetX, int offsetY) {
	if (window) {
		return nullptr;
	}
#ifdef _WIN32
	return new Win32Code::Win32Window(title, sizeX, sizeY, fullScreen, offsetX, offsetY);
#endif
#ifdef __ORBIS__
	return new PS4::PS4Window(title, sizeX, sizeY, fullScreen, offsetX, offsetY);
#endif
}

void	Window::SetRenderer(RendererBase* r) {
	if (renderer && renderer != r) {
		renderer->OnWindowDetach();
	}
	renderer = r;

	if (r) {
		renderer->OnWindowResize((int)size.x, (int)size.y);
	}
}

bool	Window::UpdateWindow() {
	std::this_thread::yield();
	timer->Tick();

	if (mouse) {
		mouse->UpdateFrameState(timer->GetTimeDeltaMSec());
	}
	if (keyboard) {
		keyboard->UpdateFrameState(timer->GetTimeDeltaMSec());
	}

	return InternalUpdate();
}

void Window::ResizeRenderer() {
	if (renderer) {
		renderer->OnWindowResize((int)size.x, (int)size.y);
	}
}