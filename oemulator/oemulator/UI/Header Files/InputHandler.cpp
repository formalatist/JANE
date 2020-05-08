#include "InputHandler.h"

namespace Input {
	InputState InputHandler::inputState = InputState();
	
	const InputState& InputHandler::handleInput()
	{
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
				
				//controller.onKeyUp(event.key.keysym.sym);
				//IO->keyboardState[event.key.keysym.sym] = false;
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
				//IO->LMBPressed = event.button.button == SDL_BUTTON_LEFT;
				//IO->RMBPressed = event.button.button == SDL_BUTTON_RIGHT;
				inputState.heldLMB = event.button.button == SDL_BUTTON_LEFT;
				inputState.heldRMB = event.button.button == SDL_BUTTON_RIGHT;
			}
			else if (event.type == SDL_MOUSEWHEEL) {
				//IO->scrollwheelY = event.wheel.y;
				inputState.scrollWheelDelta = event.wheel.y;
			}
			else if (event.type == SDL_DROPFILE) { // a file was dropped on the window
				//dir = event.drop.file;
				//SDL_free(dir);
			}
		}
		return inputState;
	}

}
