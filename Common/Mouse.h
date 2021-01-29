/******************************************************************************
Class:Mouse
Implements:
Author:Rich Davison
Description:TODO

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Vector2.h"

namespace NCL {
	using namespace NCL::Maths;
	enum class MouseButtons {
		LEFT		= 0,
		RIGHT		= 1,
		MIDDLE		= 2,
		FOUR		= 3,
		FIVE		= 4,
		MAXVAL		= 5
	};

	class Mouse {
	public:
		friend class Window;
		inline bool ButtonPressed(MouseButtons button) const {
			return buttons[(int)button] && !holdButtons[(int)button];
		}

		//Is this mouse button currently pressed down?
		inline bool	ButtonDown(MouseButtons button) const { return buttons[(int)button]; }
		//Has this mouse button been held down for multiple frames?
		inline bool	ButtonHeld(MouseButtons button) const { return buttons[(int)button] && holdButtons[(int)button]; }
		//Has this mouse button been double clicked?
		inline bool	DoubleClicked(MouseButtons button) const {
			return (buttons[(int)button] && doubleClicks[(int)button]);
		}

		//Get how much this mouse has moved since last frame
		inline Vector2	GetRelativePosition() const { return relativePosition; }
		//Get the window position of the mouse pointer
		inline Vector2	GetAbsolutePosition() const { return absolutePosition; }

		inline Vector2 GetWindowPosition() const {
			return windowPosition;
		}

		//Has the mouse wheel moved since the last update?
		inline bool	WheelMoved() const { return frameWheel != 0; }
		//Get the mousewheel movement. Positive means scroll up,
		//negative means scroll down, 0 means no movement.
		inline int	GetWheelMovement() const { return (int)frameWheel; }

		//Sets the mouse sensitivity. Currently only affects the 'relative'
		//(i.e FPS-style) mouse movement. Students! Maybe you'd like to
		//implement a 'MenuSensitivity' for absolute movement?
		inline void	SetMouseSensitivity(float amount) {
			if (amount == 0.0f) {
				amount = 1.0f;
			}
			sensitivity = amount;
		}

		//Determines the maximum amount of ms that can pass between
		//2 mouse presses while still counting as a 'double click'
		void	SetDoubleClickLimit(float msec) { clickLimit = msec; }

	protected:
		Mouse();
		virtual ~Mouse() {}

		//Set the mouse's current screen position. Maybe should be public?
		void	SetAbsolutePosition(const Vector2& pos);

		//Set the absolute screen bounds (<0 is always assumed dissallowed). Used
		//by the window resize routine...
		void	SetAbsolutePositionBounds(const Vector2& bounds);

		void	UpdateFrameState(float msec);
		void	Sleep();
		void	Wake();

		bool		isAwake;		//Is the device awake...

		//Screen position if windowing API provides it
		Vector2		windowPosition;
		//Current mouse absolute position
		Vector2		absolutePosition;
		//Current mouse absolute position maximum bounds
		Vector2		absolutePositionBounds;
		//How much as the mouse moved since the last raw packet?
		Vector2		relativePosition;
		//Current button down state for each button
		bool		buttons[(int)MouseButtons::MAXVAL];
		//Current button held state for each button
		bool		holdButtons[(int)MouseButtons::MAXVAL];
		//Current doubleClick counter for each button
		bool		doubleClicks[(int)MouseButtons::MAXVAL];
		//Counter to remember when last mouse click occured
		float		lastClickTime[(int)MouseButtons::MAXVAL];

		//last mousewheel updated position
		int			lastWheel;

		//Current mousewheel updated position
		int			frameWheel;

		//Max amount of ms between clicks count as a 'double click'
		float		clickLimit;

		//Mouse pointer sensitivity. Set this negative to get a headache!
		float		sensitivity;
	};
}