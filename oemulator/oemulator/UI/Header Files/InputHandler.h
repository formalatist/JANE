#pragma once
#include <map>
#include <vector>
#include <SDL.h>
#include <functional>

#include "InputState.h"

namespace Input {
	class InputHandler {
	public:
		static const InputState& handleInput();

	private:
		static InputState inputState;
	};
}