#include "InputBase.h"

using namespace NCL::PS4;

InputBase::InputBase()
{
	ResetInput();
}


InputBase::~InputBase()
{
}

void InputBase::ResetInput() {
	for (int i = 0; i < MAX_AXIS; ++i) {
		axis[i].x = 0.0f;
		axis[i].y = 0.0f;
	}
	for (int i = 0; i < MAX_BUTTONS; ++i) {
		buttons[i] = 0.0f;
	}
}

AXIS	InputBase::GetAxis(unsigned int i) {
	if (i > MAX_AXIS) {
		return AXIS();
	}
	return axis[i];
}

float	InputBase::GetButton(unsigned int i) {
	if (i > MAX_BUTTONS) {
		return 0.0f;
	}
	return buttons[i];
}

bool	InputBase::GetButtonDown(unsigned int i) {
	if (i > MAX_BUTTONS) {
		return false;
	}
	return buttons[i] > 0.5f ? true : false;
}