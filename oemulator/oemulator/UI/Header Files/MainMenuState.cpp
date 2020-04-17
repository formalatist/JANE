#include "MainMenuState.h"

UI::MainMenuState::MainMenuState(UIFSM * UIFSM_) : UIState(UIFSM_)
{
	UIElements = std::vector < UIElement*>();

	auto btn = new Button(SDL_Rect{ 64, 64, 128, 16 }, SDL_Color{ 40, 120, 240, 255 },
		[]() {printf("button onclick\n"); });

	UIElements.push_back(btn);
}

void UI::MainMenuState::update(float d)
{
	for (auto e : UIElements) {
		e->update(d);
	}
}

void UI::MainMenuState::draw(SDL_Renderer *renderer, int scale)
{
	for (auto e : UIElements) {
		e->draw(renderer, scale);
	}
}
