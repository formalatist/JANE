#include "InputState.h"

Input::InputState::InputState() : pressedKeys(keycodeMap()), heldKeys(keycodeMap()) {}

bool Input::InputState::isKeyPressed(SDL_Keycode key) const
{
	if (pressedKeys.find(key) != pressedKeys.end()) {
		return pressedKeys.at(key);
	}
	return false;
}

bool Input::InputState::isKeyHeld(SDL_Keycode key) const
{
	if (heldKeys.find(key) != heldKeys.end()) {
		return heldKeys.at(key);
	}
	return false;
}

bool Input::InputState::isKeyReleased(SDL_Keycode key) const
{
	if (heldKeys.find(key) != heldKeys.end()) {
		return heldKeys.at(key);
	}
	return false;
}

void Input::InputState::getMousePosition(int & x, int & y) const
{
	x = mouseX;
	y = mouseY;
}

bool Input::InputState::isLMBHeld() const
{
	return heldLMB;
}

bool Input::InputState::isLMBPressed() const
{
	return pressedLMB;
}

bool Input::InputState::isRMBHeld() const
{
	return heldRMB;
}

bool Input::InputState::isRMBPressed() const
{
	return pressedRMB;
}

int Input::InputState::getScrollWheelDelta() const
{
	return scrollWheelDelta;
}

bool Input::InputState::isQuitRequested() const
{
	return quitRequested;
}
