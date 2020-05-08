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

		bool isKeyPressed(SDL_Keycode key);
		bool isKeyHeld(SDL_Keycode key);
		void getMousePosition(int& x, int& y);
		bool isLMBHeld();
		bool isRMBHeld();
		int getScrollWheelDelta();
		bool isQuitRequested();

	private:
		//keyboard keys state
		keycodeMap pressedKeys;
		keycodeMap heldKeys;
		keycodeMap releasedKeys;

		int mouseX;
		int mouseY;

		bool heldLMB;
		bool heldRMB;

		int scrollWheelDelta;

		//has a quit been requested this frame
		bool quitRequested;
		
	};
}