#pragma once
#include <map>
#include <SDL.h>

namespace UI {
	typedef struct UIInput {
		int mouseX;
		int mouseY;
		bool LMBDown;		//always true when LMB is held
		bool LMBPressed;	//true the frame LMB is pressed
		bool RMBDown;		//always true when RMB is held
		bool RMBPressed;	//true the frame RMB is pressed
		int scrollwheelY;
		//keep track of keyboard buttons
		std::map<SDL_Keycode, bool> keyboardState;
	} UIInput;

	//global input state, this is update from wherever the sdl event loop is
	extern UIInput input;
}

