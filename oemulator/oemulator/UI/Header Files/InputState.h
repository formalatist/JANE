#pragma once
#include <vector>
#include <map>
#include <SDL.h>


namespace Input {
	typedef std::map<SDL_Keycode, bool> keycodeMap;

	class InputState {
		friend class InputHandler;

	public:
		InputState();

		bool isKeyPressed(SDL_Keycode key) const;
		bool isKeyHeld(SDL_Keycode key) const;
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