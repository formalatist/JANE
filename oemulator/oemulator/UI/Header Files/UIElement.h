#pragma once

#include <SDL.h>

#include "InputState.h"
#include "UIUtil.h"

namespace UI {
	class UIElement {
	public:
		UIElement(SDL_Rect rect_) : rect(rect_) {};

		virtual void update(const Input::InputState& input, float d) = 0;
		virtual void draw(SDL_Renderer *renderer, int scale) = 0;

		SDL_Rect rect;
	};
}