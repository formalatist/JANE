#include "InputHandler.h"

namespace Input {
	InputState InputHandler::inputState = InputState();
	
	const InputState& InputHandler::getInput(int SCREEN_SCALE)
	{
		//read events, this changes the state of buttons etc
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				inputState.quitRequested = true;
			}
			else if (event.type == SDL_KEYDOWN) {
				inputState.pressedKeys[event.key.keysym.sym] = true;
				inputState.heldKeys[event.key.keysym.sym] = true;
				inputState.releasedKeys[event.key.keysym.sym] = false;
			}
			else if (event.type == SDL_KEYUP) {
				inputState.pressedKeys[event.key.keysym.sym] = false;
				inputState.heldKeys[event.key.keysym.sym] = false;
				inputState.releasedKeys[event.key.keysym.sym] = true;
			}
			else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
				//controller.onGameControllerDown(event.cbutton.button);
			}
			else if (event.type == SDL_CONTROLLERBUTTONUP) {
				//controller.onGameControllerUp(event.cbutton.button);
			}
			else if (event.type == SDL_CONTROLLERAXISMOTION) {
				//controller.onGameControllerAxisMotion(event.caxis.axis, event.caxis.value);
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				inputState.heldLMB = event.button.button == SDL_BUTTON_LEFT;
				inputState.pressedLMB = event.button.button == SDL_BUTTON_LEFT;
				inputState.heldRMB = event.button.button == SDL_BUTTON_RIGHT;
				inputState.pressedRMB = event.button.button == SDL_BUTTON_RIGHT;
			}
			else if (event.type == SDL_MOUSEBUTTONUP) {
				inputState.heldLMB = !(event.button.button == SDL_BUTTON_LEFT);
				inputState.pressedLMB = !(event.button.button == SDL_BUTTON_LEFT);
				inputState.heldRMB = !(event.button.button == SDL_BUTTON_RIGHT);
				inputState.pressedRMB = !(event.button.button == SDL_BUTTON_RIGHT);
			}
			else if (event.type == SDL_MOUSEWHEEL) {
				inputState.scrollWheelDelta = event.wheel.y;
			}
			else if (event.type == SDL_DROPFILE) { // a file was dropped on the window
				//dir = event.drop.file;
				//SDL_free(dir);
			}
		}
		
		//update inputs that happen constantly, like mouse position
		SDL_GetMouseState(&inputState.mouseX, &inputState.mouseY);
		//adjust for the scale the emulator is running at
		inputState.mouseX /= SCREEN_SCALE;
		inputState.mouseY /= SCREEN_SCALE;

		return inputState;
	}

}
