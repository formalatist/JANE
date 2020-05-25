#pragma once
#include <map>
#include <SDL.h>


namespace Input {
	enum AxisDirection {
		positive, negative,
	};

	typedef std::map<SDL_Keycode, bool> keycodeMap;
	typedef std::map<Uint8, bool> controllerButtonMap;
	typedef std::map<SDL_GameControllerAxis, std::map<AxisDirection, bool>> controllerAxisMap;

	class InputState {
		friend class InputHandler;

	public:
		bool isKeyPressed(SDL_Keycode key) const;
		bool isKeyHeld(SDL_Keycode key) const;
		bool isKeyReleased(SDL_Keycode key) const;
		bool isControllerButtonPressed(Uint8 button) const;
		bool isControllerButtonHeld(Uint8 button) const;
		bool isAxisDirectionHeld(SDL_GameControllerAxis axis, AxisDirection dir) const;
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

		//axisdirectionMap heldAxisdirection;
		controllerAxisMap heldAxisDirections;

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