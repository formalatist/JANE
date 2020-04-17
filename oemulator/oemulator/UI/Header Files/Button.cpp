#include "Button.h"

void UI::Button::update(float d)
{
}

void UI::Button::draw(SDL_Renderer * renderer)
{
	SDL_RenderFillRect(renderer, &rect);
}
