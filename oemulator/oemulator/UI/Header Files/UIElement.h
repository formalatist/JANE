#pragma once

#include <SDL.h>

#include "UIInput.h"

namespace UI {
	class UIElement {
	public:
		virtual void update(float d) = 0;
		virtual void draw(SDL_Renderer *renderer) = 0;
	};
}