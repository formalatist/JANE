#include "ImageButton.h"

void UI::ImageButton::update(const Input::InputState& input, float d)
{
	int mouseX, mouseY;
	input.getMousePosition(mouseX, mouseY);
	if (mouseInRect(mouseX, mouseY, rect)) {
		if (input.isLMBPressed()) {
			onClick();
		}
	}
}

void UI::ImageButton::draw(SDL_Renderer * renderer, int scale)
{
	auto r = SDL_Rect{ rect.x*scale,rect.y*scale,rect.w*scale,rect.h*scale };
	SDL_RenderCopy(renderer, texture, NULL, &r);
}
