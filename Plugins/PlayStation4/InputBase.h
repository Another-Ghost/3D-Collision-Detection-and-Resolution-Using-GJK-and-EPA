#pragma once

namespace NCL {
	namespace PS4 {
		struct AXIS {
			float x;
			float y;

			AXIS() {
				x = 0.0f;
				y = 0.0f;
			}
		};

#define MAX_AXIS 8
#define MAX_BUTTONS 127

		class InputBase
		{
		public:
			InputBase();
			~InputBase();

			void ResetInput();

			AXIS	GetAxis(unsigned int i);
			float	GetButton(unsigned int i);
			bool	GetButtonDown(unsigned int i);


		protected:

			AXIS	axis[MAX_AXIS];
			float   buttons[MAX_BUTTONS];
		};
	}
}
