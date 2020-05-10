#include "InputHandler.h"
#include <iostream>

namespace Input {
	InputState InputHandler::inputState = InputState();
	
	const InputState& InputHandler::getInput(int SCREEN_SCALE)
	{
		/*When handling input in SDL it is importan to note that when holding a 
		keyboard key down, a SDL_KEYDOWN event is created every frame, but the same 
		does not happen when, for example, holding a mouse button. These two events
		needs to be handled diffrently so that the API can be equal for both. 
		"isPressed" should only be true for one frame when the button was pressed,
		same with released*/

		//reset states that should only be true for the frames they happen, like keys pressed
		inputState.pressedLMB = false;
		inputState.pressedRMB = false;
		for (const auto& value : inputState.pressedKeys) {
			inputState.pressedKeys[value.first] = false;
		}
		for (const auto& value : inputState.releasedKeys) {
			inputState.releasedKeys[value.first] = false;
		}
		for (const auto& value : inputState.pressedControllerButtons) {
			inputState.pressedControllerButtons[value.first] = false;
		}
		
		//read events, this changes the state of buttons etc
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				inputState.quitRequested = true;
			}
			else if (event.type == SDL_KEYDOWN) {
				if(!inputState.heldKeys[event.key.keysym.sym]) { //key isnt held, this is a new event for the key
					inputState.pressedKeys[event.key.keysym.sym] = true;
					inputState.heldKeys[event.key.keysym.sym] = true;
				}
			}
			else if (event.type == SDL_KEYUP) {
				inputState.heldKeys[event.key.keysym.sym] = false;
				inputState.releasedKeys[event.key.keysym.sym] = true;
			}
			else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
				inputState.pressedControllerButtons[event.cbutton.button] = true;
				inputState.heldControllerButtons[event.cbutton.button] = true;
			}
			else if (event.type == SDL_CONTROLLERBUTTONUP) {
				inputState.heldControllerButtons[event.cbutton.button] = false;
			}
			else if (event.type == SDL_CONTROLLERAXISMOTION) {
				//controller.onGameControllerAxisMotion(event.caxis.axis, event.caxis.value);
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					inputState.heldLMB = true;
					inputState.pressedLMB = true;
				}
				else if (event.button.button = SDL_BUTTON_RIGHT) {
					inputState.heldRMB = true;
					inputState.pressedRMB = true;
				}
			}
			else if (event.type == SDL_MOUSEBUTTONUP) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					inputState.heldLMB = false;
				}
				else if (event.button.button == SDL_BUTTON_RIGHT) {
					inputState.heldRMB = false;
				}
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
