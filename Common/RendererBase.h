/******************************************************************************
Class:RendererBase
Implements:
Author:Rich Davison
Description:TODO

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Window.h"

namespace NCL {
	namespace Rendering {
		enum class VerticalSyncState {
			VSync_ON,
			VSync_OFF,
			VSync_ADAPTIVE
		};
		class RendererBase {
		public:
			friend class NCL::Window;

			RendererBase(Window& w);
			virtual ~RendererBase();

			virtual bool HasInitialised() const {return true;}

			virtual void Update(float dt) {}

			void Render() {
				BeginFrame();
				RenderFrame();
				EndFrame();
				SwapBuffers();
			}

			virtual bool SetVerticalSync(VerticalSyncState s) {
				return false;
			}

		protected:
			virtual void OnWindowResize(int w, int h) = 0;
			virtual void OnWindowDetach() {}; //Most renderers won't care about this
			
			virtual void BeginFrame()	= 0;
			virtual void RenderFrame()	= 0;
			virtual void EndFrame()		= 0;
			virtual void SwapBuffers()	= 0;
			Window& hostWindow;

			int currentWidth;
			int currentHeight;
		};
	}
}
