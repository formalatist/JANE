#include "GameActiveState.h"

UI::GameActiveState::GameActiveState(UIFSM * UIFSM_) : UIState(UIFSM_) 
{
	UIElements.push_back(new Button(SDL_Rect{96,0,64,8}, SDL_Color{ 0,0,0,0 }, 
		[this]() {
		this->FSM->changeState("ROMLibrary");
		}));
}

void UI::GameActiveState::update(float d)
{
	for (auto e : UIElements) {
		e->update(d);
	}
}

void UI::GameActiveState::draw(SDL_Renderer * renderer, int scale)
{
	for (auto e : UIElements) {
		e->draw(renderer, scale);
	}
}
