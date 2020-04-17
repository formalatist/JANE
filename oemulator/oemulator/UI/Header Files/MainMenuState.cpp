#include "MainMenuState.h"

UI::MainMenuState::MainMenuState(UIFSM * UIFSM_) : UIState(UIFSM_)
{
	UIElements = std::vector < UIElement*>();

	auto btn = new Button(SDL_Rect{20, 20, 30, 30}, 
		[]() {printf("button onclick"); });

	UIElements.push_back(btn);
}

void UI::MainMenuState::update(float d)
{
	for (auto e : UIElements) {
		e->update(d);
	}
}

void UI::MainMenuState::draw(SDL_Renderer *renderer)
{
	for (auto e : UIElements) {
		e->draw(renderer);
	}
}
