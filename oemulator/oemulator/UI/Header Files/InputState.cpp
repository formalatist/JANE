#include "InputState.h"

Input::InputState::InputState(const keycodeMap& pressedKeys_, const keycodeMap& heldKeys_) : pressedKeys(pressedKeys_), heldKeys(heldKeys_) {}

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
