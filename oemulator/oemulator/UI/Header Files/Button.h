#pragma once
#include <functional>

#include "UIElement.h"

namespace UI {
	class Button : public UIElement{
	public:
		Button(SDL_Rect rect_, std::function<void()> onClick_) : rect(rect_), onClick(onClick_) {};

		void update(float d) override;
		void draw(SDL_Renderer *renderer) override;

	private:
		SDL_Rect rect;
		void onHover();

		std::function<void()> onClick;
	};
}