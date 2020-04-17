#pragma once
#include <SDL.h>

namespace UI {
	static bool mouseInRect(int mouseX, int mouseY, SDL_Rect rect) {
		if (mouseX >= rect.x && mouseX <= (rect.x + rect.w) && mouseY > rect.y && mouseY < (rect.y + rect.h)) {
			return true;
		}
		return false;
	}
}