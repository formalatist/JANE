#pragma once
#include <functional>

#include "UIElement.h"

namespace UI {
	class Button : public UIElement{
	public:
		Button(SDL_Rect rect_, SDL_Color color_, std::function<void()> onClick_) : UIElement(rect_), color(color_), onClick(onClick_) {};

		void update(float d) override;
		void draw(SDL_Renderer *renderer, int scale) override;

		SDL_Color color;
		void onHover();

		std::function<void()> onClick;
	};
}