#include "ImageButton.h"

void UI::ImageButton::update(float d)
{
	auto &IO = input;
	if (mouseInRect(IO.mouseX, IO.mouseY, rect)) {
		if (IO.LMBPressed) {
			onClick();
		}
	}
}

void UI::ImageButton::draw(SDL_Renderer * renderer, int scale)
{
	auto r = SDL_Rect{ rect.x*scale,rect.y*scale,rect.w*scale,rect.h*scale };
	SDL_RenderCopy(renderer, texture, NULL, &r);
}
