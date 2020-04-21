#pragma once
#include <SDL.h>

namespace UI {
	typedef struct Rect {
		int x, y, w, h;

		operator SDL_Rect() const {
			return SDL_Rect{ x,y,w,h };
		}

		Rect operator*(const int rhs) {
			return Rect{ x*rhs, y*rhs, w*rhs, h*rhs };
		}
	} Rect;
}
