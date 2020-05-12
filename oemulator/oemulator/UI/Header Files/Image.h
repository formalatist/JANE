#pragma once

#include "UIElement.h"

namespace UI {

	class Image : public UIElement {

	public:
		Image(SDL_Rect rect_, SDL_Texture *texture_) : UIElement(rect_), texture(texture_) {};

		void update(const Input::InputState& input, float d) override;
		void draw(SDL_Renderer *renderer, int scale) override;

	private:
		SDL_Texture *texture;
	};
}
