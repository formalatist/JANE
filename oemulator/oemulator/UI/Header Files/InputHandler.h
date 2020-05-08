#pragma once
#include <map>
#include <vector>
#include <SDL.h>
#include <functional>

#include "InputState.h"

namespace Input {
	enum InputEventType {};

	class InputHandler {
	public:
		 static void init();

		static const InputState& handleInput();

	private:
		static InputState inputState;
		static keycodeMap pressedKeys;
		static keycodeMap heldKeys;
		static keycodeMap releasedKeys;
	};
}