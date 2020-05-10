#pragma once
#include <map>
#include <vector>
#include <SDL.h>
#include <functional>

#include "InputState.h"

namespace Input {
	class InputHandler {
	public:
		static const InputState& getInput(int SCREEN_SCALE);
	private:
		static InputState inputState;

		static float AXIS_DEADZONE;
		static float AXIS_MAX_VALUE;
	};
}