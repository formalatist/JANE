#include "Button.h"
#include <iostream>

void UI::Button::update(const Input::InputState& input, float d)
{
	int mouseX, mouseY;
	input.getMousePosition(mouseX, mouseY);
	if (mouseInRect(mouseX, mouseY, rect)) {
		if (input.isLMBPressed()) {
			onClick();
		}
		color = SDL_Color{ 200, 150, 70, 255 };
	}
	else {
		color = SDL_Color{ 70,70,70,255 };
	}
}

void UI::Button::draw(SDL_Renderer * renderer, int scale)
{
	auto r = SDL_Rect{ rect.x*scale,rect.y*scale,rect.w*scale,rect.h*scale };
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &r);
	SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
	SDL_RenderDrawRect(renderer, &r);
}
