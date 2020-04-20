#pragma once
#include <functional>

#include "UIElement.h"

namespace UI {
	class ImageButton : public UIElement {
	public:
		ImageButton(SDL_Rect rect_, SDL_Texture *texture_, std::function<void()> onClick_) : UIElement(rect_), texture(texture_), onClick(onClick_) {};

		void update(float d) override;
		void draw(SDL_Renderer *renderer, int scale) override;

		void onHover();

		std::function<void()> onClick;

	private:
		SDL_Texture *texture;
	};
}