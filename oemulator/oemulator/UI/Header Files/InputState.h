#pragma once
#include <vector>
#include <map>
#include <SDL.h>


namespace Input {
	typedef std::map<SDL_Keycode, bool> keycodeMap;

	class InputState {
		friend class InputHandler;

	public:
		InputState(const keycodeMap& pressedKeys_, const keycodeMap& heldKeys_);

		bool isKeyPressed(SDL_Keycode key);
		bool isKeyHeld(SDL_Keycode key);
		const std::vector<SDL_Keycode>& getReleasedKeys();
		const std::vector<SDL_Keycode>& getHeldKeys();

	private:
		const keycodeMap& pressedKeys;
		const keycodeMap& heldKeys;
		std::vector<SDL_Keycode>* releasedKeys;
		//std::vector<SDL_Keycode>* heldKeys;
		
	};
}