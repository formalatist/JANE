#pragma once
#include <map>
#include <SDL.h>


namespace Input {
	typedef Uint8 Input_Axisdirection;
	enum {
		INPUT_LEFT_AXIS_LEFT = 0,
		INPUT_LEFT_AXIS_RIGHT = 1,
		INPUT_LEFT_AXIS_UP = 2,
		INPUT_LEFT_AXIS_DOWN = 3,
		INPUT_RIGHT_AXIS_LEFT = 4,
		INPUT_RIGHT_AXIS_RIGHT = 5,
		INPUT_RIGHT_AXIS_UP = 6,
		INPUT_RIGHT_AXIS_DOWN = 7,
	};

	typedef std::map<SDL_Keycode, bool> keycodeMap;
	typedef std::map<Uint8, bool> controllerButtonMap;
	typedef std::map<Input_Axisdirection, bool> axisdirectionMap;

	class InputState {
		friend class InputHandler;

	public:
		bool isKeyPressed(SDL_Keycode key) const;
		bool isKeyHeld(SDL_Keycode key) const;
		bool isKeyReleased(SDL_Keycode key) const;
		bool isControllerButtonPressed(Uint8 button) const;
		bool isControllerButtonHeld(Uint8 button) const;
		bool isAxisDirectionHeld(Input_Axisdirection dir) const;
		void getMousePosition(int& x, int& y) const;
		bool isLMBHeld() const;
		bool isLMBPressed() const;
		bool isRMBHeld() const;
		bool isRMBPressed() const;
		int getScrollWheelDelta() const;
		bool isQuitRequested() const;

	private:
		//keyboard keys state
		keycodeMap pressedKeys;
		keycodeMap heldKeys;
		keycodeMap releasedKeys;

		controllerButtonMap pressedControllerButtons;
		controllerButtonMap heldControllerButtons;

		axisdirectionMap heldAxisdirection;

		int mouseX;
		int mouseY;

		bool heldLMB;
		bool pressedLMB;

		bool heldRMB;
		bool pressedRMB;

		int scrollWheelDelta;

		//has a quit been requested this frame
		bool quitRequested;
		
	};
}