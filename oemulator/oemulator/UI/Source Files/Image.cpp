#include "Image.h"

void UI::Image::update(float d)
{
}

void UI::Image::draw(SDL_Renderer * renderer, int scale)
{
	auto r = SDL_Rect{ rect.x*scale,rect.y*scale,rect.w*scale,rect.h*scale };
	SDL_RenderCopy(renderer, texture, NULL, &r);
}
