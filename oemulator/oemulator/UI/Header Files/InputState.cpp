#include "InputState.h"

Input::InputState::InputState() : pressedKeys(keycodeMap()), heldKeys(keycodeMap()) {}

bool Input::InputState::isKeyPressed(SDL_Keycode key)
{
	if (pressedKeys.find(key) != pressedKeys.end()) {
		return pressedKeys.at(key);
	}
	return false;
}

bool Input::InputState::isKeyHeld(SDL_Keycode key)
{
	return false;
}

void Input::InputState::getMousePosition(int & x, int & y)
{
	x = mouseX;
	y = mouseY;
}

bool Input::InputState::isLMBHeld()
{
	return heldLMB;
}

bool Input::InputState::isRMBHeld()
{
	return heldRMB;
}

int Input::InputState::getScrollWheelDelta()
{
	return scrollWheelDelta;
}

bool Input::InputState::isQuitRequested()
{
	return quitRequested;
}
