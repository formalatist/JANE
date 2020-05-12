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

		static void searchForAndAddGameController();
	private:
		static InputState inputState;

		static void updateMouseState(int SCREEN_SCALE);
		static void updateControllerAxisState();

		static SDL_GameController* controller;
		static const float AXIS_DEADZONE;
		static const float AXIS_MAX_VALUE;
		static const std::vector<SDL_GameControllerAxis> allGameControllerAxis;
	};
}